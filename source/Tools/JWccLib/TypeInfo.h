#pragma once

#include <memory>
#include <vector>
#include <string>
#include "TokenType.h"

namespace WadeSpace
{
	struct TypeInfo;

	using TypeInfoPtr = std::shared_ptr<TypeInfo>;

	struct TypeInfo
	{
		enum class Kind { Unknown, Primitive, Pointer, Array, Struct, Function } kind = Kind::Unknown;
		// primitive type
		TokenType primitive = TokenType::NONE;
		// pointer/array element
		TypeInfoPtr element;
		// array size (0 means unspecified)
		size_t arraySize = 0;
		// struct info
		std::string structName;
		// function signature
		TypeInfoPtr returnType;
		std::vector<TypeInfoPtr> parameterTypes;

		static TypeInfoPtr makePrimitive(TokenType t);
		static TypeInfoPtr makePointer(TypeInfoPtr elem);
		static TypeInfoPtr makeArray(TypeInfoPtr elem, size_t size = 0);
		static TypeInfoPtr makeStruct(const std::string& name);
		static TypeInfoPtr makeFunction(TypeInfoPtr ret, const std::vector<TypeInfoPtr>& params);

		bool equals(const TypeInfo& other) const;
	};
}
