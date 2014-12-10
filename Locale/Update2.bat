@echo off
FOR %%a IN (*.po) DO IF NOT "%%a"=="Ignore.po" msgmerge -U -v --backup=off "%%a" Graph.pot --no-wrap
pause
