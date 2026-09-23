#include "pch.h"
#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer()
{
}

// Simple control-flow check: ensure every path in the function body returns when function is non-void
void SemanticAnalyzer::checkFunctionReturns(FunctionDeclaration& f)
{
	// conservative approach: traverse the top-level statements of the compound body
	// and verify at least one return exists; for full path-sensitivity a CFG is needed.
	if (!f.body) return;
	auto cs = dynamic_cast<CompoundStatement*>(f.body.get());
	if (!cs) return;
	bool foundReturn = false;
	for (auto& s : cs->stmts)
	{
		if (dynamic_cast<ReturnStatement*>(s.get())) { foundReturn = true; break; }
		// also check nested compound statements
		if (auto inner = dynamic_cast<CompoundStatement*>(s.get()))
		{
			for (auto& is : inner->stmts)
			{
				if (dynamic_cast<ReturnStatement*>(is.get())) { foundReturn = true; break; }
			}
			if (foundReturn) break;
		}
	}
	if (!foundReturn)
		error("Function '" + f.name + "' may not return on all paths", f.loc);
}

bool SemanticAnalyzer::analyze(Program& prog)
{
	errs.clear();
	// First pass: register function prototypes and global vars
	for (auto& d : prog.declarations)
	{
		if (auto vd = dynamic_cast<VariableDeclaration*>(d.get()))
		{
			if (!symbols.declareVariable(vd->name, vd->type))
				error("Redeclaration of global variable '" + vd->name + "'", vd->loc);
		}
		else if (auto fd = dynamic_cast<FunctionDeclaration*>(d.get()))
		{
			vector<string> ptypes;
			for (auto& p : fd->params) ptypes.push_back(p.first);
			if (!symbols.declareFunctionProto(fd->name, fd->retType, ptypes))
				error("Conflicting declaration of function '" + fd->name + "'", fd->loc);
		}
	}

	return errs.empty();
}

// Visitors
void SemanticAnalyzer::visit(Program& n)
{
	// handled in analyze()
}

void SemanticAnalyzer::visit(VariableDeclaration& n)
{
	if (n.init)
	{
		string t = evalExprType(n.init.get());
		if (t != n.type)
			error("Type mismatch in initializer for '" + n.name + "' expected '" + n.type + "' got '" + t + "'", n.loc);
	}
}

void SemanticAnalyzer::visit(FunctionDeclaration& n)
{
	// define function (ensure prototype matches)
	vector<string> ptypes;
	for (auto& p : n.params) ptypes.push_back(p.first);
	auto defres = symbols.defineFunction(n.name, n.retType, ptypes);
	if (defres == SymbolTable::DefineResult::PrototypeMismatch)
	{
		error("Function definition for '" + n.name + "' does not match previous prototype", n.loc);
		// still proceed to allow further checks
	}
	else if (defres == SymbolTable::DefineResult::AlreadyDefined)
	{
		error("Multiple definitions of function '" + n.name + "'", n.loc);
		// skip body analysis to avoid cascading errors
		return;
	}

	// enter function scope
	symbols.pushScope();
	// declare parameters as local variables
	for (auto& p : n.params)
	{
		if (!symbols.declareVariable(p.second, p.first))
			error("Parameter name redeclared '" + p.second + "'", n.loc);
	}

	string prevRet = currentFunctionRetType;
	currentFunctionRetType = n.retType;
	if (n.body) {
		n.body->accept(*this);
		// control-flow check: ensure non-void functions return on all paths
		if (n.retType != "void") checkFunctionReturns(n);
	}
	currentFunctionRetType = prevRet;

	symbols.popScope();
}

void SemanticAnalyzer::visit(CompoundStatement& n)
{
	symbols.pushScope();
	for (auto& d : n.localDeclarations) d->accept(*this);
	for (auto& s : n.stmts) s->accept(*this);
	symbols.popScope();
}

void SemanticAnalyzer::visit(ReturnStatement& n)
{
	if (currentFunctionRetType.empty())
	{
		error("Return statement outside of function", n.loc);
		return;
	}
	if (currentFunctionRetType == "void")
	{
		if (n.expr) error("Returning value from void function", n.loc);
	}
	else
	{
		if (!n.expr) error("Non-void function must return a value", n.loc);
		else
		{
			string t = evalExprType(n.expr.get());
			if (t != currentFunctionRetType) error("Return type mismatch: expected '" + currentFunctionRetType + "' got '" + t + "'", n.loc);
		}
	}
}

void SemanticAnalyzer::visit(ExpressionStatement& n)
{
	if (n.expr) evalExprType(n.expr.get());
}

void SemanticAnalyzer::visit(NumberExpression& n)
{
	// nothing
}

void SemanticAnalyzer::visit(VarExpression& n)
{
	const SymbolInfo* si = symbols.lookup(n.name);
	if (!si) error("Use of undeclared identifier '" + n.name + "'", n.loc);
}

void SemanticAnalyzer::visit(BinaryExpression& n)
{
	string l = evalExprType(n.lhs.get());
	string r = evalExprType(n.rhs.get());
	if (l != "int" || r != "int") error("Binary operator applied to non-int types.", n.loc);
}

void SemanticAnalyzer::visit(AssignExpression& n)
{
	const SymbolInfo* si = symbols.lookup(n.name);
	if (!si) { error("Assignment to undeclared variable '" + n.name + "'", n.loc); return; }
	string rt = evalExprType(n.value.get());
	if (si->type != rt) error("Assignment type mismatch for '" + n.name + "': expected '" + si->type + "' got '" + rt + "'", n.loc);
}

void SemanticAnalyzer::visit(CallExpression& n)
{
	const SymbolInfo* si = symbols.lookup(n.callee);
	if (!si) { error("Call to undeclared function '" + n.callee + "'", n.loc); return; }
	if (si->kind != SymbolKind::Function) { error("'" + n.callee + "' is not a function", n.loc); return; }
	if (n.args.size() != si->paramTypes.size()) { error("Wrong number of arguments in call to '" + n.callee + "'", n.loc); return; }
	for (size_t i = 0; i < n.args.size(); ++i)
	{
		string t = evalExprType(n.args[i].get());
		if (t != si->paramTypes[i]) error("Argument type mismatch in call to '" + n.callee + "'", n.loc);
	}
}

// Evaluate expression type, returns type name or empty string on error
string SemanticAnalyzer::evalExprType(Expression* e)
{
	if (!e) return string();
	if (auto ne = dynamic_cast<NumberExpression*>(e)) return string("int");
	if (auto ve = dynamic_cast<VarExpression*>(e))
	{
		const SymbolInfo* si = symbols.lookup(ve->name);
		if (!si) { error("Use of undeclared identifier '" + ve->name + "'", ve->loc); return string(); }
		return si->type;
	}
	if (auto be = dynamic_cast<BinaryExpression*>(e))
	{
		string l = evalExprType(be->lhs.get());
		string r = evalExprType(be->rhs.get());
		if (l != "int" || r != "int") { error("Binary operator applied to non-int types", be->loc); return string(); }
		return string("int");
	}
	if (auto ae = dynamic_cast<AssignExpression*>(e))
	{
		const SymbolInfo* si = symbols.lookup(ae->name);
		if (!si) { error("Assignment to undeclared variable '" + ae->name + "'", ae->loc); return string(); }
		string rt = evalExprType(ae->value.get());
		if (si->type != rt) { error("Assignment type mismatch for '" + ae->name + "'", ae->loc); return string(); }
		return si->type;
	}
	if (auto ce = dynamic_cast<CallExpression*>(e))
	{
		const SymbolInfo* si = symbols.lookup(ce->callee);
		if (!si) { error("Call to undeclared function '" + ce->callee + "'", ce->loc); return string(); }
		if (si->kind != SymbolKind::Function) { error("'" + ce->callee + "' is not a function", ce->loc); return string(); }
		if (ce->args.size() != si->paramTypes.size()) { error("Wrong number of arguments in call to '" + ce->callee + "'", ce->loc); return string(); }
		for (size_t i = 0; i < ce->args.size(); ++i)
		{
			string t = evalExprType(ce->args[i].get());
			if (t != si->paramTypes[i]) { error("Argument type mismatch in call to '" + ce->callee + "'", ce->loc); return string(); }
		}
		return si->type;
	}
	// unknown expr
	if (e) error("Unknown expression type encountered", static_cast<Node*>(e)->loc);
	else error("Unknown expression type encountered");
	return string();
}

void SemanticAnalyzer::error(const string& msg)
{
	errs.push_back(msg);
}

void SemanticAnalyzer::error(const string& msg, const Node::SourceLoc& loc)
{
	ostringstream os;
	if (loc.startLine == loc.endLine && loc.startColumn == loc.endColumn) {
		os << "line " << loc.startLine << ", col " << loc.startColumn << ": " << msg;
	} else {
		os << "line " << loc.startLine << ", col " << loc.startColumn
		   << " - line " << loc.endLine << ", col " << loc.endColumn << ": " << msg;
	}
	errs.push_back(os.str());
}
