import glob
import os
import gettext
import codecs
import sys

Languages = ["Croatian", "Czech", "Danish", "Dutch", "French", "German", "Italian", "Portuguese (Brazil)", "Slovenian", "Spanish", "Swedish"]
LocalePath = "../../Locale/"

def Encode(Str):
    return Str.replace('"', '\\"').replace('\n', ' ')

def CreateLanguage(Language):
    print
    print Language + ":"
    FileName = "GraphHelp_%s.po" % (Language[:Language.find(" ") if Language.find(" ") != -1 else None],)

    if os.path.exists(FileName):
        # Merge with old translation
        os.system("msgmerge -U -v -q --backup=off %s GraphHelp.pot --no-wrap" % (FileName,))
        InFile = FileName
    else:
        InFile = "GraphHelp.pot"


    os.system("msgfmt %s -o %s" % (LocalePath + Language + '.po', LocalePath + Language + '.mo'))
    Lang = gettext.GNUTranslations(open(LocalePath + Language + ".mo", "rb"))
    Lang.install()

    Lines = codecs.open(InFile, "r", "utf-8").readlines()
    OutFile = codecs.open(FileName, "w", "utf-8")
    IgnoreNext = False
    for Line in Lines:
        if not IgnoreNext:
            OutFile.write(Line)
        IgnoreNext = False
        if Line[:5] == "msgid":
            Str = Line[7 : Line.find('"', 7)]
            if Str:
                Str2 = Lang.ugettext(Str)
                if Str2 == Str:
                    Str2 = Lang.ugettext(Str + ":")[:-1]
                if Str2 != Str:
                    OutFile.write('msgstr "%s"\n' % (Encode(Str2),))
                    IgnoreNext = True

os.environ['PATH'] = os.environ['PATH'] + os.pathsep + '../../Tools'
os.chdir("../Source")
Files = glob.glob("*.xml")
os.system(sys.executable + " ../Scripts/xml2po.py -k -o ../po/GraphHelp.pot " + " ".join(Files))
os.chdir("../po")

if len(sys.argv) == 1:
    for Language in Languages:
        CreateLanguage(Language)
else:
    for Language in sys.argv[1:]:
        CreateLanguage(Language)
