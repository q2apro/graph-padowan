@echo off
FOR %%a IN (*.po) DO IF NOT %%a==Template.po IF NOT %%a==Ignore.po msgfmt "%%a" -c -o"%%~na.mo"
pause
