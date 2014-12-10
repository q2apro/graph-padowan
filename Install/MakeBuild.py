# -*- coding: latin-1 -*-

from ftplib import FTP
from getpass import getpass
from VersionInfo import CalcVersionInfo
import os
import sys
import traceback
import datetime
import subprocess

try:
  # Test that Graph can be started without some obscure dll files
  os.system("Graph.exe /regserver")

  # Sign Graph.exe
  if "/sign" in sys.argv:
    subprocess.check_call(['signtool.exe',  'sign', '/f', 'IvanMøllerJohansen.crt', '/t', 'http://timestamp.comodoca.com/authenticode', '/d', '"Graph"', 'Graph.exe'])

  # Compile SetupGraphBeta-4.2.0.x.exe
  print("Compiling...")
  subprocess.check_call(["c:\\program files\\Inno Setup 5\\iscc.exe", "/Q", "Graph.iss"])

  VersionInfo = CalcVersionInfo("Graph.exe")
  FileName = "SetupGraphBeta-" + VersionInfo + ".exe"

  # Sign SetupGraphBeta-x.x.x.exe
  if "/sign" in sys.argv:
    subprocess.check_call(['signtool.exe',  'sign', '/f', 'IvanMøllerJohansen.crt', '/t', 'http://timestamp.comodoca.com/authenticode', '/d', '"Graph"', FileName])

  #Creating GraphBeta.inf
  print("Writing GraphBeta.inf ...")
  File = open("GraphBeta.inf", "w")
  File.write("[Graph]\n")
  File.write("Major = " + VersionInfo[0] + "\n")
  File.write("Minor = " + VersionInfo[2] + "\n")
  File.write("Release = " + VersionInfo[4] + "\n")
  File.write("Build = " + VersionInfo[6:] + "\n")
  File.write("Date = " + datetime.date.today().strftime("%d-%m-%Y\n"))
  File.write("DownloadFile = http://www.padowan.dk/bin/" + FileName + '\n')
  File.write("DownloadPage = http://www.padowan.dk/beta\n")

  # Upload SetupGraphBeta.exe to the server
  Password = getpass()
  ftp = FTP('ftp.padowan.dk')   # connect to host, default port
  ftp.login('padowan.dk', Password)
  ftp.cwd('bin')
  print("Uploading", FileName, "...")
  File = open(FileName, 'rb')
  ftp.storbinary('STOR ' + FileName, File)
  if ftp.size(FileName) != os.stat(FileName).st_size:
    raise Exception("Wrong file size on server")

  print("Uploading GraphBeta.inf ...")
  ftp.cwd('../graph')
  File = open("GraphBeta.inf", 'rb')
  ftp.storbinary('STOR GraphBeta.inf', File)

  ftp.quit()
  print("Upload complete!")
except Exception:
  traceback.print_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2])
