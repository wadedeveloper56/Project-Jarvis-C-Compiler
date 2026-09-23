#include "pch.h"
#include "ASTPrinter.h"

using namespace std;

void ASTPrinter::ind() { for (int i = 0; i < indent; ++i) cout << "  "; }
//void printLoc(const Node& n) { cout << " [" << n.loc.startLine << ":" << n.loc.startColumn << "-" << n.loc.endLine << ":" << n.loc.endColumn << "]"; }

void ASTPrinter::visit(Program& n) {
	cout << "Program\n";
	indent++;
	for (auto& d : n.declarations) { d->accept(*this); }
	indent--;
}

void ASTPrinter::visit(VariableDeclaration& n) {
	ind(); cout << "VariableDeclaration type=" << n.type << " name=" << n.name;
	// printLoc(n);
	if (n.init) { cout << " init=\n"; indent++; n.init->accept(*this); indent--; }
	else cout << "\n";
}

void ASTPrinter::visit(FunctionDeclaration& n) {
	ind(); cout << "FunctionDeclaration ret=" << n.retType << " name=" << n.name << "\n";
	// printLoc(n);
	indent++;
	ind(); cout << "Params:\n"; indent++;
	for (auto& p : n.params) { ind(); cout << p.first << " " << p.second << "\n"; }
	indent--;
	ind(); cout << "Body:\n"; indent++;
	if (n.body) n.body->accept(*this);
	indent -= 2;
}

void ASTPrinter::visit(CompoundStatement& n) {
	ind(); cout << "CompoundStatement\n";
	// printLoc(n);
	indent++;
	for (auto& d : n.localDeclarations) d->accept(*this);
	for (auto& s : n.statements) s->accept(*this);
	indent--;
}

void ASTPrinter::visit(ReturnStatement& n) {
	ind(); cout << "Return\n";
	if (n.expr) { indent++; n.expr->accept(*this); indent--; }
}

void ASTPrinter::visit(ExpressionStatement& n) {
	ind(); cout << "ExpressionStatement\n";
	if (n.expr) { indent++; n.expr->accept(*this); indent--; }
}

void ASTPrinter::visit(NumberExpression& n) {
	ind(); cout << "Number " << n.value << "\n";
}

void ASTPrinter::visit(VariableExpression& n) {
	ind(); cout << "Variable " << n.name << "\n";
}

void ASTPrinter::visit(BinaryExpression& n) {
	ind(); cout << "Binary op=" << n.op << "\n";
	indent++; n.lhs->accept(*this); n.rhs->accept(*this); indent--;
}

void ASTPrinter::visit(AssignExpression& n) {
	ind(); cout << "Assign to " << n.name << "\n";
	indent++; n.value->accept(*this); indent--;
}

void ASTPrinter::visit(CallExpression& n) {
	ind(); cout << "Call " << n.callee << "\n";
	indent++;
	for (auto& a : n.args) a->accept(*this);
	indent--;
}
