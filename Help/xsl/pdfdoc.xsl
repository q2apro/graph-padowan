<xsl:stylesheet
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns:fo="http://www.w3.org/1999/XSL/Format"
 version="1.0">

  <xsl:import href="./db_pdfdoc.xsl"/>
  <xsl:import href="./defaults.xsl"/>
  <xsl:import href="./titlepage.xsl"/>

  <xsl:param name="fop1.extensions" select="1" />
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="double.sided" select="0"/>
  <xsl:param name="variablelist.as.blocks" select="1"/>
  <!--xsl:param name="symbol.font.family">Symbol,ZapfDingbats</xsl:param-->
  <xsl:param name="symbol.font.family">serif</xsl:param> <!-- This avoids a strange warning -->
  <xsl:param name="body.start.indent">0pt</xsl:param> <!-- Body text indention -->
  <xsl:param name="title.margin.left">-1em</xsl:param> <!-- Body text indention -->
  <xsl:param name="generate.index" select="0" /> <!-- Don't generate index for pdf file -->
  <xsl:param name="hyphenate">false</xsl:param>  <!-- Disable hyphernation. It doesn't seem to work anyway. -->
	<xsl:param name="alignment">left</xsl:param>

  <xsl:param name="table.frame.border.thickness" select="'1pt'"/>
  <xsl:param name="table.frame.border.style" select="'solid'"/>
  <xsl:param name="table.frame.border.color" select="'#7099C5'"/>
  <xsl:param name="table.cell.border.thickness" select="'1pt'"/>
  <xsl:param name="table.cell.border.style" select="'solid'"/>
  <xsl:param name="table.cell.border.color" select="'#7099C5'"/>

  <xsl:param name="img.src.path">Images/</xsl:param>
  <xsl:param name="glossary.as.blocks" select="1"/>

  <!-- Add shadings and border to program listings -->
  <xsl:param name="shade.verbatim" select="1"/>
  <xsl:attribute-set name="shade.verbatim.style">
    <xsl:attribute name="background-color">#E0E0E0</xsl:attribute>
    <xsl:attribute name="border-width">0.5pt</xsl:attribute>
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-color">#575757</xsl:attribute>
    <xsl:attribute name="padding">3pt</xsl:attribute>
  </xsl:attribute-set>

  <xsl:param name="formal.title.placement">
    figure after
    example after
    equation after
    table after
    procedure after
  </xsl:param>

  <xsl:template match="property">
    <fo:inline font-weight="bold">
      <xsl:call-template name="inline.charseq"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="guilabel">
    <fo:inline font-family="Helvetica" font-style="italic">
      <xsl:call-template name="inline.charseq"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="personname">
    <fo:inline font-family="Times" font-style="normal">
      <xsl:call-template name="inline.charseq"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="menuchoice">
    <fo:inline font-family="Helvetica">
      <xsl:call-template name="process.menuchoice"/>
      <xsl:variable name="shortcut" select="./shortcut"/>
      <xsl:if test="$shortcut">
        <xsl:text> (</xsl:text>
        <xsl:apply-templates select="$shortcut"/>
        <xsl:text>)</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="guibutton|guimenu|guisubmenu|guimenuitem">
    <fo:inline font-family="Helvetica">
      <xsl:call-template name="inline.charseq"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="keysym">
    <fo:inline font-family="Symbol">
      <xsl:call-template name="inline.charseq"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="thead">
    <xsl:variable name="tgroup" select="parent::*"/>
    <fo:table-header font-weight="bold" color="#ffffff" background-color="#7099C5">
      <xsl:apply-templates select="row[1]">
        <xsl:with-param name="spans">
          <xsl:call-template name="blank.spans">
            <xsl:with-param name="cols" select="../@cols"/>
          </xsl:call-template>
        </xsl:with-param>
      </xsl:apply-templates>
    </fo:table-header>
  </xsl:template>

  <xsl:template match="tfoot">
    <xsl:variable name="tgroup" select="parent::*"/>
    <fo:table-footer background-color="#f0f0ff">
      <xsl:apply-templates select="row[1]">
        <xsl:with-param name="spans">
          <xsl:call-template name="blank.spans">
            <xsl:with-param name="cols" select="../@cols"/>
          </xsl:call-template>
        </xsl:with-param>
      </xsl:apply-templates>
    </fo:table-footer>
  </xsl:template>

  <xsl:template match="tbody">
    <xsl:variable name="tgroup" select="parent::*"/>
    <fo:table-body start-indent="0pt" end-indent="0pt" background-color="#f0f0ff">
    <xsl:apply-templates select="row[1]">
        <xsl:with-param name="spans">
          <xsl:call-template name="blank.spans">
            <xsl:with-param name="cols" select="../@cols"/>
          </xsl:call-template>-
        </xsl:with-param>
      </xsl:apply-templates>
    </fo:table-body>
  </xsl:template>

  <xsl:attribute-set name="xref.properties">
    <xsl:attribute name="color">blue</xsl:attribute>
  </xsl:attribute-set>

  <xsl:template match="markup[@role = 'symbolfont']">
    <fo:inline font-family="Symbol" font-style="normal">
      <xsl:call-template name="inline.charseq"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="symbol[@role = 'html']" />

  <xsl:attribute-set name="section.title.level1.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 2.00"/>
      <xsl:text>pt</xsl:text>
    </xsl:attribute>
    <xsl:attribute name="keep-with-next.within-column">always</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="section.title.level2.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 1.40"/>
      <xsl:text>pt</xsl:text>
    </xsl:attribute>
    <xsl:attribute name="keep-with-next.within-column">always</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="section.title.level3.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master"/>
      <xsl:text>pt</xsl:text>
    </xsl:attribute>
    <xsl:attribute name="keep-with-next.within-column">always</xsl:attribute>
  </xsl:attribute-set>

	<xsl:attribute-set name="monospace.verbatim.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 0.80"/>
    </xsl:attribute>
    <xsl:attribute name="wrap-option">wrap</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="normal.para.spacing">
    <xsl:attribute name="space-before.optimum">0em</xsl:attribute>
    <xsl:attribute name="space-before.minimum">0em</xsl:attribute>
    <xsl:attribute name="space-before.maximum">0em</xsl:attribute>
    <xsl:attribute name="space-after.optimum">1.0em</xsl:attribute>
    <xsl:attribute name="space-after.minimum">0.8em</xsl:attribute>
    <xsl:attribute name="space-after.maximum">1.2em</xsl:attribute>
  </xsl:attribute-set>

  <!-- Make it possible to use <?hard-pagebreak?> in the xml to force a hard page break -->
  <xsl:template match="processing-instruction('hard-pagebreak')">
    <fo:block break-after='page'/>
  </xsl:template>

  <!-- Remove the <indexterm> from the processing as it else will prevent keep-with-next from working
       because the <indexterm> will be between <title> and <para> in the output. -->
  <xsl:template match="indexterm" />

  <!-- Show "Translator:" followed by the list of translators, one on each line -->
  <xsl:template match="othercredit" mode="titlepage.mode">
    <xsl:variable name="contrib" select="string(contrib)"/>
    <xsl:if test="not(preceding-sibling::othercredit[string(contrib)=$contrib])">
      <fo:block>
      <xsl:apply-templates mode="titlepage.mode" select="contrib"/>
      </fo:block>
    </xsl:if>
    <xsl:call-template name="person.name"/>
  </xsl:template>

  <!-- Copied from fo\lists.xsl in DocBook and changed to not show comma before "and" -->
  <xsl:template match="simplelist[@type='inline']">
    <!-- if dbchoice PI exists, use that to determine the choice separator -->
    <!-- (that is, equivalent of "and" or "or" in current locale), or literal -->
    <!-- value of "choice" otherwise -->
    <fo:inline><xsl:variable name="localized-choice-separator">
      <xsl:choose>
        <xsl:when test="processing-instruction('dbchoice')">
          <xsl:call-template name="select.choice.separator"/>
        </xsl:when>
        <xsl:otherwise>
          <!-- empty -->
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:for-each select="member">
      <xsl:apply-templates/>
      <xsl:choose>
        <xsl:when test="position() &lt; last() - 1">
          <xsl:text>, </xsl:text>
        </xsl:when>
      </xsl:choose>
      <xsl:if test="position() = last() - 1">
        <xsl:if test="$localized-choice-separator != ''">
          <xsl:text> </xsl:text>
          <xsl:value-of select="$localized-choice-separator"/>
          <xsl:text> </xsl:text>
        </xsl:if>
      </xsl:if>
    </xsl:for-each></fo:inline>
  </xsl:template>

</xsl:stylesheet>

