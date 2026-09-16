#include "pch.h"
#include "Lexer.h"

using namespace std;

Lexer::Lexer(string s) : src(move(s)) {}

Token Lexer::next() 
{
	while (i < src.size() && isspace((unsigned char)src[i])) { ++i; ++pos; }
	if (i >= src.size()) return { TokenKind::End, "", 0, pos };

	char c = src[i];
	Token t; t.pos = pos;

	// identifiers / keywords
	if (isalpha((unsigned char)c) || c == '_')
	{
		size_t start = i;
		while (i < src.size() && (isalnum((unsigned char)src[i]) || src[i] == '_')) ++i;
		t.text = src.substr(start, i - start);
		if (t.text == "int") t.kind = TokenKind::Int;
		else if (t.text == "void") t.kind = TokenKind::Void;
		else if (t.text == "return") t.kind = TokenKind::Return;
		else { t.kind = TokenKind::Identifier; }
		pos += (int)t.text.size();
		return t;
	}

	// numbers (integers only)
	if (isdigit((unsigned char)c))
	{
		size_t start = i;
		while (i < src.size() && isdigit((unsigned char)src[i])) ++i;
		t.text = src.substr(start, i - start);
		t.number = stoi(t.text);
		t.kind = TokenKind::Number;
		pos += (int)t.text.size();
		return t;
	}

	// two-character or one-character tokens
	++i; ++pos;
	switch (c)
	{
		case '+': t.kind = TokenKind::Plus; return t;
		case '-': t.kind = TokenKind::Minus; return t;
		case '*': t.kind = TokenKind::Star; return t;
		case '/':
			// handle comments: // and /* */
			if (i < src.size() && src[i] == '/')
			{ // line comment
				while (i < src.size() && src[i] != '\n') ++i;
				return next();
			}
			if (i < src.size() && src[i] == '*')
			{ // block comment
				++i;
				while (i + 1 < src.size() && !(src[i] == '*' && src[i + 1] == '/')) ++i;
				if (i + 1 < src.size()) i += 2;
				return next();
			}
			t.kind = TokenKind::Slash; return t;
		case '(': t.kind = TokenKind::LParen; return t;
		case ')': t.kind = TokenKind::RParen; return t;
		case '{': t.kind = TokenKind::LBrace; return t;
		case '}': t.kind = TokenKind::RBrace; return t;
		case ';': t.kind = TokenKind::Semicolon; return t;
		case ',': t.kind = TokenKind::Comma; return t;
		case '=': t.kind = TokenKind::Assign; return t;
		default:
			t.kind = TokenKind::Unknown;
			t.text = string(1, c);
			return t;
	}
}

