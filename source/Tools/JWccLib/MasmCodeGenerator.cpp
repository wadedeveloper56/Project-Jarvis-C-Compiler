#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include "MasmCodeGenerator.h"
#include "GlobalVars.h"
#include "Initializer.h"
#include "ExpressionTree.h"
#include "Compiler.h"

using namespace WadeSpace;
using namespace std;

MasmCodeGenerator::MasmCodeGenerator(shared_ptr<vector<shared_ptr<VariableData>>> variableTable, shared_ptr<vector<shared_ptr<FunctionData>>> functionTable): BaseCodeGenerator(variableTable, functionTable)
{}

string MasmCodeGenerator::vectorToCommaSeparatedList(const vector<string>& vec)
{
	string result;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		result += vec[i];
		if (i < vec.size() - 1)
		{
			result += ", ";
		}
	}
	return result;
}

string MasmCodeGenerator::getAsmType(TokenType type, bool isPointer, bool isUnsigned)
{
	string asmType;
	if (isPointer)
	{
		if (bit16) asmType = "WORD ";
		else if (bit32) asmType = "DWORD ";
		else asmType = "QWORD ";
	}
	else if (!isUnsigned && type == CHAR) asmType = "SBYTE ";
	else if (!isUnsigned && type == BOOLT) asmType = "SBYTE ";
	else if (!isUnsigned && type == SHORTT) asmType = "SWORD ";
	else if (!isUnsigned && type == INT) asmType = "SDWORD ";
	else if (!isUnsigned && type == LONG) asmType = "SDWORD ";
	else if (!isUnsigned && type == FLOAT) asmType = "SDWORD ";
	else if (!isUnsigned && type == LONG_LONG) asmType = "SQWORD ";
	else if (!isUnsigned && type == DOUBLE) asmType = "SQWORD ";
	else if (!isUnsigned && type == LONG_DOUBLE) asmType = "TBYTE ";
	else if (isUnsigned && type == CHAR) asmType = "BYTE ";
	else if (isUnsigned && type == BOOLT) asmType = "BYTE ";
	else if (isUnsigned && type == SHORTT) asmType = "WORD ";
	else if (isUnsigned && type == INT) asmType = "DWORD ";
	else if (isUnsigned && type == LONG) asmType = "DWORD ";
	else if (isUnsigned && type == FLOAT) asmType = "DWORD ";
	else if (isUnsigned && type == LONG_LONG) asmType = "QWORD ";
	else if (isUnsigned && type == DOUBLE) asmType = "QWORD ";
	else if (isUnsigned && type == LONG_DOUBLE) asmType = "TBYTE ";
	return asmType;
}

void MasmCodeGenerator::handleFunctionWithParameters(ostream& out, string name, shared_ptr<vector<shared_ptr<VariableData>>> list)
{
	vector<string> paramList;
	for (shared_ptr<VariableData> ptr : *list)
	{
		optional<TokenType> type = ptr->type;
		auto variableName = "_" + ptr->name;
		bool isInitialized = ptr->initializer != nullptr;
		bool isPointer = ptr->pointer;
		bool isArray = ptr->arraySize > 1;
		bool isStruct = type == STRUCT || type == UNION;
		bool isUnsigned = ptr->unsign == true;

		string asmType = getAsmType(type.value(), isPointer, isUnsigned);
		paramList.push_back(variableName + ":" + asmType);
	}
	string paramListStr = vectorToCommaSeparatedList(paramList);
	out << "_" << name << " PROC C, " << paramListStr << endl;
}

static void jumpProcess(ostream& out, shared_ptr<TreeNodeData> left, shared_ptr<TreeNodeData> right, shared_ptr<TreeNodeData> current)
{
	if (current != nullptr && left == nullptr && right == nullptr)
	{
		if (current->getConstant() != nullptr)
		{
			if (current->getIConst() != nullptr)
			{
				if (bit16) out << "\t" << "mov ax," << current->getIConst()->getIntegerConst() << endl;
				else if (bit32) out << "\t" << "mov eax," << current->getIConst()->getIntegerConst() << endl;
				else if (bit64) out << "\t" << "mov rax," << current->getIConst()->getIntegerConst() << endl;
			}
		}
	}
}

void MasmCodeGenerator::handleIndividualFunctionStatements(ostream	& out, TokenType returnType, shared_ptr<BaseStatement> statements)
{
	for (shared_ptr<BaseStatement> node : *statements->getStatementList())
	{
		if (node->getOp() == expression_statement)
		{
			shared_ptr<BaseStatement> base_statement = node->getStatement();
			shared_ptr<ExpressionTree> exp = base_statement->getExp();
			NodeType nt = exp->getData()->getType();
			if (nt == NT_FUNCTION_CALL)
			{
				shared_ptr<vector<shared_ptr<ExpressionTree>>> parameters = exp->getData()->getArgumentList();
				string functionName = exp->getLeft()->getData()->getToken3()->getSymbolName();
				vector<string> list;
				if (parameters != nullptr)
				{
					for (shared_ptr<ExpressionTree> node2 : *parameters)
					{
						string str = to_string(node2->getData()->getConstant()->getIConst()->getIntegerConst());
						list.push_back(str);
					}
					out << "\t" << "invoke _" << functionName << ", " << vectorToCommaSeparatedList(list) << endl;
				}
				else
				{
					out << "\t" << "invoke _" << functionName << endl;
				}
			}
		}
		else if (node->getOp() == jump_statement)
		{
			shared_ptr<BaseStatement> base_statement = node->getStatement();
			TokenType op = base_statement->getOp().value();
			switch (op)
			{
				case RETURN:
					base_statement->getExp()->evaluate(out, jumpProcess);
					out << "\tret" << endl;
					break;
			}
		}
	}
}

void MasmCodeGenerator::handleIndividualFunction(ostream& out, shared_ptr<FunctionData> ptr)
{
	optional<TokenType> returnType = ptr->type;
	shared_ptr<vector<shared_ptr<VariableData>>> parameters = ptr->parameters;
	shared_ptr<BaseStatement> statements = ptr->statements;
	if (parameters != nullptr && !parameters->empty())
	{
		handleFunctionWithParameters(out, ptr->name, parameters);
	}
	else
	{
		out << "_" << ptr->name << " PROC C" << endl;
	}
	handleIndividualFunctionStatements(out, returnType.value(), statements);
	out << "_" << ptr->name << " endp" << endl;
}

string MasmCodeGenerator::convertToAsmType(bool isUnsigned, bool isPointer, TokenType type)
{
	string asmType = "";
	if (isPointer)
	{
		if (bit16) asmType = " WORD ";
		else if (bit32) asmType = " DWORD ";
		else asmType = " QWORD ";
	}
	else if (!isUnsigned && type == CHAR) asmType = " SBYTE ";
	else if (!isUnsigned && type == BOOLT) asmType = " SBYTE ";
	else if (!isUnsigned && type == SHORTT) asmType = " SWORD ";
	else if (!isUnsigned && type == INT) asmType = " SDWORD ";
	else if (!isUnsigned && type == LONG) asmType = " SDWORD ";
	else if (!isUnsigned && type == LONG_LONG) asmType = " SQWORD ";
	else if (!isUnsigned && type == FLOAT) asmType = " REAL4 ";
	else if (!isUnsigned && type == DOUBLE) asmType = " REAL8 ";
	else if (!isUnsigned && type == LONG_DOUBLE) asmType = " REAL10 ";
	else if (isUnsigned && type == CHAR) asmType = " BYTE ";
	else if (isUnsigned && type == BOOLT) asmType = " BYTE ";
	else if (isUnsigned && type == SHORTT) asmType = " WORD ";
	else if (isUnsigned && type == INT) asmType = " DWORD ";
	else if (isUnsigned && type == LONG) asmType = " DWORD ";
	else if (isUnsigned && type == LONG_LONG) asmType = " QWORD ";
	else if (isUnsigned && type == FLOAT) asmType = " REAL4 ";
	else if (isUnsigned && type == DOUBLE) asmType = " REAL8 ";
	else if (isUnsigned && type == LONG_DOUBLE) asmType = " REAL10 ";
	return asmType;
}

void MasmCodeGenerator::outputVariable(ostream& out, shared_ptr<VariableData> ptr)
{
	auto type = ptr->type;
	auto variableName = "_" + ptr->name;
	bool isInitialized = ptr->initializer != nullptr;
	bool isPointer = ptr->pointer;
	bool isArray = ptr->arraySize > 1;
	bool isStruct = type == STRUCT || type == UNION;
	bool isUnsigned = ptr->unsign == true;

	string asmType = convertToAsmType(isUnsigned, isPointer, type.value());

	if (!isInitialized)
	{
		if (!isPointer)
		{
			if (isStruct && !isArray)
			{
				string structName = ptr->structName;
				out << variableName << " " << structName << " <>" << endl;
			}
			else if (!isStruct && isArray)
			{
				out << variableName << asmType << ptr->arraySize << " dup(?)" << endl;
			}
			else if (isStruct && isArray)
			{
				shared_ptr<StructOrUnionSpecifier> suSpec = ptr->suSpec;
				auto structName = suSpec->getName()->getSymbolName();
				out << variableName << " " << structName << " " << ptr->arraySize << " dup(<>)" << endl;
			}
			else
			{
				out << variableName << asmType << " ?" << endl;
			}
		}
		else
		{
			out << variableName << asmType << " ?" << endl;
		}
	}
	else
	{
		auto constant = ptr->initializer->getAssignmentExpression()->getData()->getConstant();
		if (constant->getIConst() != nullptr)
		{
			long long int value = constant->getIConst()->getIntegerConst();
			out << variableName << asmType << value << endl;
		}
		if (constant->getFConst() != nullptr)
		{
			long double value = constant->getFConst()->getDoubleConst();
			out << variableName << asmType << (long double)value << endl;
		}
	}
}

void MasmCodeGenerator::handleInitializedVariable(ostream& out, shared_ptr<VariableData> ptr)
{
	if (ptr->initializer != nullptr && ptr->plist == nullptr)
	{
		outputVariable(out, ptr);
	}
}

void MasmCodeGenerator::handleUUninitializedVariable(ostream& out, shared_ptr<VariableData> ptr)
{
	if (ptr->initializer == nullptr && ptr->plist == nullptr)
	{
		outputVariable(out, ptr);
	}
}

void MasmCodeGenerator::handleVariableTable(ostream& out)
{
	out << ".data" << endl;
	for (shared_ptr<VariableData> ptr : *variableTable)
	{
		handleInitializedVariable(out, ptr);
	}
	out << ".data?" << endl;
	for (shared_ptr<VariableData> ptr : *variableTable)
	{
		handleUUninitializedVariable(out, ptr);
	}
}

void MasmCodeGenerator::handleFunctionTablePrototypes(ostream& out)
{
	for (shared_ptr<FunctionData> ptr : *functionTable)
	{
		optional<TokenType> returnType = ptr->type;
		shared_ptr<vector<shared_ptr<VariableData>>> parameters = ptr->parameters;
		string name = "_" + ptr->name;
		vector<string> paramList;
		if (parameters != nullptr)
		{
			for (shared_ptr<VariableData> ptr : *parameters)
			{
				optional<TokenType> type = ptr->type;
				string variableName = "_" + ptr->name;
				bool isInitialized = ptr->initializer != nullptr;
				bool isPointer = ptr->pointer;
				bool isArray = ptr->arraySize > 1;
				bool isStruct = type == STRUCT || type == UNION;
				bool isUnsigned = ptr->unsign == true;

				string asmType = getAsmType(type.value(), isPointer, isUnsigned);
				paramList.push_back(variableName + ":" + asmType);
			}
			string paramListStr = vectorToCommaSeparatedList(paramList);
			out << name << " PROTO C " << paramListStr << ";" << endl;
		}
		else
		{
			out << name << " PROTO C;" << endl;
		}
	}
}

void MasmCodeGenerator::handleFunctionTable(ostream& out)
{
	out << endl << ".code" << endl;
	handleFunctionTablePrototypes(out);
	for (shared_ptr<FunctionData> ptr : *functionTable)
	{
		out << endl;
		handleIndividualFunction(out, ptr);
	}
}

void MasmCodeGenerator::handleStructs(ostream& out)
{
	shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> temp = compiler->getStructList();
	for (map<string, shared_ptr<StructOrUnionSpecifier>>::iterator iterator = temp->begin(); iterator != temp->end(); ++iterator)
	{
		string key = iterator->first;
		shared_ptr<StructOrUnionSpecifier> value = iterator->second;
		shared_ptr<StructOrUnionSpecifier> suSpec = value;
		string name = suSpec->getName()->getSymbolName();
		shared_ptr<vector<shared_ptr<StructDeclaration>>> vars = suSpec->getVectorStructDeclaration();
		out << name << (value->getStructOrUnion()->getKeywordName()=="struct" ? " STRUCT" : " UNION") << endl;
		for (shared_ptr<StructDeclaration> var : *vars)
		{
			TokenType type = var->getSpecifierQualifierList()->getTypeSpecifier()->getType().value();
			shared_ptr<vector<shared_ptr<StructDeclarator>>> structDecl = var->getVectorStructDeclarator();
			for (shared_ptr<StructDeclarator> decl : *structDecl)
			{
				auto varName = decl->getDeclarator()->getDirectDeclarator()->getIdentifier()->getSymbolName();
				if (type == CHAR || type == BOOLT) out << "\t" << varName << " SBYTE ?" << endl;
				else if (type == SHORTT) out << "\t" << varName << " SWORD ?" << endl;
				else if (type == INT) out << "\t" << varName << " SDWORD ?" << endl;
				else if (type == LONG) out << "\t" << varName << " SDWORD ?" << endl;
				else if (type == LONG_LONG) out << "\t" << varName << " SQWORD ?" << endl;
				else if (type == FLOAT) out << "\t" << varName << " REAL4 ?" << endl;
				else if (type == DOUBLE) out << "\t" << varName << " REAL8 ?" << endl;
				else if (type == LONG_DOUBLE) out << "\t" << varName << " REAL10 ?" << endl;
			}
		}
		out << name << " ENDS" << endl;

	}
}

void MasmCodeGenerator::handlePrototype(ostream& out)
{
	for (shared_ptr<VariableData> ptr : *variableTable)
	{
		if (ptr->plist != nullptr)
		{
			vector<string> paramList;
			for (shared_ptr<ParameterDeclaration> node : *ptr->plist->getVectorParameterDeclaration())
			{
				shared_ptr<DeclarationSpecifiers> temp1 = node->getDeclarationSpecifiers();
				shared_ptr<Declarator> temp2 = node->getDeclarator();
				shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> temp3 = temp1->getDeclarationSpecifiersNodeList();

				auto name = temp2->getDirectDeclarator()->getIdentifier()->getSymbolName();
				bool isUnsigned = false;
				TokenType type = UNKNOWN;
				bool isPointer = temp2->hasPointer();

				for (shared_ptr<DeclarationSpecifiersNode> node2 : *temp3)
				{
					TokenType temp = node2->getTypeSpecifier()->getType().value();
					if (temp == CHAR || temp == BOOLT) type = temp;
					else if (temp == SHORTT) type = temp;
					else if (temp == INT) type = temp;
					else if (temp == LONG) type = temp;
					else if (temp == LONG_LONG) type = temp;
					else if (temp == FLOAT) type = temp;
					else if (temp == DOUBLE) type = temp;
					else if (temp == LONG_DOUBLE)  type = temp;
					else if (temp == UNSIGNED) isUnsigned = true;
					else if (temp == SIGNED) isUnsigned = false;
				}
				if (type != UNKNOWN)
				{
					string asmType = convertToAsmType(isUnsigned, isPointer, type);
					paramList.push_back(name + ":" + asmType);
				}
			}
			auto name = "_" + ptr->name;
			string paramListStr = vectorToCommaSeparatedList(paramList);
			//vector<string>::iterator it = find(compiler->getFunctionList()->begin(), compiler->getFunctionList()->end(), name);
			//if (it == compiler->getFunctionList()->end()) 
				out << "EXTERN " << name << " :PROTO " << paramListStr << ";" << endl;
		}
	}
}

void MasmCodeGenerator::generateCode(ostream& out)
{
	if (bit16)
	{
		out << ".286" << endl;
		out << ".model small;" << endl;
	}
	else if (bit32)
	{
		out << ".386" << endl;
		out << ".model flat, c;" << endl;
	}
	else
	{
		out << ".x64p" << endl;
		//out << ".model flat, c;" << endl;
	}
	out << "option casemap : none" << endl;
	out << endl;
	handlePrototype(out);
	out << endl;
	handleStructs(out);
	out << endl;
	handleVariableTable(out);
	handleFunctionTable(out);
	out << "end" << endl << endl;;
}

MasmCodeGenerator::MasmCodeGenerator(MasmCodeGenerator&& other) noexcept : BaseCodeGenerator(std::move(other))
{}

MasmCodeGenerator& MasmCodeGenerator::operator=(const MasmCodeGenerator& other)
{
	if (this == &other)
		return *this;
	BaseCodeGenerator::operator =(other);
	return *this;
}

MasmCodeGenerator& MasmCodeGenerator::operator=(MasmCodeGenerator&& other) noexcept
{
	if (this == &other)
		return *this;
	BaseCodeGenerator::operator =(std::move(other));
	return *this;
}