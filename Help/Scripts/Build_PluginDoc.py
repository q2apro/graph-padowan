import os
import shutil
import sys

ToolsDir = "..\\..\\Tools\\"

print "Copying image files..."
os.system("XCopy ..\\Images\\Common\\GraphIcon.png ..\\Temp\\Images\\ /I /Q /Y > NUL")
os.system("XCopy ..\\Images\\Common\\Graph_Icon.svg ..\\Temp\\Images\\ /I /Q /Y > NUL")
os.system("XCopy ..\\Images\\Common\\C4B94475.png ..\\Temp\\Images\\ /I /Q /Y > NUL")
os.system("XCopy ..\\Source\\dtd ..\\Temp\\dtd /S /I /Q /Y > NUL")
os.system("XCopy ..\\Source\\styles_chm.css ..\\Temp /Q /Y > NUL")

os.chdir("..\\PluginDoc")
os.environ["path"] += ";" + ToolsDir
os.system("xsltproc.exe --output ..\\xsl\\titlepage.xsl " + ToolsDir + "xsl\\template\\titlepage.xsl ..\\xsl\\titlepage.xml")

os.system("xmllint --nonet --xinclude --output ..\\Temp\PluginDoc.tmp PluginDoc.xml")

InFile = "..\\Temp\\PluginDoc.tmp"

print "Creating PDF file..."

StyleSheet = "..\\xsl\\pdfdoc.xsl"
os.system("xsltproc.exe --nonet --xinclude --output ..\\Temp\\PluginDoc.fo %s %s" % (StyleSheet, InFile))

UserConfig = "..\\xsl\\userconfig.xml"
os.system(ToolsDir + "fop\\fop.bat -q -c %s -fo ..\\Temp\\PluginDoc.fo -pdf ..\\PluginDoc.pdf" % (UserConfig,))

print "Creating CHM file..."
StyleSheet = "..\\xsl\\htmlhelp.xsl"
os.system('xsltproc.exe --nonet --xinclude --stringparam htmlhelp.chm ..\\PluginDoc.chm --stringparam htmlhelp.title "Script Engine Documentation" --stringparam htmlhelp.use.hhk 1 --output ..\\Temp\\PluginDoc %s %s'  % (StyleSheet, InFile))
os.system("hhc.exe ..\\Temp\\htmlhelp.hhp > NUL")

print "Creating HTML chunks..."
StyleSheet = "..\\xsl\\htmlchunk.xsl"
os.system("XCopy ..\\Source\\*.css ..\\html\\PluginDoc\\ /Q /Y /I > NUL")
os.system("xsltproc.exe --nonet --xinclude --noout --output ..\\html\\PluginDoc\\PluginDoc %s %s > NUL"  % (StyleSheet, InFile))
os.system("XCopy ..\\Images\\Common\\GraphIcon.png ..\\html\\PluginDoc\\images\\ /I /Q /Y > NUL")

