#include "pch.h"
#include "Parser.h"

using namespace std;

void Parser::next() { cur = lexer.next(); }

bool Parser::accept(TokenKind k)
{
	if (cur.kind == k) { next(); return true; }
	return false;
}

void Parser::expect(TokenKind k, const char* msg)
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

Parser::Parser(string s) : lexer(move(s))
{
	next();
}

unique_ptr<Program> Parser::parseProgram()
{
	auto prog = make_unique<Program>();
	while (cur.kind != TokenKind::EndToken)
	{
		prog->declarations.push_back(parseDecl());
	}
	return prog;
}

unique_ptr<Declaration> Parser::parseDecl()
{
	string type = parseType();
	if (cur.kind != TokenKind::IdentifierToken)
	{
		ostringstream os;
		os << "Parse error at line " << cur.line << ", col " << cur.column << ": expected identifier after type";
		throw runtime_error(os.str());
	}
	Token idTok = cur;
	string name = cur.text;
	next();
	if (cur.kind == TokenKind::LParenToken)
	{
		// function declaration
		auto fn = make_unique<FunctionDeclaration>();
		fn->loc.startPos = idTok.pos; fn->loc.startLine = idTok.line; fn->loc.startColumn = idTok.column;
		fn->loc.endPos = idTok.endPos; fn->loc.endLine = idTok.endLine; fn->loc.endColumn = idTok.endColumn;
		fn->retType = type;
		fn->name = name;
		next(); // consume '('
		if (cur.kind != TokenKind::RParenToken)
		{
			// parse params
			while (true)
			{
				string ptype = parseType();
				// allow 'void' as the sole parameter to indicate no parameters: int f(void)
				if (ptype == "void" && cur.kind == TokenKind::RParenToken)
				{
					// consume ')' and treat as no-parameter list
					expect(TokenKind::RParenToken);
					break;
				}
				if (cur.kind != TokenKind::IdentifierToken)
				{
					ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": expected parameter name";
					throw runtime_error(os.str());
				}
				string pname = cur.text; Token pTok = cur; next();
				fn->params.emplace_back(ptype, pname);
				if (accept(TokenKind::CommaToken)) continue;
				expect(TokenKind::RParenToken);
				break;
			}
		}
		else
		{
			expect(TokenKind::RParenToken);
		}
		// parse body
		fn->body = parseCompoundStmt();
		return fn;
	}
	else
	{
		// global var declaration
		auto vd = make_unique<VariableDeclaration>();
		vd->loc.startPos = idTok.pos; vd->loc.startLine = idTok.line; vd->loc.startColumn = idTok.column;
		vd->loc.endPos = idTok.endPos; vd->loc.endLine = idTok.endLine; vd->loc.endColumn = idTok.endColumn;
		vd->type = type; vd->name = name;
		if (accept(TokenKind::AssignToken))
		{
			vd->init = parseExpression();
		}
		expect(TokenKind::SemicolonToken);
		return vd;
	}
}

unique_ptr<CompoundStatement> Parser::parseCompoundStmt()
{
	Token lbraceTok = cur;
	expect(TokenKind::LBraceToken);
	auto comp = make_unique<CompoundStatement>();
	comp->loc.startPos = lbraceTok.pos; comp->loc.startLine = lbraceTok.line; comp->loc.startColumn = lbraceTok.column;
	comp->loc.endPos = lbraceTok.endPos; comp->loc.endLine = lbraceTok.endLine; comp->loc.endColumn = lbraceTok.endColumn;
	while (cur.kind != TokenKind::RBraceToken && cur.kind != TokenKind::EndToken)
	{
		// either local decl (type identifier ;) or stmt
		if (cur.kind == TokenKind::IntToken || cur.kind == TokenKind::VoidToken)
		{
			// for simplicity only accept 'int' local decls (void params only as function param)
			string t = parseType();
			if (cur.kind != TokenKind::IdentifierToken)
			{
				ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": expected identifier in local declaration";
				throw runtime_error(os.str());
			}
			Token idTok = cur;
			string n = cur.text; next();
			auto vd = make_unique<VariableDeclaration>();
			vd->loc.startPos = idTok.pos; vd->loc.startLine = idTok.line; vd->loc.startColumn = idTok.column;
			vd->loc.endPos = idTok.endPos; vd->loc.endLine = idTok.endLine; vd->loc.endColumn = idTok.endColumn;
			vd->type = t; vd->name = n;
			if (accept(TokenKind::AssignToken)) vd->init = parseExpression();
			expect(TokenKind::SemicolonToken);
			comp->localDeclarations.push_back(move(vd));
		}
		else
		{
			comp->statements.push_back(parseStmt());
		}
	}
	expect(TokenKind::RBraceToken);
	return comp;
}

unique_ptr<Statement> Parser::parseStmt()
{
	if (cur.kind == TokenKind::ReturnToken)
	{
		Token retTok = cur;
		next();
		auto ret = make_unique<ReturnStatement>();
		ret->loc.startPos = retTok.pos; ret->loc.startLine = retTok.line; ret->loc.startColumn = retTok.column;
		ret->loc.endPos = retTok.endPos; ret->loc.endLine = retTok.endLine; ret->loc.endColumn = retTok.endColumn;
		if (cur.kind != TokenKind::SemicolonToken) ret->expr = parseExpression();
		expect(TokenKind::SemicolonToken);
		return ret;
	}
	if (cur.kind == TokenKind::LBraceToken) return parseCompoundStmt();
	// expression statement
	auto es = make_unique<ExpressionStatement>();
	if (cur.kind != TokenKind::SemicolonToken) es->expr = parseExpression();
	expect(TokenKind::SemicolonToken);
	return es;
}

unique_ptr<Expression> Parser::parseExpression()
{
	return parseAssignment();
}

unique_ptr<Expression> Parser::parseAssignment()
{
	// parse left as primary or identifier; support simple 'id = expr'
	auto left = parseAddSub();
	if (auto* ve = dynamic_cast<VariableExpression*>(left.get()))
	{
		if (cur.kind == TokenKind::AssignToken)
		{
			Token assignTok = cur;
			next();
			auto val = parseAssignment();
			auto asn = make_unique<AssignExpression>(ve->name, move(val));
			asn->loc.startPos = assignTok.pos; asn->loc.startLine = assignTok.line; asn->loc.startColumn = assignTok.column;
			asn->loc.endPos = assignTok.endPos; asn->loc.endLine = assignTok.endLine; asn->loc.endColumn = assignTok.endColumn;
			return asn;
		}
	}
	return left;
}

unique_ptr<Expression> Parser::parseAddSub()
{
	auto node = parseMulDiv();
	while (cur.kind == TokenKind::PlusToken || cur.kind == TokenKind::MinusToken)
	{
		Token opTok = cur;
		char op = (cur.kind == TokenKind::PlusToken ? '+' : '-'); next();
		auto rhs = parseMulDiv();
		auto bin = make_unique<BinaryExpression>(op, move(node), move(rhs));
		bin->loc.startPos = opTok.pos; bin->loc.startLine = opTok.line; bin->loc.startColumn = opTok.column;
		bin->loc.endPos = opTok.endPos; bin->loc.endLine = opTok.endLine; bin->loc.endColumn = opTok.endColumn;
		node = move(bin);
	}
	return node;
}

unique_ptr<Expression> Parser::parseMulDiv()
{
	auto node = parseUnary();
	while (cur.kind == TokenKind::StarToken || cur.kind == TokenKind::SlashToken)
	{
		Token opTok = cur;
		char op = (cur.kind == TokenKind::StarToken ? '*' : '/'); next();
		auto rhs = parseUnary();
		auto bin = make_unique<BinaryExpression>(op, move(node), move(rhs));
		bin->loc.startPos = opTok.pos; bin->loc.startLine = opTok.line; bin->loc.startColumn = opTok.column;
		bin->loc.endPos = opTok.endPos; bin->loc.endLine = opTok.endLine; bin->loc.endColumn = opTok.endColumn;
		node = move(bin);
	}
	return node;
}

unique_ptr<Expression> Parser::parseUnary()
{
	if (cur.kind == TokenKind::PlusToken) { next(); return parseUnary(); }
	if (cur.kind == TokenKind::MinusToken)
	{
		Token minusTok = cur;
		next();
		auto rhs = parseUnary();
		auto zero = make_unique<NumberExpression>(0);
		auto bin = make_unique<BinaryExpression>('-', move(zero), move(rhs));
		bin->loc.startPos = minusTok.pos; bin->loc.startLine = minusTok.line; bin->loc.startColumn = minusTok.column;
		bin->loc.endPos = minusTok.endPos; bin->loc.endLine = minusTok.endLine; bin->loc.endColumn = minusTok.endColumn;
		return bin;
	}
	return parsePrimary();
}

unique_ptr<Expression> Parser::parsePrimary()
{
	if (cur.kind == TokenKind::NumberToken)
	{
		Token numTok = cur;
		auto n = make_unique<NumberExpression>(cur.number);
		n->loc.startPos = numTok.pos; n->loc.startLine = numTok.line; n->loc.startColumn = numTok.column;
		n->loc.endPos = numTok.endPos; n->loc.endLine = numTok.endLine; n->loc.endColumn = numTok.endColumn;
		next(); return n;
	}
	if (cur.kind == TokenKind::IdentifierToken)
	{
		Token idTok = cur;
		string name = cur.text; next();
		if (cur.kind == TokenKind::LParenToken)
		{
			next(); // consume '('
			auto call = make_unique<CallExpression>(name);
			call->loc.startPos = idTok.pos; call->loc.startLine = idTok.line; call->loc.startColumn = idTok.column;
			call->loc.endPos = idTok.endPos; call->loc.endLine = idTok.endLine; call->loc.endColumn = idTok.endColumn;
			if (cur.kind != TokenKind::RParenToken)
			{
				while (true)
				{
					call->args.push_back(parseExpression());
					if (accept(TokenKind::CommaToken)) continue;
					expect(TokenKind::RParenToken);
					break;
				}
			}
			return call;
		}
		auto v = make_unique<VariableExpression>(name);
		v->loc.startPos = idTok.pos; v->loc.startLine = idTok.line; v->loc.startColumn = idTok.column;
		v->loc.endPos = idTok.endPos; v->loc.endLine = idTok.endLine; v->loc.endColumn = idTok.endColumn;
		return v;
	}
	if (accept(TokenKind::LParenToken))
	{
		auto e = parseExpression();
		expect(TokenKind::RParenToken);
		return e;
	}
	{
		ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": unexpected token in primary (got '" << cur.text << "')";
		throw runtime_error(os.str());
	}
}

string Parser::parseType()
{
	if (accept(TokenKind::IntToken)) return "int";
	if (accept(TokenKind::VoidToken)) return "void";
	if (accept(TokenKind::UnsignedToken))
	{
		if (accept(TokenKind::IntToken)) return "unsigned int";
	}
	ostringstream os; os << "Parse error at line " << cur.line << ", col " << cur.column << ": unknown type '" << cur.text << "'";
	throw runtime_error(os.str());
}

