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
    void visit(VarDecl& n) override {
        ind(); cout << "VarDecl type=" << n.type << " name=" << n.name;
        printLoc(n);
        if (n.init) { cout << " init=\n"; indent++; n.init->accept(*this); indent--; }
        else cout << "\n";
    }
    void visit(FunctionDecl& n) override {
        ind(); cout << "FunctionDecl ret=" << n.retType << " name=" << n.name << "\n";
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
    void visit(ExprStatement& n) override {
        ind(); cout << "ExprStatement\n";
        if (n.expr) { indent++; n.expr->accept(*this); indent--; }
    }
    void visit(NumberExpr& n) override {
        ind(); cout << "Number " << n.value << "\n";
    }
    void visit(VarExpr& n) override {
        ind(); cout << "Variable " << n.name << "\n";
    }
    void visit(BinaryExpr& n) override {
        ind(); cout << "Binary op=" << n.op << "\n";
        indent++; n.lhs->accept(*this); n.rhs->accept(*this); indent--;
    }
    void visit(AssignExpr& n) override {
        ind(); cout << "Assign to " << n.name << "\n";
        indent++; n.value->accept(*this); indent--;
    }
    void visit(CallExpr& n) override {
        ind(); cout << "Call " << n.callee << "\n";
        indent++;
        for (auto& a : n.args) a->accept(*this);
        indent--;
    }
};
