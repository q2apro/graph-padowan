<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:import href="./db_htmlchunk.xsl"/>
  <xsl:import href="./defaults.xsl"/>
  <xsl:import href="./html.xsl"/>

  <xsl:param name="generate.toc">
    appendix  toc,title
    article/appendix  nop
    article   toc,title
    book      toc,title,figure,table,example,equation
    chapter   toc,title
    part      toc,title
    preface   toc,title
    reference toc,title
    sect1     toc
    sect2     toc
    sect3     toc
    sect4     toc
    sect5     toc
    section   toc
    set       toc,title
  </xsl:param>

  <xsl:param name="suppress.navigation" select="0"/>
  <xsl:param name="toc.section.depth" select="1"/>
  <xsl:param name="generate.section.toc.level" select="0"/>

  <xsl:param name="img.src.path">images/</xsl:param>
  <xsl:param name="glossterm.auto.link" select="1"/>
  <xsl:param name="chunk.section.depth" select="1"/>
  <xsl:param name="example.autolabel" select="0"/>
  <xsl:param name="html.stylesheet">styles_html.css</xsl:param>
  <xsl:param name="use.id.as.filename" select="1"/>
  <xsl:param name="chunk.quietly" select="1"/>
  <xsl:param name="chunk.first.sections" select="1"/>
  <xsl:param name="ignore.image.scaling" select="1"/>
  <xsl:param name="generate.index" select="1" />
  <xsl:param name="navig.showtitles" select="1" />

</xsl:stylesheet>
