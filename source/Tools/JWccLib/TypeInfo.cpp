#include "pch.h"
#include "TypeInfo.h"

using namespace WadeSpace;

TypeInfoPtr TypeInfo::makePrimitive(TokenType t)
{
	auto p = std::make_shared<TypeInfo>();
	p->kind = Kind::Primitive;
	p->primitive = t;
	return p;
}

TypeInfoPtr TypeInfo::makePointer(TypeInfoPtr elem)
{
	auto p = std::make_shared<TypeInfo>();
	p->kind = Kind::Pointer;
	p->element = elem;
	return p;
}

TypeInfoPtr TypeInfo::makeArray(TypeInfoPtr elem, size_t size)
{
	auto p = std::make_shared<TypeInfo>();
	p->kind = Kind::Array;
	p->element = elem;
	p->arraySize = size;
	return p;
}

TypeInfoPtr TypeInfo::makeStruct(const std::string& name)
{
	auto p = std::make_shared<TypeInfo>();
	p->kind = Kind::Struct;
	p->structName = name;
	return p;
}

TypeInfoPtr TypeInfo::makeFunction(TypeInfoPtr ret, const std::vector<TypeInfoPtr>& params)
{
	auto p = std::make_shared<TypeInfo>();
	p->kind = Kind::Function;
	p->returnType = ret;
	p->parameterTypes = params;
	return p;
}

bool TypeInfo::equals(const TypeInfo& other) const
{
	if (kind != other.kind) return false;
	switch (kind)
	{
	case Kind::Primitive:
		return primitive == other.primitive;
	case Kind::Pointer:
		return element && other.element && element->equals(*other.element);
	case Kind::Array:
		return element && other.element && element->equals(*other.element); // ignore size for compatibility
	case Kind::Struct:
		return structName == other.structName;
	case Kind::Function:
		if (!returnType || !other.returnType) return false;
		if (!returnType->equals(*other.returnType)) return false;
		if (parameterTypes.size() != other.parameterTypes.size()) return false;
		for (size_t i = 0; i < parameterTypes.size(); ++i)
		{
			if (!parameterTypes[i] || !other.parameterTypes[i]) return false;
			if (!parameterTypes[i]->equals(*other.parameterTypes[i])) return false;
		}
		return true;
	default:
		return false;
	}
}
