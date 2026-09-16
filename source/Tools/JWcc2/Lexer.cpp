#include <cctype>
#include <string>
#include <vector>
#include "Lexer.h"

using namespace std;

Lexer::Lexer(string source) : source_(move(source)), index_(0) {}

vector<Token> Lexer::tokenize()
{
	vector<Token> tokens;
	while (index_ < source_.size())
	{
		char current = source_[index_];

		if (isspace(current))
		{
			index_++;
			continue;
		}

		if (isalpha(current) || current == '_')
		{
			tokens.push_back(lexIdentifier());
			continue;
		}

		if (isdigit(current))
		{
			tokens.push_back(lexNumber());
			continue;
		}

		switch (current)
		{
			case '(': index_++; tokens.push_back({ TokenType::OpenParen, "(" }); break;
			case ')': index_++; tokens.push_back({ TokenType::CloseParen, ")" }); break;
			case '{': index_++; tokens.push_back({ TokenType::OpenBrace, "{" }); break;
			case '}': index_++; tokens.push_back({ TokenType::CloseBrace, "}" }); break;
			case ';': index_++; tokens.push_back({ TokenType::Semicolon, ";" }); break;
			case '=': index_++; tokens.push_back({ TokenType::Equal, "=" }); break;
			case ',': index_++; tokens.push_back({ TokenType::Comma, "," }); break;
			default:
				index_++;
				tokens.push_back({ TokenType::Unknown, string(1, current) });
				break;
		}
	}
	tokens.push_back({ TokenType::Eof, "" });
	return tokens;
}

Token Lexer::lexIdentifier() 
{
	size_t start = index_;
	while (index_ < source_.size() && (isalnum(source_[index_]) || source_[index_] == '_'))
	{
		index_++;
	}
	string text = source_.substr(start, index_ - start);
	if (text == "int") return { TokenType::Keyword_int, text };
	if (text == "void") return { TokenType::Keyword_void, text };
	if (text == "return") return { TokenType::Keyword_return, text };
	return { TokenType::Identifier, text };
}

Token Lexer::lexNumber() 
{
	size_t start = index_;
	while (index_ < source_.size() && isdigit(source_[index_]))
	{
		index_++;
	}
	return { TokenType::Number, source_.substr(start, index_ - start) };
}

