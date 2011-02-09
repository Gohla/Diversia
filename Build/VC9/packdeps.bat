@echo off
set /p version=Please enter the dependency package version: 
7z.exe a -tzip -mx=9 -r -x!*.pdb -x!.svn dependencies-vc9-%version%.zip dependencies\
7z.exe a -tzip -mx=9 -r -x!*.dll -x!*.lib dependencies-vc9-pdb-%version%.zip dependencies\lib\