#pragma once

#include <cctype>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <map>
#include <algorithm>
#include <unordered_set>

using namespace std;

enum class TokenKind
{
    EndToken,
    IdentifierToken,
    NumberToken,
    IntToken,
    VoidToken,
    ReturnToken,
    PlusToken,
    MinusToken,
    StarToken, 
    SlashToken,
    LParenToken, 
    RParenToken, 
    LBraceToken, 
    RBraceToken,
    SemicolonToken, 
    CommaToken, 
    AssignToken,
    UnsignedToken,
    UnknownToken
};

struct Token
{
    TokenKind kind;
    string text;
    int number = 0;
    int pos = 0; // start index in source
    int endPos = 0; // inclusive end index
    int line = 1; // start line
    int column = 1; // start column
    int endLine = 1; // end line
    int endColumn = 1; // end column
};

class Lexer
{
    string src;
    size_t i = 0;
    int pos = 0;
    unordered_set<string> keywords{ "int", "void", "return" };
    int line = 1;
    int column = 1;
public:
    Lexer(string s);
    Token next();
};

