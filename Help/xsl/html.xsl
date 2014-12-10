<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="markup[@role = 'fo']" />

  <!-- Show "Translator:" followed by the list of translators, one on each line -->
  <xsl:template match="othercredit" mode="titlepage.mode">
    <xsl:variable name="contrib" select="string(contrib)"/>
    <xsl:if test="not(preceding-sibling::othercredit[string(contrib)=$contrib])">
      <xsl:apply-templates mode="titlepage.mode" select="contrib"/>
      <br/>
    </xsl:if>
    <xsl:call-template name="person.name"/>
  </xsl:template>

  <!-- Copied from fo\lists.xsl in DocBook and changed to not show comma before "and" -->
  <xsl:template match="simplelist[@type='inline']">
    <span>
      <xsl:apply-templates select="." mode="class.attribute"/>
    <!-- if dbchoice PI exists, use that to determine the choice separator -->
    <!-- (that is, equivalent of "and" or "or" in current locale), or literal -->
    <!-- value of "choice" otherwise -->
    <xsl:variable name="localized-choice-separator">
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
      <xsl:call-template name="simple.xlink">
        <xsl:with-param name="content">
          <xsl:apply-templates/>
        </xsl:with-param>
      </xsl:call-template>
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
    </xsl:for-each>
    </span>
  </xsl:template>

</xsl:stylesheet>
