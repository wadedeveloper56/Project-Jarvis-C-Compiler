#include "pch.h"
#include "Lexer.h"

using namespace std;

Lexer::Lexer(string s) : src(move(s)) {}

Token Lexer::next()
{
	// skip whitespace while updating line/column
	while (i < src.size()) {
		char c = src[i];
		if (!isspace((unsigned char)c)) break;
		if (c == '\n') {
			++i; ++pos; ++line; column = 1; continue;
		}
		// for tabs, treat as single column advance
		++i; ++pos; ++column;
	}

	if (i >= src.size()) {
		Token t; t.kind = TokenKind::End; t.text = ""; t.number = 0;
		t.pos = (int)i; t.endPos = (int)i; t.line = line; t.column = column; t.endLine = line; t.endColumn = column;
		return t;
	}

	int start = (int)i;
	int startLine = line;
	int startCol = column;
	Token t; t.pos = start; t.line = startLine; t.column = startCol;

	char c = src[i];
	// identifiers / keywords
	if (isalpha((unsigned char)c) || c == '_') {
		size_t s = i;
		while (i < src.size() && (isalnum((unsigned char)src[i]) || src[i] == '_')) { ++i; ++pos; ++column; }
		t.text = src.substr(s, i - s);
		if (t.text == "int") t.kind = TokenKind::Int;
		else if (t.text == "void") t.kind = TokenKind::Void;
		else if (t.text == "return") t.kind = TokenKind::Return;
		else t.kind = TokenKind::Identifier;
		t.endPos = (int)i - 1; t.endLine = line; t.endColumn = column - 1;
		return t;
	}

	// numbers
	if (isdigit((unsigned char)c)) {
		size_t s = i;
		while (i < src.size() && isdigit((unsigned char)src[i])) { ++i; ++pos; ++column; }
		t.text = src.substr(s, i - s);
		t.number = stoi(t.text);
		t.kind = TokenKind::Number;
		t.endPos = (int)i - 1; t.endLine = line; t.endColumn = column - 1;
		return t;
	}

	// single-character tokens and comments
	++i; ++pos; ++column;
	switch (c) {
	case '+': t.kind = TokenKind::Plus; break;
	case '-': t.kind = TokenKind::Minus; break;
	case '*': t.kind = TokenKind::Star; break;
	case '/':
		if (i < src.size() && src[i] == '/') { // line comment
			// consume until newline
			while (i < src.size() && src[i] != '\n') { ++i; ++pos; ++column; }
			return next();
		}
		if (i < src.size() && src[i] == '*') { // block comment
			++i; ++pos; ++column;
			while (i + 1 < src.size() && !(src[i] == '*' && src[i+1] == '/')) {
				if (src[i] == '\n') { ++i; ++pos; ++line; column = 1; }
				else { ++i; ++pos; ++column; }
			}
			if (i + 1 < src.size()) { i += 2; pos += 2; column += 2; }
			return next();
		}
		t.kind = TokenKind::Slash; break;
	case '(': t.kind = TokenKind::LParen; break;
	case ')': t.kind = TokenKind::RParen; break;
	case '{': t.kind = TokenKind::LBrace; break;
	case '}': t.kind = TokenKind::RBrace; break;
	case ';': t.kind = TokenKind::Semicolon; break;
	case ',': t.kind = TokenKind::Comma; break;
	case '=': t.kind = TokenKind::Assign; break;
	default:
		t.kind = TokenKind::Unknown;
		t.text = string(1, c);
		break;
	}
	t.endPos = (int)i - 1; t.endLine = line; t.endColumn = column - 1;
	return t;
}

