@ECHO ON
..\..\..\external\flex-bison\win_bison --verbose --output="c11ansi.c" --defines="c11ansi.h"  "c11ansi.y"
..\..\..\external\flex-bison\win_flex --verbose --yylineno --outfile="c11ansi.lex.c" --wincompat  "c11ansi.l"