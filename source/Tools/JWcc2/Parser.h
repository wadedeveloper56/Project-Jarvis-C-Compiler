#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "Lexer.h"
#include "AST.h"

using namespace std;

class Parser
{
	Lexer lexer;
	Token cur;
	istream& in;

	void next() { cur = lexer.next(); }

	bool accept(TokenKind k)
	{
		if (cur.kind == k) { next(); return true; }
		return false;
	}

	void expect(TokenKind k, const char* msg = "")
	{
		if (cur.kind != k)
		{
			ostringstream os;
			os << "1 Parse error at line " << cur.line << ", col " << cur.column << " expected token";
			if (*msg) os << " (" << msg << ")";
			os << " (got '" << cur.text << "')";
			throw runtime_error(os.str());
		}
		next();
	}

public:
	Parser(string s, istream& in) : lexer(move(s), in), in(in) { next(); }

	unique_ptr<Program> parseProgram()
	{
		auto prog = make_unique<Program>();
		while (cur.kind != TokenKind::End)
		{
			prog->declarations.push_back(parseDecl());
		}
		return prog;
	}

	unique_ptr<Declaration> parseDecl()
	{
		// Only support 'int' and 'void' return types
		string type = parseType();
		if (cur.kind != TokenKind::Identifier)
		{
			ostringstream os; 
			os << "Parse error at line " << cur.line << ", col " << cur.column << ": expected identifier after type";
			throw runtime_error(os.str());
		}
		Token idTok = cur;
		string name = cur.text;
		next();
		if (cur.kind == TokenKind::LParen)
		{
			// function declaration
			auto fn = make_unique<FunctionDecl>();
			fn->loc.startPos = idTok.pos; fn->loc.startLine = idTok.line; fn->loc.startColumn = idTok.column;
			fn->loc.endPos = idTok.endPos; fn->loc.endLine = idTok.endLine; fn->loc.endColumn = idTok.endColumn;
			fn->retType = type;
			fn->name = name;
			next(); // consume '('
			if (cur.kind != TokenKind::RParen)
			{
				// parse params
				while (true)
				{
					string ptype = parseType();
					// allow 'void' as the sole parameter to indicate no parameters: int f(void)
					if (ptype == "void" && cur.kind == TokenKind::RParen)
					{
						// consume ')' and treat as no-parameter list
						expect(TokenKind::RParen);
						break;
					}
					if (cur.kind != TokenKind::Identifier)
					{
						ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": expected parameter name";
						throw runtime_error(os.str());
					}
					string pname = cur.text; Token pTok = cur; next();
					fn->params.emplace_back(ptype, pname);
					if (accept(TokenKind::Comma)) continue;
					expect(TokenKind::RParen);
					break;
				}
			}
			else
			{
				expect(TokenKind::RParen);
			}
			// parse body
			fn->body = parseCompoundStmt();
			return fn;
		}
		else
		{
			// global var declaration
			auto vd = make_unique<VarDecl>();
			vd->loc.startPos = idTok.pos; vd->loc.startLine = idTok.line; vd->loc.startColumn = idTok.column;
			vd->loc.endPos = idTok.endPos; vd->loc.endLine = idTok.endLine; vd->loc.endColumn = idTok.endColumn;
			vd->type = type; vd->name = name;
			if (accept(TokenKind::Assign))
			{
				vd->init = parseExpression();
			}
			expect(TokenKind::Semicolon);
			return vd;
		}
	}

	unique_ptr<Statement> parseCompoundStmt()
	{
		Token lbraceTok = cur;
		expect(TokenKind::LBrace);
		auto comp = make_unique<CompoundStatement>();
		comp->loc.startPos = lbraceTok.pos; comp->loc.startLine = lbraceTok.line; comp->loc.startColumn = lbraceTok.column;
		comp->loc.endPos = lbraceTok.endPos; comp->loc.endLine = lbraceTok.endLine; comp->loc.endColumn = lbraceTok.endColumn;
		while (cur.kind != TokenKind::RBrace && cur.kind != TokenKind::End)
		{
			// either local decl (type identifier ;) or stmt
			if (cur.kind == TokenKind::Int || cur.kind == TokenKind::Void)
			{
				// for simplicity only accept 'int' local decls (void params only as function param)
				string t = parseType();
				if (cur.kind != TokenKind::Identifier)
				{
					ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": expected identifier in local declaration";
					throw runtime_error(os.str());
				}
				Token idTok = cur;
				string n = cur.text; next();
				auto vd = make_unique<VarDecl>();
				vd->loc.startPos = idTok.pos; vd->loc.startLine = idTok.line; vd->loc.startColumn = idTok.column;
				vd->loc.endPos = idTok.endPos; vd->loc.endLine = idTok.endLine; vd->loc.endColumn = idTok.endColumn;
				vd->type = t; vd->name = n;
				if (accept(TokenKind::Assign)) vd->init = parseExpression();
				expect(TokenKind::Semicolon);
				comp->localDeclarations.push_back(move(vd));
			}
			else
			{
				comp->stmts.push_back(parseStmt());
			}
		}
		expect(TokenKind::RBrace);
		return comp;
	}

	unique_ptr<Statement> parseStmt()
	{
		if (cur.kind == TokenKind::Return)
		{
			Token retTok = cur;
			next();
			auto ret = make_unique<ReturnStatement>();
			ret->loc.startPos = retTok.pos; ret->loc.startLine = retTok.line; ret->loc.startColumn = retTok.column;
			ret->loc.endPos = retTok.endPos; ret->loc.endLine = retTok.endLine; ret->loc.endColumn = retTok.endColumn;
			if (cur.kind != TokenKind::Semicolon) ret->expr = parseExpression();
			expect(TokenKind::Semicolon);
			return ret;
		}
		if (cur.kind == TokenKind::LBrace) return parseCompoundStmt();
		// expression statement
		auto es = make_unique<ExprStatement>();
		if (cur.kind != TokenKind::Semicolon) es->expr = parseExpression();
		expect(TokenKind::Semicolon);
		return es;
	}

	unique_ptr<Expression> parseExpression()
	{
		return parseAssignment();
	}

	unique_ptr<Expression> parseAssignment()
	{
		// parse left as primary or identifier; support simple 'id = expr'
		auto left = parseAddSub();
		if (auto* ve = dynamic_cast<VarExpr*>(left.get()))
		{
			if (cur.kind == TokenKind::Assign)
			{
				Token assignTok = cur;
				next();
				auto val = parseAssignment();
				auto asn = make_unique<AssignExpr>(ve->name, move(val));
				asn->loc.startPos = assignTok.pos; asn->loc.startLine = assignTok.line; asn->loc.startColumn = assignTok.column;
				asn->loc.endPos = assignTok.endPos; asn->loc.endLine = assignTok.endLine; asn->loc.endColumn = assignTok.endColumn;
				return asn;
			}
		}
		return left;
	}

	unique_ptr<Expression> parseAddSub()
	{
		auto node = parseMulDiv();
		while (cur.kind == TokenKind::Plus || cur.kind == TokenKind::Minus)
		{
			Token opTok = cur;
			char op = (cur.kind == TokenKind::Plus ? '+' : '-'); next();
			auto rhs = parseMulDiv();
			auto bin = make_unique<BinaryExpr>(op, move(node), move(rhs));
			bin->loc.startPos = opTok.pos; bin->loc.startLine = opTok.line; bin->loc.startColumn = opTok.column;
			bin->loc.endPos = opTok.endPos; bin->loc.endLine = opTok.endLine; bin->loc.endColumn = opTok.endColumn;
			node = move(bin);
		}
		return node;
	}

	unique_ptr<Expression> parseMulDiv()
	{
		auto node = parseUnary();
		while (cur.kind == TokenKind::Star || cur.kind == TokenKind::Slash)
		{
			Token opTok = cur;
			char op = (cur.kind == TokenKind::Star ? '*' : '/'); next();
			auto rhs = parseUnary();
			auto bin = make_unique<BinaryExpr>(op, move(node), move(rhs));
			bin->loc.startPos = opTok.pos; bin->loc.startLine = opTok.line; bin->loc.startColumn = opTok.column;
			bin->loc.endPos = opTok.endPos; bin->loc.endLine = opTok.endLine; bin->loc.endColumn = opTok.endColumn;
			node = move(bin);
		}
		return node;
	}

	unique_ptr<Expression> parseUnary()
	{
		if (cur.kind == TokenKind::Plus) { next(); return parseUnary(); }
		if (cur.kind == TokenKind::Minus)
		{
			Token minusTok = cur;
			next();
			auto rhs = parseUnary();
			auto zero = make_unique<NumberExpr>(0);
			auto bin = make_unique<BinaryExpr>('-', move(zero), move(rhs));
			bin->loc.startPos = minusTok.pos; bin->loc.startLine = minusTok.line; bin->loc.startColumn = minusTok.column;
			bin->loc.endPos = minusTok.endPos; bin->loc.endLine = minusTok.endLine; bin->loc.endColumn = minusTok.endColumn;
			return bin;
		}
		return parsePrimary();
	}

	unique_ptr<Expression> parsePrimary()
	{
		if (cur.kind == TokenKind::Number)
		{
			Token numTok = cur;
			auto n = make_unique<NumberExpr>(cur.number);
			n->loc.startPos = numTok.pos; n->loc.startLine = numTok.line; n->loc.startColumn = numTok.column;
			n->loc.endPos = numTok.endPos; n->loc.endLine = numTok.endLine; n->loc.endColumn = numTok.endColumn;
			next(); return n;
		}
		if (cur.kind == TokenKind::Identifier)
		{
			Token idTok = cur;
			string name = cur.text; next();
			if (cur.kind == TokenKind::LParen)
			{
				next(); // consume '('
				auto call = make_unique<CallExpr>(name);
				call->loc.startPos = idTok.pos; call->loc.startLine = idTok.line; call->loc.startColumn = idTok.column;
				call->loc.endPos = idTok.endPos; call->loc.endLine = idTok.endLine; call->loc.endColumn = idTok.endColumn;
				if (cur.kind != TokenKind::RParen)
				{
					while (true)
					{
						call->args.push_back(parseExpression());
						if (accept(TokenKind::Comma)) continue;
						expect(TokenKind::RParen);
						break;
					}
				}
				return call;
			}
			auto v = make_unique<VarExpr>(name);
			v->loc.startPos = idTok.pos; v->loc.startLine = idTok.line; v->loc.startColumn = idTok.column;
			v->loc.endPos = idTok.endPos; v->loc.endLine = idTok.endLine; v->loc.endColumn = idTok.endColumn;
			return v;
		}
		if (accept(TokenKind::LParen))
		{
			auto e = parseExpression();
			expect(TokenKind::RParen);
			return e;
		}
		{
			ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": unexpected token in primary (got '" << cur.text << "')";
			throw runtime_error(os.str());
		}
	}

	string parseType()
	{
		if (accept(TokenKind::Int)) return "int";
		if (accept(TokenKind::Void)) return "void";
		{
			ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": unknown type '" << cur.text << "'";
			throw runtime_error(os.str());
		}
	}
};

