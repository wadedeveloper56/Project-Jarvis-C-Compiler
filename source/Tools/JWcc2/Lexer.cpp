#include <cctype>
#include <string>
#include <vector>
#include "Lexer.h"

using namespace std;

Lexer::Lexer(string source) : src(source) {}

vector<Token> Lexer::tokenize() {
	vector<Token> tokens;
	while (pos < src.length())
	{
		char current = src[pos];

		if (isspace(current))
		{
			pos++;
			continue;
		}
		if (isalpha(current))
		{
			tokens.push_back(lex_identifier_or_keyword());
			continue;
		}
		if (isdigit(current))
		{
			tokens.push_back(lex_number());
			continue;
		}
		if (current == ';') { pos++; tokens.push_back({ TokenType::Semicolon, ";" }); continue; }
		if (current == '(') { pos++; tokens.push_back({ TokenType::OpenParen, "(" }); continue; }
		if (current == ')') { pos++; tokens.push_back({ TokenType::CloseParen, ")" }); continue; }
		if (current == '{') { pos++; tokens.push_back({ TokenType::OpenBrace, "{" }); continue; }
		if (current == '}') { pos++; tokens.push_back({ TokenType::CloseBrace, "}" }); continue; }

		pos++; // Handle unknown characters or errors
	}
	tokens.push_back({ TokenType::EOFToken, "" });
	return tokens;
}

Token Lexer::lex_number() {
	string res;
	while (pos < src.length() && isdigit(src[pos]))
	{
		res += src[pos++];
	}
	return { TokenType::IntegerLiteral, res };
}

Token Lexer::lex_identifier_or_keyword() {
	string res;
	while (pos < src.length() && isalnum(src[pos]))
	{
		res += src[pos++];
	}
	if (res == "int") return { TokenType::Keyword_int, res };
	if (res == "return") return { TokenType::Keyword_return, res };
	return { TokenType::Identifier, res };
}

