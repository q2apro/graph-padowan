<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:param name="draft.mode" select="no"/>

  <xsl:param name="section.autolabel" select="0" />
  <xsl:param name="section.label.includes.component.label" select="1" />
  <xsl:param name="chapter.autolabel" select="0"/>
  <xsl:param name="qanda.defaultlabel">qanda</xsl:param>

  <xsl:attribute-set name="table.cell.padding">
    <xsl:attribute name="padding-left">4pt</xsl:attribute>
    <xsl:attribute name="padding-right">4pt</xsl:attribute>
    <xsl:attribute name="padding-top">2pt</xsl:attribute>
    <xsl:attribute name="padding-bottom">2pt</xsl:attribute>
  </xsl:attribute-set>


  <xsl:param name="admon.graphics" select="1"/>
  <xsl:param name="email.delimiters.enabled" select="0"/> <!-- Don't show angle brackets around email addresses -->

  <xsl:attribute-set name="admonition.title.properties">
    <xsl:attribute name="font-size">14pt</xsl:attribute>
    <xsl:attribute name="font-weight">bold</xsl:attribute>
    <xsl:attribute name="hyphenate">false</xsl:attribute>
    <xsl:attribute name="keep-with-next.within-column">always</xsl:attribute>

    <xsl:attribute name="padding">.33em 0 0 5px</xsl:attribute>

    <xsl:attribute name="border-top">3px solid</xsl:attribute>
    <xsl:attribute name="border-left">1px solid</xsl:attribute>

    <xsl:attribute name="border-color">
      <xsl:param name="node" select="."/>
      <xsl:choose>
        <xsl:when test="name($node)='note'">#069</xsl:when>
        <xsl:when test="name($node)='warning'">#900</xsl:when>
        <xsl:when test="name($node)='caution'">#c60</xsl:when>
        <xsl:when test="name($node)='tip'">#090</xsl:when>
        <xsl:when test="name($node)='important'">#069</xsl:when>
        <xsl:otherwise>#069</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>

  </xsl:attribute-set>

  <xsl:attribute-set name="admonition.properties">
    <xsl:attribute name="padding">.33em 0 0 5px</xsl:attribute>

    <xsl:attribute name="border-left">1px solid</xsl:attribute>
    <xsl:attribute name="border-color">
      <xsl:param name="node" select="."/>
      <xsl:choose>
        <xsl:when test="name($node)='note'">#069</xsl:when>
        <xsl:when test="name($node)='warning'">#900</xsl:when>
        <xsl:when test="name($node)='caution'">#c60</xsl:when>
        <xsl:when test="name($node)='tip'">#090</xsl:when>
        <xsl:when test="name($node)='important'">#069</xsl:when>
        <xsl:otherwise>#069</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>

  </xsl:attribute-set>

  <xsl:template match="guilabel">
    <xsl:call-template name="inline.italicseq"/>
  </xsl:template>

  <xsl:param name="glossary.sort" select="1"/>

  <!-- Only use title in xref references -->
  <xsl:param name="local.l10n.xml" select="document('')"/>
  <l:i18n xmlns:l="http://docbook.sourceforge.net/xmlns/l10n/1.0">
    <l:l10n language="hr">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="cs">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="da">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="nl">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="fr">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="en">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="de">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="sl">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="es">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="sv">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
    <l:l10n language="it">
      <l:context name="xref">
        <l:template name="section" text="%t"/>
        <l:template name="simplesect" text="%t"/>
      </l:context>
    </l:l10n>
  </l:i18n>

</xsl:stylesheet>
