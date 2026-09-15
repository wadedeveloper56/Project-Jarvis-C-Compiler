#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum class TokenType
{
    Keyword_int,
    Keyword_return,
    Identifier,
    IntegerLiteral,
    OpenParen,
    CloseParen,
    OpenBrace,
    CloseBrace,
    Semicolon,
    EOFToken
};

struct Token
{
    TokenType type;
    string value;
};

#include <string>
#include <vector>
#include <cctype>

class Lexer
{
    string src;
    size_t pos = 0;
public:
    Lexer(string source);
    vector<Token> tokenize();
private:
    Token lex_number();
    Token lex_identifier_or_keyword();
};



