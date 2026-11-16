#include "pch.h"
#include "Token.h"

shared_ptr<CToken> createToken(shared_ptr<CTokData> data)
{
	auto tok = make_shared<CToken>();
	tok->data = data;
	return tok;
}
shared_ptr<CTokData> createTokData(void)
{
	auto data = make_shared<CTokData>();
	data->repr.symbol.string = nullptr;
	return data;
}

shared_ptr<CToken> createConstantULLToken(const unsigned long long num)
{
	auto tok = createToken(createTokData());
	tok->data->code = YC_NUMERIC;
	tok->data->repr.numericConstant.type = CONSTT_UINT_CONST;
	tok->data->repr.numericConstant.radix = RADT_DECIMAL;
	tok->data->repr.numericConstant.repr.lIntConst = num;
	return tok;
}

shared_ptr<CToken> createConstantLDToken(const long double num)
{
	auto tok = createToken(createTokData());
	tok->data->code = YC_NUMERIC;
	tok->data->repr.numericConstant.type = CONSTT_LDOUBLE_CONST;
	tok->data->repr.numericConstant.radix = RADT_DECIMAL;
	tok->data->repr.numericConstant.repr.lDoubleConst = num;
	return tok;
}

shared_ptr<CToken> createStringConstantToken(char* str)
{
	auto tok = createToken(createTokData());
	tok->data->code = YC_STRING;
	tok->data->repr.stringConstant.s = new char[strlen(str) + 1];
	strcpy(tok->data->repr.stringConstant.s, str);
	tok->data->repr.stringConstant.strLen = (int)strlen(str);
	return tok;
}

shared_ptr<CToken> createStringIDToken(char* str)
{
	auto tok = createToken(createTokData());
	tok->data->code = YC_SYMBOL;
	tok->data->repr.symbol.string = new char[strlen(str) + 1];
	strcpy(tok->data->repr.symbol.string, str);
	tok->data->repr.symbol.strLen = (int)strlen(str);
	return tok;
}

shared_ptr<CToken> createKeywordToken(char* str, int keyword)
{
	auto tok = createToken(createTokData());
	tok->data->code = YC_KEYWORD;
	tok->data->repr.keyword.string = new char[strlen(str) + 1];
	strcpy(tok->data->repr.keyword.string, str);
	tok->data->repr.keyword.keyword = keyword;
	tok->data->repr.keyword.strLen = (int)strlen(str);
	return tok;
}

