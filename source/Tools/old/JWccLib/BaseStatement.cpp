#include "pch.h"
#include "BaseStatement.h"
#include "ExpressionTree.h"

using namespace WadeSpace;
using namespace std;

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<CToken> identifier, shared_ptr<ExpressionTree> const exp, shared_ptr<ExpressionTree> const contExp,
                             shared_ptr<BaseStatement> const statement, shared_ptr<BaseStatement> const es1, shared_ptr<BaseStatement> const es2,
                             shared_ptr<BaseStatement> const statement2, vector<shared_ptr<BaseStatement>>* const statementList,
                             shared_ptr<vector<shared_ptr<Declaration>>> const declarationList)
	: op(op),
	  identifier(identifier),
	  exp(exp),
	  contExp(contExp),
	  statement(statement),
	  es1(es1),
	  es2(es2),
	  statement2(statement2),
	  statementList(statementList),
	  declarationList(declarationList)
{
}

BaseStatement::BaseStatement(shared_ptr<CToken> identifier, shared_ptr<BaseStatement> statement) : op(NONE), identifier(identifier),
                                                                              contExp(nullptr), exp(nullptr),
                                                                              statement(statement), es1(nullptr),
                                                                              es2(nullptr), statement2(nullptr),
                                                                              statementList(nullptr),
                                                                              declarationList(nullptr)
{
}

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement) : op(op), identifier(nullptr),
	contExp(exp), exp(nullptr), statement(statement), es1(nullptr), es2(nullptr), statement2(nullptr),
	statementList(nullptr), declarationList(nullptr)
{
}

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<BaseStatement> statement) : op(op), identifier(nullptr), contExp(nullptr),
                                                                       exp(nullptr), statement(statement), es1(nullptr),
                                                                       es2(nullptr), statement2(nullptr),
                                                                       statementList(nullptr), declarationList(nullptr)
{
}

BaseStatement::BaseStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> statementList) : op(NONE), identifier(nullptr), contExp(nullptr),
                                                                      exp(nullptr), statement(nullptr), es1(nullptr),
                                                                      es2(nullptr), statement2(nullptr),
                                                                      statementList(statementList),
                                                                      declarationList(nullptr)
{
}

BaseStatement::BaseStatement(shared_ptr<vector<shared_ptr<Declaration>>> declarationList) : op(NONE), identifier(nullptr), contExp(nullptr),
                                                                      exp(nullptr), statement(nullptr), es1(nullptr),
                                                                      es2(nullptr), statement2(nullptr),
                                                                      statementList(nullptr),
                                                                      declarationList(declarationList)
{
}

BaseStatement::BaseStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> statementList, shared_ptr<vector<shared_ptr<Declaration>>> declarationList) :
	op(NONE), identifier(nullptr), contExp(nullptr), exp(nullptr), statement(nullptr), es1(nullptr), es2(nullptr),
	statement2(nullptr), statementList(statementList), declarationList(declarationList)
{
}

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<ExpressionTree> exp,
                             shared_ptr<BaseStatement> statement) : op(op), identifier(nullptr), contExp(nullptr), exp(exp),
                                                         statement(statement), es1(es1), es2(es2), statement2(nullptr),
                                                         statementList(nullptr), declarationList(nullptr)
{
}

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<BaseStatement> statement) :
	op(op), identifier(nullptr), contExp(nullptr), exp(nullptr), statement(statement), es1(nullptr), es2(nullptr),
	statement2(nullptr), statementList(nullptr), declarationList(nullptr)
{
}

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> ifStatement, shared_ptr<BaseStatement> elseStatement) :
	op(op), identifier(nullptr), contExp(nullptr), exp(exp), statement(ifStatement), es1(nullptr), es2(nullptr),
	statement2(elseStatement), statementList(nullptr), declarationList(nullptr)
{
}

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<ExpressionTree> exp) : op(op), identifier(nullptr), contExp(nullptr), exp(exp),
                                                              statement(nullptr), es1(nullptr), es2(nullptr),
                                                              statement2(nullptr), statementList(nullptr),
                                                              declarationList(nullptr)
{
}

BaseStatement::BaseStatement(optional<TokenType> op) : op(op), identifier(nullptr), contExp(nullptr), exp(nullptr),
                                             statement(nullptr),
                                             es1(nullptr), es2(nullptr), statement2(nullptr), statementList(nullptr),
                                             declarationList(nullptr)
{
}

BaseStatement::BaseStatement(optional<TokenType> op, shared_ptr<CToken> identifier) : op(op), identifier(identifier), contExp(nullptr),
                                                                  exp(nullptr),
                                                                  statement(nullptr), es1(nullptr), es2(nullptr),
                                                                  statement2(nullptr), statementList(nullptr),
                                                                  declarationList(nullptr)
{
}

