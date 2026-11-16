#pragma once

namespace WadeSpace
{
	enum NodeType
	{
		NT_NONE,
		NT_ARRAY,
		NT_FUNCTION_CALL,
		NT_VAR_ACCESS,
		NT_INC,
		NT_DEC,
		NT_TYPECAST,
		NT_SIZEOF,
		NT_UNARY,
		NT_OP,
		NT_QUESTION
	};
}