#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum class TokenType
{
    Keyword_int, Keyword_void, Keyword_return,
    Identifier, Number,
    OpenParen, CloseParen, OpenBrace, CloseBrace,
    Semicolon, Equal, Comma,
    Eof, Unknown
};

struct Token
{
    TokenType type;
    string value;
};

class Lexer
{
public:
    explicit Lexer(string source);
    vector<Token> tokenize();
private:
    string source_;
    size_t index_;
    Token lexIdentifier();
    Token lexNumber();
};
