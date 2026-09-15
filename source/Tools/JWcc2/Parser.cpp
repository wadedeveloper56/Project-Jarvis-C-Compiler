#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "Parser.h"

Parser::Parser(vector<Token> t) : tokens(move(t)) {}

// Main entry point: Program -> Function*
unique_ptr<ProgramNode> Parser::parse_program() {
	auto program = make_unique<ProgramNode>();

	while (peek().type != TokenType::EOFToken)
	{
		program->functions.push_back(parse_function());
	}
	return program;
}

// Function -> "int" Identifier "(" ")" "{" Statement* "}"
unique_ptr<FunctionNode> Parser::parse_function() {
	consume(TokenType::Keyword_int, "Expected return type 'int'");

	Token id_tok = consume(TokenType::Identifier, "Expected function name");
	string func_name = id_tok.value;

	consume(TokenType::OpenParen, "Expected '(' after function name");
	consume(TokenType::CloseParen, "Expected ')' after parameters");
	consume(TokenType::OpenBrace, "Expected '{' to start function body");

	vector<unique_ptr<StmtNode>> body;
	while (peek().type != TokenType::CloseBrace)
	{
		if (peek().type == TokenType::EOFToken)
		{
			throw runtime_error("Unexpected EOF: Missing closing brace '}'");
		}
		body.push_back(parse_statement());
	}

	consume(TokenType::CloseBrace, "Expected '}' to close function body");

	return make_unique<FunctionNode>(func_name, move(body));
}

// Statement -> "return" Expression ";"
unique_ptr<StmtNode> Parser::parse_statement() {
	if (peek().type == TokenType::Keyword_return)
	{
		consume(TokenType::Keyword_return, "Expected 'return'");
		auto expr = parse_expression();
		consume(TokenType::Semicolon, "Expected ';' after return statement");
		return make_unique<ReturnStmtNode>(move(expr));
	}

	throw runtime_error("Unknown statement type starting with token: " + peek().value);
}

// Expression -> IntegerLiteral
unique_ptr<ExprNode> Parser::parse_expression() {
	if (peek().type == TokenType::IntegerLiteral)
	{
		Token lit_tok = consume(TokenType::IntegerLiteral, "Expected integer literal");
		return make_unique<LiteralNode>(lit_tok.value);
	}

	throw runtime_error("Expected expression, found: " + peek().value);
}

// --- Parser Utilities ---

// Look at the current token without moving forward
Token Parser::peek() const {
	if (pos >= tokens.size()) return { TokenType::EOFToken, "" };
	return tokens[pos];
}

// Assert the current token type matches expectations, then advance
Token Parser::consume(TokenType type, const string& error_msg) {
	Token current = peek();
	if (current.type != type)
	{
		throw runtime_error("Parser Error: " + error_msg + " Got: '" + current.value + "'");
	}
	pos++;
	return current;
}

