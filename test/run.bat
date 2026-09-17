@ECHO ON
SET LIB32=C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\x86\shell32.lib"
SET KLIB32=C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\x86\kernel32.lib"
SET LLIB52="C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\x86\ntstc_msvcrt.lib"

SET LIB64="C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\x64\shell32.lib"
SET KLIB64="C:\PROGRA~2\WI3CF2~1\10\Lib\10.0.26100.0\um\x64\kernel32.lib"
SET LLIB64="C:\PROGRA~1\MICROS~2\18\Insiders\VC\Tools\MSVC\14.51.36231\lib\x64\msvcrt.lib"

SET CD=%CD%
MKDIR %CD%\bin
MKDIR %CD%\bin\windows
MKDIR %CD%\bin\windows\x86
MKDIR %CD%\bin\windows\x64
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc -b 64 -o %CD%\bin\windows\x64\test1.asm test1.c > %CD%\bin\windows\x64\test1.c.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -win64 -Fl=%CD%\bin\windows\x64\test1.lst -Sa -Fobin\windows\x64\test1.obj %CD%\bin\windows\x64\test1.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -win64 -Fl=%CD%\bin\windows\x64\c064.lst -Sa -Fobin\windows\x64\c064.obj c064.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWlink format win pe option map=%CD%\bin\windows\x64\test1.map file %CD%\bin\windows\x64\test1.obj, %CD%\bin\windows\x64\c064.obj lib %LIB64%,%KLIB64%,%LLIB64%  name %CD%\bin\windows\x64\test1.exe
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\x64\test1.obj > %CD%\bin\windows\x64\test1.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\x64\c064.obj > %CD%\bin\windows\x64\c064.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\x64\test1.exe > %CD%\bin\windows\x64\test1.exe.log

C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcc -b 32 -o %CD%\bin\windows\x86\test1.asm test1.c > %CD%\bin\windows\x86\test1.c.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -coff -Fl=%CD%\bin\windows\x86\test1.lst -Sa -Fobin\windows\x86\test1.obj %CD%\bin\windows\x86\test1.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWasm -coff -Fl=%CD%\bin\windows\x86\c032.lst -Sa -Fobin\windows\x86\c032.obj c032.asm
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWlink format win pe option map=%CD%\bin\windows\x86\test1.map file %CD%\bin\windows\x86\test1.obj, %CD%\bin\windows\x86\c032.obj lib %LIB32%,%KLIB32%  name %CD%\bin\windows\x86\test1.exe
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\x86\test1.obj > %CD%\bin\windows\x86\test1.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\x86\c032.obj > %CD%\bin\windows\x86\c032.asm.log
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\pedump -a %CD%\bin\windows\x86\test1.exe > %CD%\bin\windows\x86\test1.exe.log

C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWlib ?
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWmake --help --version
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWcpp --help --version
C:\projects\OSDev\Project-Jarvis-C-Compiler\bin\tools\Release\x64\JWrc --help
