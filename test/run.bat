@ECHO ON
SET LIB32=C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\32bit\shell32.lib"
SET KLIB32=C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\32bit\kernel32.lib"
SET LLIB32="C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\32bit\ntstc_msvcrt.lib"

SET LIB64=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.28000.0\um\x64\shell32.lib
SET KLIB64=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.28000.0\um\x64\kernel32.lib
SET LLIB64=C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Tools\MSVC\14.53.36911\lib\x64\msvcrt.lib

SET CD=%CD%
MKDIR %CD%\bin
MKDIR %CD%\bin\windows
MKDIR %CD%\bin\windows\16bit
MKDIR %CD%\bin\windows\32bit
MKDIR %CD%\bin\windows\64bit
SET DIRECTORY=C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64

%DIRECTORY%\JWcc -b 64 -o %CD%\bin\windows\64bit\test1.asm test1.c > %CD%\bin\windows\64bit\test1.c.log
%DIRECTORY%\JWasm -win64 -Fl=%CD%\bin\windows\64bit\test1.lst -Sa -Fobin\windows\64bit\test1.obj %CD%\bin\windows\64bit\test1.asm
%DIRECTORY%\JWasm -win64 -Fl=%CD%\bin\windows\64bit\c064.lst -Sa -Fobin\windows\64bit\c064.obj c064.asm
%DIRECTORY%\JWlink @link64.lnk
%DIRECTORY%\pedump -a %CD%\bin\windows\64bit\test1.obj > %CD%\bin\windows\64bit\test1.asm.log
%DIRECTORY%\pedump -a %CD%\bin\windows\64bit\c064.obj > %CD%\bin\windows\64bit\c064.asm.log
%DIRECTORY%\pedump -a %CD%\bin\windows\64bit\test1.exe > %CD%\bin\windows\64bit\test1.exe.log
