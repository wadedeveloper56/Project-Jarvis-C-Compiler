#include "pch.h"
#include "Parser.h"

Parser::Parser(vector<Token> tokens) : tokens_(move(tokens)), index_(0) {}

unique_ptr<ProgramNode> Parser::parseProgram() 
{
	auto program = make_unique<ProgramNode>();
	while (peek().type != TokenType::Eof)
	{
		program->externalDeclarations.push_back(parseExternalDeclaration());
	}
	return program;
}

Token Parser::peek() { return tokens_[index_]; }
Token Parser::advance() { return tokens_[index_++]; }

void Parser::consume(TokenType type, const string& errorMsg) 
{
	if (peek().type == type)
	{
		advance();
	}
	else
	{
		throw runtime_error("Parser Error: " + errorMsg + " Got: " + peek().value);
	}
}

unique_ptr<ASTNode> Parser::parseExternalDeclaration() 
{
	// C99 external declarations start with a type speculator
	string type = advance().value; // e.g., "int" or "void"
	string name = advance().value; // Identifier name

	if (peek().type == TokenType::OpenParen)
	{
		// It's a function declaration/definition
		return parseFunctionDefinition(type, name);
	}
	else
	{
		// It's a global variable declaration
		string val = "";
		if (peek().type == TokenType::Equal)
		{
			advance();
			val = advance().value; // grab literal value
		}
		consume(TokenType::Semicolon, "Expected ';' after global variable declaration");
		return make_unique<VarDeclNode>(type, name, val, true);
	}
}

unique_ptr<FunctionDeclNode> Parser::parseFunctionDefinition(const string& returnType, const string& name) 
{
	auto funcNode = make_unique<FunctionDeclNode>(returnType, name);
	consume(TokenType::OpenParen, "Expected '('");

	// Parse parameters
	if (peek().type != TokenType::CloseParen)
	{
		while (true)
		{
			string pType = advance().value;
			string pName = advance().value;
			funcNode->params.emplace_back(pType, pName);
			if (peek().type == TokenType::Comma)
			{
				advance();
			}
			else
			{
				break;
			}
		}
	}
	consume(TokenType::CloseParen, "Expected ')'");
	consume(TokenType::OpenBrace, "Expected '{' to start function body");

	// Parse simple body block
	while (peek().type != TokenType::CloseBrace)
	{
		if (peek().type == TokenType::Keyword_return)
		{
			advance();
			string retVal = advance().value;
			consume(TokenType::Semicolon, "Expected ';'");
			funcNode->body.push_back(make_unique<ReturnStmtNode>(retVal));
		}
		else if (peek().type == TokenType::Keyword_int)
		{
			// Local variable
			string lType = advance().value;
			string lName = advance().value;
			string lVal = "";
			if (peek().type == TokenType::Equal)
			{
				advance();
				lVal = advance().value;
			}
			consume(TokenType::Semicolon, "Expected ';'");
			funcNode->body.push_back(make_unique<VarDeclNode>(lType, lName, lVal, false));
		}
		else
		{
			throw runtime_error("Unsupported statement in function body: " + peek().value);
		}
	}
	consume(TokenType::CloseBrace, "Expected '}'");
	return funcNode;
}

