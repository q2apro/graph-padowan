<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:import href="./db_htmlhelp.xsl"/>
  <xsl:import href="./defaults.xsl"/>
  <xsl:import href="./html.xsl"/>

  <xsl:param name="suppress.navigation" select="0"/>
  <xsl:param name="toc.section.depth" select="4"/>

  <xsl:param name="htmlhelp.title">Graph</xsl:param>
  <xsl:param name="htmlhelp.force.map.and.alias" select="1"/>
  <xsl:param name="htmlhelp.show.menu" select="1"/>
  <xsl:param name="htmlhelp.show.advanced.search" select="1"/>
  <xsl:param name="htmlhelp.hhc.folders.instead.books" select="1"/>
  <xsl:param name="htmlhelp.hhc.binary" select="0"/>
  <xsl:param name="htmlhelp.use.hhk" select="0"/>
  <xsl:param name="htmlhelp.display.progress" select="0"/>
  <xsl:param name="htmlhelp.chm">..\Graph-English.chm</xsl:param>
  <xsl:param name="htmlhelp.hhc.show.root" select="0"/>

  <xsl:param name="img.src.path">Images/</xsl:param>
  <xsl:param name="glossterm.auto.link" select="1"/>
  <xsl:param name="generate.toc"/>
  <xsl:param name="html.stylesheet">styles_chm.css</xsl:param>
  <xsl:param name="use.id.as.filename" select="1"/>
  <xsl:param name="chunk.section.depth" select="2"/>
  <xsl:param name="chunk.quietly" select="1"/>
  <xsl:param name="chunk.first.sections" select="1"/>
  <xsl:param name="ignore.image.scaling" select="1"/>
  <xsl:param name="generate.index" select="1" />
  <xsl:param name="preferred.mediaobject.role">htmlhelp</xsl:param>

</xsl:stylesheet>
