@ECHO ON
SET LIB32=C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\32bit\shell32.lib"
SET KLIB32=C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\32bit\kernel32.lib"
SET LLIB52="C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\32bit\ntstc_msvcrt.lib"

SET LIB64="C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\64bit\shell32.lib"
SET KLIB64="C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\64bit\kernel32.lib"
SET LLIB64="C:\PROGRA~1\MICROS~2\18\Insiders\VC\Tools\MSVC\14.51.36231\lib\64bit\msvcrt.lib"

SET CD=%CD%
MKDIR %CD%\bin
MKDIR %CD%\bin\windows
MKDIR %CD%\bin\windows\16bit
MKDIR %CD%\bin\windows\32bit
MKDIR %CD%\bin\windows\64bit
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc -b 64 -o %CD%\bin\windows\64bit\test1.asm test1.c > %CD%\bin\windows\64bit\test1.c.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -win64 -Fl=%CD%\bin\windows\64bit\test1.lst -Sa -Fobin\windows\64bit\test1.obj %CD%\bin\windows\64bit\test1.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -win64 -Fl=%CD%\bin\windows\64bit\c064.lst -Sa -Fobin\windows\64bit\c064.obj c064.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWlink format win pe option map=%CD%\bin\windows\64bit\test1.map file %CD%\bin\windows\64bit\test1.obj, %CD%\bin\windows\64bit\c064.obj lib %LIB64%,%KLIB64%,%LLIB64%  name %CD%\bin\windows\64bit\test1.exe
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\64bit\test1.obj > %CD%\bin\windows\64bit\test1.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\64bit\c064.obj > %CD%\bin\windows\64bit\c064.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\64bit\test1.exe > %CD%\bin\windows\64bit\test1.exe.log

C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc -b 32 -o %CD%\bin\windows\32bit\test1.asm test1.c > %CD%\bin\windows\32bit\test1.c.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -coff -Fl=%CD%\bin\windows\32bit\test1.lst -Sa -Fobin\windows\32bit\test1.obj %CD%\bin\windows\32bit\test1.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -coff -Fl=%CD%\bin\windows\32bit\c032.lst -Sa -Fobin\windows\32bit\c032.obj c032.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWlink format win pe option map=%CD%\bin\windows\32bit\test1.map file %CD%\bin\windows\32bit\test1.obj, %CD%\bin\windows\32bit\c032.obj lib %LIB32%,%KLIB32%  name %CD%\bin\windows\32bit\test1.exe
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\32bit\test1.obj > %CD%\bin\windows\32bit\test1.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\32bit\c032.obj > %CD%\bin\windows\32bit\c032.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\32bit\test1.exe > %CD%\bin\windows\32bit\test1.exe.log

C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc -b 16 -o %CD%\bin\windows\16bit\test1.asm test1.c > %CD%\bin\windows\16bit\test1.c.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -omf -Fl=%CD%\bin\windows\16bit\test1.lst -Sa -Fobin\windows\16bit\test1.obj %CD%\bin\windows\16bit\test1.asm
REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -coff -Fl=%CD%\bin\windows\32bit\c032.lst -Sa -Fobin\windows\32bit\c032.obj c032.asm
REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWlink format win pe option map=%CD%\bin\windows\32bit\test1.map file %CD%\bin\windows\32bit\test1.obj, %CD%\bin\windows\32bit\c032.obj lib %LIB32%,%KLIB32%  name %CD%\bin\windows\32bit\test1.exe
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\16bit\test1.obj > %CD%\bin\windows\16bit\test1.asm.log
REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\32bit\c032.obj > %CD%\bin\windows\32bit\c032.asm.log
REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\32bit\test1.exe > %CD%\bin\windows\32bit\test1.exe.log

REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWlib ?
REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWmake --help --version
REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcpp --help --version
REM C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWrc --help

C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc2 -b 64 -c x64 -o test2_64.asm test2.c
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -win64 -Fl=test2_64.lst -Sa -Fotest2_64.obj test2_64.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a test2_64.obj > test2_64.asm.log

C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc2 -b 32 -c 386 -o test2_32.asm test2.c
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -coff -Fl=test2_32.lst -Sa -Fotest2_32.obj test2_32.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a test2_32.obj > test2_32.asm.log

C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc2 -b 16 -c 386 -o test2_16.asm test2.c
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -omf -Fl=test2_16.lst -Sa -Fotest2_16.obj test2_16.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a test2_16.obj > test2_16.asm.log

