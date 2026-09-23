#pragma once

#include "Lexer.h"
#include "Parser.h"

struct ASTPrinter : ASTVisitor
{
    int indent = 0;
    void ind() { for (int i = 0; i < indent; ++i) cout << "  "; }
    void printLoc(const Node& n) { cout << " [" << n.loc.startLine << ":" << n.loc.startColumn << "-" << n.loc.endLine << ":" << n.loc.endColumn << "]"; }

    void visit(Program& n) override {
        cout << "Program\n";
        indent++;
        for (auto& d : n.declarations) { d->accept(*this); }
        indent--;
    }
    void visit(VariableDeclaration& n) override {
        ind(); cout << "VariableDeclaration type=" << n.type << " name=" << n.name;
        printLoc(n);
        if (n.init) { cout << " init=\n"; indent++; n.init->accept(*this); indent--; }
        else cout << "\n";
    }
    void visit(FunctionDeclaration& n) override {
        ind(); cout << "FunctionDeclaration ret=" << n.retType << " name=" << n.name << "\n";
        printLoc(n);
        indent++;
        ind(); cout << "Params:\n"; indent++;
        for (auto& p : n.params) { ind(); cout << p.first << " " << p.second << "\n"; }
        indent--;
        ind(); cout << "Body:\n"; indent++;
        if (n.body) n.body->accept(*this);
        indent -= 2;
    }
    void visit(CompoundStatement& n) override {
        ind(); cout << "CompoundStatement\n";
        printLoc(n);
        indent++;
        for (auto& d : n.localDeclarations) d->accept(*this);
        for (auto& s : n.stmts) s->accept(*this);
        indent--;
    }
    void visit(ReturnStatement& n) override {
        ind(); cout << "Return\n";
        if (n.expr) { indent++; n.expr->accept(*this); indent--; }
    }
    void visit(ExpressionStatement& n) override {
        ind(); cout << "ExpressionStatement\n";
        if (n.expr) { indent++; n.expr->accept(*this); indent--; }
    }
    void visit(NumberExpression& n) override {
        ind(); cout << "Number " << n.value << "\n";
    }
    void visit(VarExpression& n) override {
        ind(); cout << "Variable " << n.name << "\n";
    }
    void visit(BinaryExpression& n) override {
        ind(); cout << "Binary op=" << n.op << "\n";
        indent++; n.lhs->accept(*this); n.rhs->accept(*this); indent--;
    }
    void visit(AssignExpression& n) override {
        ind(); cout << "Assign to " << n.name << "\n";
        indent++; n.value->accept(*this); indent--;
    }
    void visit(CallExpression& n) override {
        ind(); cout << "Call " << n.callee << "\n";
        indent++;
        for (auto& a : n.args) a->accept(*this);
        indent--;
    }
};
