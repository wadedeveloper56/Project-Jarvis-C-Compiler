#include "pch.h"
#include "SymbolTable.h"

using namespace WadeSpace;

SymbolTable::SymbolTable()
{
	// start with global scope
	pushScope();
}

void SymbolTable::pushScope()
{
	scopes.emplace_back();
}

void SymbolTable::popScope()
{
	if (!scopes.empty()) scopes.pop_back();
}

bool SymbolTable::declare(const SymbolInfo& info)
{
	if (scopes.empty()) pushScope();
	auto& cur = scopes.back();
	if (cur.find(info.name) != cur.end()) return false;
	cur.insert({ info.name, info });
	return true;
}

std::optional<SymbolInfo> SymbolTable::lookup(const std::string& name) const
{
	for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
	{
		auto found = it->find(name);
		if (found != it->end()) return found->second;
	}
	return std::nullopt;
}

bool SymbolTable::isDeclaredInCurrentScope(const std::string& name) const
{
	if (scopes.empty()) return false;
	auto& cur = scopes.back();
	return cur.find(name) != cur.end();
}
