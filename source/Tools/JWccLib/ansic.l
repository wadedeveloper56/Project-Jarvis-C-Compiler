%e  1019
%p  2807
%n  371
%k  284
%a  1213
%o  1117

O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\n\f]

%{
    #define __STDC_LIMIT_MACROS 1
    #include "pch.h"
    #include "GlobalVars.h"
    #include "TokenType.h"
    #define YY_NO_UNISTD_H
    #include "scanner.h"
    #include "interpreter.h"
    #include "parser.hpp"
    #include "location.hh"
    #include "Token.h"
    #include "Compiler.h"

    using namespace std;
    using namespace WadeSpace;

    // Original yyterminate() macro returns int. Since we're using Bison 3 variants
    // as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
    #define yyterminate() WadeSpace::Parser::make_END(WadeSpace::location());

    // This will track current scanner location.
    // Action is called when length of the token is known.
    #define YY_USER_ACTION m_driver.increaseLocation(yyleng);
    
    // !!!WARNING!!!
    // Location API is used, but the location is not initialized, 'cause I'm lazy. When making
    // a token with make_{something} method you can pass detailed token location. Current location
    // is accessible with m_driver.location() method. All puzzle elements are there - just
    // pass location value in every action code block below. I'm going to waste more time writing
    // this excuse than putting this boilerplate below...
    //
    // Location class can be found in location.hh and posistion.hh files. It's just a bit too much
    // boilerplate for this small example. Bummer.
%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="WadeSpace_"

%%

"auto"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_AUTO(createKeywordToken(yytext,AUTO), WadeSpace::location()); 
               }               
"break"        { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_BREAK(createKeywordToken(yytext,BREAK), WadeSpace::location()); 
               }
"case"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_CASE(createKeywordToken(yytext,CASE), WadeSpace::location()); 
               }
"char"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_CHAR(createKeywordToken(yytext,CHAR), WadeSpace::location()); 
               }
"const"        { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_CONSTT(createKeywordToken(yytext,CONSTT), WadeSpace::location()); 
               }
"continue"     { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_CONTINUE(createKeywordToken(yytext,CONTINUE), WadeSpace::location()); 
               }
"default"      { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_DEFAULT(createKeywordToken(yytext,DEFAULT), WadeSpace::location()); 
               }
"do"           { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_DO(createKeywordToken(yytext,DO), WadeSpace::location()); 
               }
"double"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_DOUBLE(createKeywordToken(yytext,DOUBLE), WadeSpace::location()); 
               }
"long double"  { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_LONG_DOUBLE(createKeywordToken(yytext,LONG_DOUBLE), WadeSpace::location()); 
               }
"else"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_ELSE(createKeywordToken(yytext,ELSE), WadeSpace::location()); 
               }
"enum"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_ENUM(createKeywordToken(yytext,ENUM), WadeSpace::location()); 
               }
"extern"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_EXTERN(createKeywordToken(yytext,EXTERN), WadeSpace::location()); 
               }
"float"        { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_FLOAT(createKeywordToken(yytext,FLOAT), WadeSpace::location()); 
               }
"for"          { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_FOR(createKeywordToken(yytext,FOR), WadeSpace::location()); 
               }
"goto"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_GOTO(createKeywordToken(yytext,GOTO), WadeSpace::location()); 
               }
"if"           { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_IF(createKeywordToken(yytext,IF), WadeSpace::location()); 
               }
"inline"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_INLINE(createKeywordToken(yytext,INLINE), WadeSpace::location()); 
               }
"int"          { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_INT(createKeywordToken(yytext,INT), WadeSpace::location()); 
               }
"long"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_LONG(createKeywordToken(yytext,LONG), WadeSpace::location()); 
               }
"long long"    { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_LONG_LONG(createKeywordToken(yytext,LONG_LONG), WadeSpace::location()); 
               }
"register"     { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_REGISTER(createKeywordToken(yytext,REGISTER), WadeSpace::location()); 
               }
"restrict"     { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_RESTRICT(createKeywordToken(yytext,RESTRICT), WadeSpace::location()); 
               }
"return"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_RETURN(createKeywordToken(yytext,RETURN), WadeSpace::location()); 
               }
"short"        { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_SHORTT(createKeywordToken(yytext,SHORTT), WadeSpace::location()); 
               }
"signed"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_SIGNED(createKeywordToken(yytext,SIGNED), WadeSpace::location()); 
               }
"sizeof"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_SIZEOF(createKeywordToken(yytext,SIZEOF), WadeSpace::location()); 
               }
"static"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_STATIC(createKeywordToken(yytext,STATIC), WadeSpace::location()); 
               }
"struct"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_STRUCT(createKeywordToken(yytext,STRUCT), WadeSpace::location()); 
               }
"switch"       { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_SWITCH(createKeywordToken(yytext,SWITCH), WadeSpace::location()); 
               }
"typedef"      { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_TYPEDEF(createKeywordToken(yytext,TYPEDEF), WadeSpace::location()); 
               }
"union"        { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_UNION(createKeywordToken(yytext,UNION), WadeSpace::location()); 
               }
"unsigned"     { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_UNSIGNED(createKeywordToken(yytext,UNSIGNED), WadeSpace::location()); 
               }
"void"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_VOIDT(createKeywordToken(yytext,VOIDT), WadeSpace::location()); 
               }
"volatile"     { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_VOLATILE(createKeywordToken(yytext,VOLATILE), WadeSpace::location()); 
               }
"while"        { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_WHILE(createKeywordToken(yytext,WHILE), WadeSpace::location()); 
               }
"bool"         { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_BOOLT(createKeywordToken(yytext,BOOLT), WadeSpace::location()); 
               }
{L}{A}*        { 
                if (compiler != nullptr && compiler->hasTypedef(yytext))
                {
                    //cout << "Scanner: typedef name [" << yytext << "]" << endl;
                    return WadeSpace::Parser::make_TYPE_NAME(createStringIDToken(yytext), WadeSpace::location()); 
                }
                else
                {
                    //cout << "Scanner: identifier [" << yytext << "]" << endl;
                    return WadeSpace::Parser::make_IDENTIFIER(createStringIDToken(yytext), WadeSpace::location()); 
                }
               }
{HP}{H}+{IS}?  {
                //cout << "Scanner: decimal number: " << yytext << endl;
                uint64_t number = strtoull(yytext, 0, 10);
                return WadeSpace::Parser::make_I_CONST(createConstantULLToken(number), WadeSpace::location());
               }
{NZ}{D}*{IS}?  {
                //cout << "Scanner: decimal number: " << yytext << endl;
                uint64_t number = strtoull(yytext, 0, 10);
                return WadeSpace::Parser::make_I_CONST(createConstantULLToken(number), WadeSpace::location());
               }
"0"{O}*{IS}?   {
                //cout << "Scanner: decimal number: " << yytext << endl;
                uint64_t number = strtoull(yytext, 0, 10);
                return WadeSpace::Parser::make_I_CONST(createConstantULLToken(number), WadeSpace::location());
               }
{CP}?"'"([^'\\\n]|{ES})+"'"  {
                //cout << "Scanner: decimal number: " << yytext << endl;
                uint64_t number = strtoull(yytext, 0, 10);
                return WadeSpace::Parser::make_I_CONST(createConstantULLToken(number), WadeSpace::location());
               }
{D}+{E}{FS}?  {              
                //cout << "Scanner: decimal number: " << yytext << endl;
                long double number = strtold(yytext, NULL);
                return WadeSpace::Parser::make_F_CONST(createConstantLDToken(number), WadeSpace::location());
               }
{D}*"."{D}+{E}?{FS}? {              
                //cout << "Scanner: decimal number: " << yytext << endl;
                long double number = strtold(yytext, NULL);
                return WadeSpace::Parser::make_F_CONST(createConstantLDToken(number), WadeSpace::location());
               }
{D}+"."{E}?{FS}? {              
                //cout << "Scanner: decimal number: " << yytext << endl;
                long double number = strtold(yytext, NULL);
                return WadeSpace::Parser::make_F_CONST(createConstantLDToken(number), WadeSpace::location());
               }
{HP}{H}+{P}{FS}? {              
                //cout << "Scanner: decimal number: " << yytext << endl;
                long double number = strtold(yytext, NULL);
                return WadeSpace::Parser::make_F_CONST(createConstantLDToken(number), WadeSpace::location());
               }
{HP}{H}*"."{H}+{P}{FS}? {              
                //cout << "Scanner: decimal number: " << yytext << endl;
                long double number = strtold(yytext, NULL);
                return WadeSpace::Parser::make_F_CONST(createConstantLDToken(number), WadeSpace::location());
               }
{HP}{H}+"."{P}{FS}? {              
                //cout << "Scanner: decimal number: " << yytext << endl;
                long double number = strtold(yytext, NULL);
                return WadeSpace::Parser::make_F_CONST(createConstantLDToken(number), WadeSpace::location());
               }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+   { 
                //  //cout << "Scanner: string literal [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_STRING_LITERAL(createStringConstantToken(yytext), WadeSpace::location()); 
               }               

"..."          {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_ELLIPSIS(createKeywordToken(yytext,ELLIPSIS),WadeSpace::location());
               }
"?"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_QUESTION(createKeywordToken(yytext,QUESTION),WadeSpace::location());
               }
";"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_SEMICOLON(createKeywordToken(yytext,SEMICOLON),WadeSpace::location());
               }
"{"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_OCURLY(createKeywordToken(yytext,OCURLY),WadeSpace::location());
               }
"}"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_CCURLY(createKeywordToken(yytext,CCURLY),WadeSpace::location());
               }
","            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_COMMA(createKeywordToken(yytext,COMMA),WadeSpace::location());
               }
":"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_COLON(createKeywordToken(yytext,COLON),WadeSpace::location());
               }
"("            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_OPAREN(createKeywordToken(yytext,OPAREN),WadeSpace::location());
               }
")"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_CPAREN(createKeywordToken(yytext,CPAREN),WadeSpace::location());
               }
"["            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_OBRACE(createKeywordToken(yytext,OBRACE),WadeSpace::location());
               }
"]"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_CBRACE(createKeywordToken(yytext,CBRACE),WadeSpace::location());
               }
"."            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_PERIOD(createKeywordToken(yytext,PERIOD),WadeSpace::location());
               }
"~"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_TILDE(createKeywordToken(yytext,TILDE),WadeSpace::location());
               }
"="            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_EQUAL(createKeywordToken(yytext,EQUAL),WadeSpace::location());
               }
">>="          {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_RIGHT_ASSIGN(createKeywordToken(yytext,RIGHT_ASSIGN),WadeSpace::location());
               }
"<<="          {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_LEFT_ASSIGN(createKeywordToken(yytext,LEFT_ASSIGN),WadeSpace::location());
               }
"+="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_ADD_ASSIGN(createKeywordToken(yytext,ADD_ASSIGN),WadeSpace::location());
               }
"-="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_SUB_ASSIGN(createKeywordToken(yytext,SUB_ASSIGN),WadeSpace::location());
               }
"*="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_MUL_ASSIGN(createKeywordToken(yytext,MUL_ASSIGN),WadeSpace::location());
               }
"/="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_DIV_ASSIGN(createKeywordToken(yytext,DIV_ASSIGN),WadeSpace::location());
               }
"%="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_MOD_ASSIGN(createKeywordToken(yytext,MOD_ASSIGN),WadeSpace::location());
               }
"&="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_AND_ASSIGN(createKeywordToken(yytext,AND_ASSIGN),WadeSpace::location());
               }
"^="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_XOR_ASSIGN(createKeywordToken(yytext,XOR_ASSIGN),WadeSpace::location());
               }
"|="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_OR_ASSIGN(createKeywordToken(yytext,OR_ASSIGN),WadeSpace::location());
               }
">>"           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_RIGHT_OP(createKeywordToken(yytext,RIGHT_OP),WadeSpace::location());
               }
"<<"           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_LEFT_OP(createKeywordToken(yytext,LEFT_OP),WadeSpace::location());
               }
"++"           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_INC_OP(createKeywordToken(yytext,INC_OP),WadeSpace::location());
               }
"--"           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_DEC_OP(createKeywordToken(yytext,DEC_OP),WadeSpace::location());
               }
"->"           { 
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_PTR_OP(createKeywordToken(yytext,PTR_OP),WadeSpace::location());
               }
"&&"           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_AND_OP(createKeywordToken(yytext,AND_OP),WadeSpace::location());
               }
"||"           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_OR_OP(createKeywordToken(yytext,OR_OP),WadeSpace::location());
               }
"<"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_LESS(createKeywordToken(yytext,LESS),WadeSpace::location());
               }
">"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_GREATER(createKeywordToken(yytext,GREATER),WadeSpace::location());
               }
"<="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_LESS_EQUAL(createKeywordToken(yytext,LESS_EQUAL),WadeSpace::location());
               }
">="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_GREATER_EQUAL(createKeywordToken(yytext,GREATER_EQUAL),WadeSpace::location());
               }
"=="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_EQUAL_EQUAL(createKeywordToken(yytext,EQUAL_EQUAL),WadeSpace::location());
               }
"!="           {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_NOT_EQUAL(createKeywordToken(yytext,NOT_EQUAL),WadeSpace::location());
               }
"!"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_NOT_OP(createKeywordToken(yytext,NOT_OP),WadeSpace::location());
               }
"^"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_XOR_OP(createKeywordToken(yytext,XOR_OP),WadeSpace::location());
               }
"&"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_BIT_AND(createKeywordToken(yytext,BIT_AND),WadeSpace::location());
               }
"|"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_BIT_OR(createKeywordToken(yytext,BIT_OR),WadeSpace::location());
               }
"-"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_MINUS_OP(createKeywordToken(yytext,MINUS_OP),WadeSpace::location());
               }
"+"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_PLUS_OP(createKeywordToken(yytext,PLUS_OP),WadeSpace::location());
               }
"*"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_TIMES_OP(createKeywordToken(yytext,TIMES_OP),WadeSpace::location());
               }
"/"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_DIV_OP(createKeywordToken(yytext,DIV_OP),WadeSpace::location());
               }
"%"            {
                //cout << "Scanner: token [" << yytext << "]" << endl;
                return WadeSpace::Parser::make_MOD_OP(createKeywordToken(yytext,MOD_OP),WadeSpace::location());
               }
\"[^"]*\"      { cout << "Scanner: multiline comment [" << yytext << "]" << endl;  
               }
"//".*         { cout << "Scanner: single line comment [" << yytext << "]" << endl;  
               }
\n             { m_driver.clear(); }
[ \t\v\f]    { }
.              { cout << "Scanner: unknown character [" << yytext << "]" << endl; }
<<EOF>>        { return yyterminate(); }

%%
