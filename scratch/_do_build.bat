@echo off
cd /d "C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2"
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat" x64 >NUL 2>&1
msbuild mintaly-cs2.vcxproj /p:Configuration=Release /p:Platform=x64 /v:q > "C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\scratch\build_out.txt" 2>&1
echo EXITCODE=%ERRORLEVEL%