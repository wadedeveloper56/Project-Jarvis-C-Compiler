#include "pch.h"
#include "Lexer.h"

using namespace std;

Lexer::Lexer(string s) : src(move(s)) {}

Token Lexer::next()
{
	// skip whitespace while updating line/column
	while (i < src.size())
	{
		char c = src[i];
		if (!isspace((unsigned char)c)) break;
		if (c == '\n')
		{
			++i; ++pos; ++line; column = 1; continue;
		}
		// for tabs, treat as single column advance
		++i; ++pos; ++column;
	}

	if (i >= src.size())
	{
		Token t; t.kind = TokenKind::EndToken; t.text = "<EOF>"; t.number = 0;
		t.pos = (int)i; t.endPos = (int)i; t.line = line; t.column = column; t.endLine = line; t.endColumn = column;
		return t;
	}

	int start = (int)i;
	int startLine = line;
	int startCol = column;
	Token t; t.pos = start; t.line = startLine; t.column = startCol;

	char c = src[i];
	// identifiers / keywords
	if (isalpha((unsigned char)c) || c == '_')
	{
		size_t s = i;
		while (i < src.size() && (isalnum((unsigned char)src[i]) || src[i] == '_')) { ++i; ++pos; ++column; }
		t.text = src.substr(s, i - s);
		if (t.text == "int") t.kind = TokenKind::IntToken;
		else if (t.text == "void") t.kind = TokenKind::VoidToken;
		else if (t.text == "return") t.kind = TokenKind::ReturnToken;
		else if (t.text == "unsigned") t.kind = TokenKind::UnsignedToken;
		else t.kind = TokenKind::IdentifierToken;
		t.endPos = (int)i - 1; t.endLine = line; t.endColumn = column - 1;
		return t;
	}

	// numbers
	if (isdigit((unsigned char)c))
	{
		size_t s = i;
		while (i < src.size() && isdigit((unsigned char)src[i])) { ++i; ++pos; ++column; }
		t.text = src.substr(s, i - s);
		t.number = stoi(t.text);
		t.kind = TokenKind::NumberToken;
		t.endPos = (int)i - 1; t.endLine = line; t.endColumn = column - 1;
		return t;
	}

	// single-character tokens and comments
	++i; ++pos; ++column;
	switch (c)
	{
		case '+': t.kind = TokenKind::PlusToken; t.text = "+"; break;
		case '-': t.kind = TokenKind::MinusToken; t.text = "-"; break;
		case '*': t.kind = TokenKind::StarToken; t.text = "*"; break;
		case '/':
			if (i < src.size() && src[i] == '/')
			{ // line comment
// consume until newline
				while (i < src.size() && src[i] != '\n') { ++i; ++pos; ++column; }
				return next();
			}
			if (i < src.size() && src[i] == '*')
			{ // block comment
				++i; ++pos; ++column;
				while (i + 1 < src.size() && !(src[i] == '*' && src[i + 1] == '/'))
				{
					if (src[i] == '\n') { ++i; ++pos; ++line; column = 1; }
					else { ++i; ++pos; ++column; }
				}
				if (i + 1 < src.size()) { i += 2; pos += 2; column += 2; }
				return next();
			}
			t.kind = TokenKind::SlashToken; break;
		case '(': t.kind = TokenKind::LParenToken; t.text = "("; break;
		case ')': t.kind = TokenKind::RParenToken; t.text = ")"; break;
		case '{': t.kind = TokenKind::LBraceToken; t.text = "{"; break;
		case '}': t.kind = TokenKind::RBraceToken; t.text = "}"; break;
		case ';': t.kind = TokenKind::SemicolonToken; t.text = ";"; break;
		case ',': t.kind = TokenKind::CommaToken; t.text = ","; break;
		case '=': t.kind = TokenKind::AssignToken; t.text = "="; break;
		default:
			t.kind = TokenKind::UnknownToken;
			t.text = string(1, c);
			break;
	}
	t.endPos = (int)i - 1; t.endLine = line; t.endColumn = column - 1;
	return t;
}

