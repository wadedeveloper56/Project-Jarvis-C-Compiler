#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

void argRexResetFn(void* parent_) {
	ArgRexPtr parent = (ArgRexPtr)parent_;
	parent->count = 0;
}

int argRexScanFn(void* parent_, const char* argval) {
	ArgRexPtr parent = (ArgRexPtr)parent_;
	int errorcode = 0;
	const TRexChar* error = NULL;
	TRex* rex = NULL;
	TRexBool is_match = TRex_False;

	if (parent->count == parent->hdr.maxcount) {
		errorcode = ARG_ERR_MAXCOUNT;
	}
	else if (!argval) {
		parent->count++;
	}
	else {
		PrivHdrPtr priv = (PrivHdrPtr)parent->hdr.priv;

		rex = trex_compile(priv->pattern, &error, priv->flags);
		is_match = trex_match(rex, argval);
		if (!is_match)
			errorcode = ARG_ERR_REGNOMATCH;
		else
			parent->sval[parent->count++] = argval;

		trex_free(rex);
	}

	return errorcode;
}

int argRexCheckFn(void* parent_) {
	ArgRexPtr parent = (ArgRexPtr)parent_;
	int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;
	return errorcode;
}

void argRexErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname) {
	ArgRexPtr parent = (ArgRexPtr)parent_;
	const char* shortopts = parent->hdr.shortopts;
	const char* longopts = parent->hdr.longopts;
	const char* datatype = parent->hdr.datatype;

	argval = argval ? argval : "";

	argDstrCatF(ds, "%s: ", progname);
	switch (errorcode) {
		case ARG_ERR_MINCOUNT:
			argDstrCat(ds, "missing option ");
			argPrintOptionDs(ds, shortopts, longopts, datatype, "\n");
			break;

		case ARG_ERR_MAXCOUNT:
			argDstrCat(ds, "excess option ");
			argPrintOptionDs(ds, shortopts, longopts, argval, "\n");
			break;

		case ARG_ERR_REGNOMATCH:
			argDstrCat(ds, "illegal value  ");
			argPrintOptionDs(ds, shortopts, longopts, argval, "\n");
			break;

		default:
			break;
	}
}

ArgRexPtr argRex0(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary) {
	return argRexN(shortopts, longopts, pattern, datatype, 0, 1, flags, glossary);
}

ArgRexPtr argRex1(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary) {
	return argRexN(shortopts, longopts, pattern, datatype, 1, 1, flags, glossary);
}

ArgRexPtr argRexN(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int mincount, int maxcount, int flags, const char* glossary) {
	size_t nbytes;
	ArgRexPtr result;
	PrivHdrPtr priv;
	int i;
	const TRexChar* error = NULL;
	TRex* rex = NULL;

	if (!pattern) {
		printf("argtable: ERROR - illegal regular expression pattern \"(nullptr)\"\n");
		printf("argtable: Bad argument table.\n");
		return NULL;
	}

	maxcount = (maxcount < mincount) ? mincount : maxcount;

	nbytes = sizeof(ArgRex) + sizeof(PrivHdr) + (size_t)maxcount * sizeof(char*);

	result = (ArgRexPtr)malloc(nbytes);
	if (result) {
		result->hdr.flag = ARG_HASVALUE;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts = longopts;
		result->hdr.datatype = datatype ? datatype : pattern;
		result->hdr.glossary = glossary;
		result->hdr.mincount = mincount;
		result->hdr.maxcount = maxcount;
		result->hdr.parent = result;
		result->hdr.resetfn = argRexResetFn;
		result->hdr.scanfn = argRexScanFn;
		result->hdr.checkfn = argRexCheckFn;
		result->hdr.errorfn = argRexErrorFn;

		result->hdr.priv = result + 1;
		priv = (PrivHdrPtr)(result->hdr.priv);
		if (priv) {
			priv->pattern = pattern;
			priv->flags = flags;

			result->sval = (const char**)(priv + 1);
			result->count = 0;
			for (i = 0; i < maxcount; i++)
				result->sval[i] = "";

			rex = trex_compile(priv->pattern, &error, priv->flags);
			trex_free(rex);
		}
	}
	return result;
}

int trex_newnode(TRex* exp, TRexNodeType type) {
	TRexNode n;
	int newid;
	n.type = type;
	n.next = n.right = n.left = -1;
	if (type == OP_EXPR)
		n.right = exp->_nsubexpr++;
	if (exp->_nallocated < (exp->_nsize + 1)) {
		exp->_nallocated *= 2;
		exp->_nodes = (TRexNode*)realloc(exp->_nodes, (size_t)exp->_nallocated * sizeof(TRexNode));
	}
	exp->_nodes[exp->_nsize++] = n;
	newid = exp->_nsize - 1;
	return (int)newid;
}

void trex_error(TRex* exp, const TRexChar* error) {
	if (exp->_error)
		*exp->_error = error;
	longjmp(*((jmp_buf*)exp->_jmpbuf), -1);
}

void trex_expect(TRex* exp, int n) {
	if ((*exp->_p) != n)
		trex_error(exp, "expected paren");
	exp->_p++;
}

TRexChar trex_escapechar(TRex* exp) {
	if (*exp->_p == TREX_SYMBOL_ESCAPE_CHAR) {
		exp->_p++;
		switch (*exp->_p) {
			case 'v':
				exp->_p++;
				return '\v';
			case 'n':
				exp->_p++;
				return '\n';
			case 't':
				exp->_p++;
				return '\t';
			case 'r':
				exp->_p++;
				return '\r';
			case 'f':
				exp->_p++;
				return '\f';
			default:
				return (*exp->_p++);
		}
	}
	else if (!isprint((int)(*exp->_p)))
		trex_error(exp, "letter expected");
	return (*exp->_p++);
}

int trex_charclass(TRex* exp, int classid) {
	int n = trex_newnode(exp, OP_CCLASS);
	exp->_nodes[n].left = classid;
	return n;
}

int trex_charnode(TRex* exp, TRexBool isclass) {
	TRexChar t;
	if (*exp->_p == TREX_SYMBOL_ESCAPE_CHAR) {
		exp->_p++;
		switch (*exp->_p) {
			case 'n':
				exp->_p++;
				return trex_newnode(exp, '\n');
			case 't':
				exp->_p++;
				return trex_newnode(exp, '\t');
			case 'r':
				exp->_p++;
				return trex_newnode(exp, '\r');
			case 'f':
				exp->_p++;
				return trex_newnode(exp, '\f');
			case 'v':
				exp->_p++;
				return trex_newnode(exp, '\v');
			case 'a':
			case 'A':
			case 'w':
			case 'W':
			case 's':
			case 'S':
			case 'd':
			case 'D':
			case 'x':
			case 'X':
			case 'c':
			case 'C':
			case 'p':
			case 'P':
			case 'l':
			case 'u': {
				t = *exp->_p;
				exp->_p++;
				return trex_charclass(exp, t);
			}
			case 'b':
			case 'B':
				if (!isclass) {
					int node = trex_newnode(exp, OP_WB);
					exp->_nodes[node].left = *exp->_p;
					exp->_p++;
					return node;
				}
			default:
				t = *exp->_p;
				exp->_p++;
				return trex_newnode(exp, t);
		}
	}
	else if (!isprint((int)(*exp->_p))) {
		trex_error(exp, "letter expected");
	}
	t = *exp->_p;
	exp->_p++;
	return trex_newnode(exp, t);
}

int trex_class(TRex* exp) {
	int ret = -1;
	int first = -1, chain;
	if (*exp->_p == TREX_SYMBOL_BEGINNING_OF_STRING) {
		ret = trex_newnode(exp, OP_NCLASS);
		exp->_p++;
	}
	else
		ret = trex_newnode(exp, OP_CLASS);

	if (*exp->_p == ']')
		trex_error(exp, "empty class");
	chain = ret;
	while (*exp->_p != ']' && exp->_p != exp->_eol) {
		if (*exp->_p == '-' && first != -1) {
			int r, t;
			if (*exp->_p++ == ']')
				trex_error(exp, "unfinished range");
			r = trex_newnode(exp, OP_RANGE);
			if (first > *exp->_p)
				trex_error(exp, "invalid range");
			if (exp->_nodes[first].type == OP_CCLASS)
				trex_error(exp, "cannot use character classes in ranges");
			exp->_nodes[r].left = exp->_nodes[first].type;
			t = trex_escapechar(exp);
			exp->_nodes[r].right = t;
			exp->_nodes[chain].next = r;
			chain = r;
			first = -1;
		}
		else {
			if (first != -1) {
				int c = first;
				exp->_nodes[chain].next = c;
				chain = c;
				first = trex_charnode(exp, TRex_True);
			}
			else {
				first = trex_charnode(exp, TRex_True);
			}
		}
	}

	if (first != -1) {
		int c = first;
		exp->_nodes[chain].next = c;
	}

	exp->_nodes[ret].left = exp->_nodes[ret].next;
	exp->_nodes[ret].next = -1;
	return ret;
}

int trex_parsenumber(TRex* exp) {
	int ret = *exp->_p - '0';
	int positions = 10;
	exp->_p++;
	while (isdigit((int)(*exp->_p))) {
		ret = ret * 10 + (*exp->_p++ - '0');
		if (positions == 1000000000)
			trex_error(exp, "overflow in numeric constant");
		positions *= 10;
	};
	return ret;
}

int trex_element(TRex* exp) {
	int ret = -1;
	switch (*exp->_p) {
		case '(': {
			int expr, newn;
			exp->_p++;

			if (*exp->_p == '?') {
				exp->_p++;
				trex_expect(exp, ':');
				expr = trex_newnode(exp, OP_NOCAPEXPR);
			}
			else
				expr = trex_newnode(exp, OP_EXPR);
			newn = trex_list(exp);
			exp->_nodes[expr].left = newn;
			ret = expr;
			trex_expect(exp, ')');
		} break;
		case '[':
			exp->_p++;
			ret = trex_class(exp);
			trex_expect(exp, ']');
			break;
		case TREX_SYMBOL_END_OF_STRING:
			exp->_p++;
			ret = trex_newnode(exp, OP_EOL);
			break;
		case TREX_SYMBOL_ANY_CHAR:
			exp->_p++;
			ret = trex_newnode(exp, OP_DOT);
			break;
		default:
			ret = trex_charnode(exp, TRex_False);
			break;
	}

	{
		TRexBool isgreedy = TRex_False;
		unsigned short p0 = 0, p1 = 0;
		switch (*exp->_p) {
			case TREX_SYMBOL_GREEDY_ZERO_OR_MORE:
				p0 = 0;
				p1 = 0xFFFF;
				exp->_p++;
				isgreedy = TRex_True;
				break;
			case TREX_SYMBOL_GREEDY_ONE_OR_MORE:
				p0 = 1;
				p1 = 0xFFFF;
				exp->_p++;
				isgreedy = TRex_True;
				break;
			case TREX_SYMBOL_GREEDY_ZERO_OR_ONE:
				p0 = 0;
				p1 = 1;
				exp->_p++;
				isgreedy = TRex_True;
				break;
			case '{':
				exp->_p++;
				if (!isdigit((int)(*exp->_p)))
					trex_error(exp, "number expected");
				p0 = (unsigned short)trex_parsenumber(exp);
				switch (*exp->_p) {
					case '}':
						p1 = p0;
						exp->_p++;
						break;
					case ',':
						exp->_p++;
						p1 = 0xFFFF;
						if (isdigit((int)(*exp->_p))) {
							p1 = (unsigned short)trex_parsenumber(exp);
						}
						trex_expect(exp, '}');
						break;
					default:
						trex_error(exp, ", or } expected");
				}
				isgreedy = TRex_True;
				break;
		}
		if (isgreedy) {
			int nnode = trex_newnode(exp, OP_GREEDY);
			exp->_nodes[nnode].left = ret;
			exp->_nodes[nnode].right = ((p0) << 16) | p1;
			ret = nnode;
		}
	}
	if ((*exp->_p != TREX_SYMBOL_BRANCH) && (*exp->_p != ')') && (*exp->_p != TREX_SYMBOL_GREEDY_ZERO_OR_MORE) &&
		(*exp->_p != TREX_SYMBOL_GREEDY_ONE_OR_MORE) && (*exp->_p != '\0')) {
		int nnode = trex_element(exp);
		exp->_nodes[ret].next = nnode;
	}

	return ret;
}

int trex_list(TRex* exp) {
	int ret = -1, e;
	if (*exp->_p == TREX_SYMBOL_BEGINNING_OF_STRING) {
		exp->_p++;
		ret = trex_newnode(exp, OP_BOL);
	}
	e = trex_element(exp);
	if (ret != -1) {
		exp->_nodes[ret].next = e;
	}
	else
		ret = e;

	if (*exp->_p == TREX_SYMBOL_BRANCH) {
		int temp, tright;
		exp->_p++;
		temp = trex_newnode(exp, OP_OR);
		exp->_nodes[temp].left = ret;
		tright = trex_list(exp);
		exp->_nodes[temp].right = tright;
		ret = temp;
	}
	return ret;
}

TRexBool trex_matchcclass(int cclass, TRexChar c) {
	switch (cclass) {
		case 'a':
			return isalpha(c) ? TRex_True : TRex_False;
		case 'A':
			return !isalpha(c) ? TRex_True : TRex_False;
		case 'w':
			return (isalnum(c) || c == '_') ? TRex_True : TRex_False;
		case 'W':
			return (!isalnum(c) && c != '_') ? TRex_True : TRex_False;
		case 's':
			return isspace(c) ? TRex_True : TRex_False;
		case 'S':
			return !isspace(c) ? TRex_True : TRex_False;
		case 'd':
			return isdigit(c) ? TRex_True : TRex_False;
		case 'D':
			return !isdigit(c) ? TRex_True : TRex_False;
		case 'x':
			return isxdigit(c) ? TRex_True : TRex_False;
		case 'X':
			return !isxdigit(c) ? TRex_True : TRex_False;
		case 'c':
			return iscntrl(c) ? TRex_True : TRex_False;
		case 'C':
			return !iscntrl(c) ? TRex_True : TRex_False;
		case 'p':
			return ispunct(c) ? TRex_True : TRex_False;
		case 'P':
			return !ispunct(c) ? TRex_True : TRex_False;
		case 'l':
			return islower(c) ? TRex_True : TRex_False;
		case 'u':
			return isupper(c) ? TRex_True : TRex_False;
	}
	return TRex_False;
}

TRexBool trex_matchclass(TRex* exp, TRexNode* node, TRexChar c) {
	do {
		switch (node->type) {
			case OP_RANGE:
				if (exp->_flags & TREX_ICASE) {
					if (c >= toupper(node->left) && c <= toupper(node->right))
						return TRex_True;
					if (c >= tolower(node->left) && c <= tolower(node->right))
						return TRex_True;
				}
				else {
					if (c >= node->left && c <= node->right)
						return TRex_True;
				}
				break;
			case OP_CCLASS:
				if (trex_matchcclass(node->left, c))
					return TRex_True;
				break;
			default:
				if (exp->_flags & TREX_ICASE) {
					if (c == tolower(node->type) || c == toupper(node->type))
						return TRex_True;
				}
				else {
					if (c == node->type)
						return TRex_True;
				}
		}
	} while ((node->next != -1) && ((node = &exp->_nodes[node->next]) != NULL));
	return TRex_False;
}

const TRexChar* trex_matchnode(TRex* exp, TRexNode* node, const TRexChar* str, TRexNode* next) {
	TRexNodeType type = node->type;
	switch (type) {
		case OP_GREEDY: {
			TRexNode* greedystop = NULL;
			int p0 = (node->right * 16) & 0x0000FFFF, p1 = node->right & 0x0000FFFF, nmaches = 0;
			const TRexChar* s = str, * good = str;

			if (node->next != -1) {
				greedystop = &exp->_nodes[node->next];
			}
			else {
				greedystop = next;
			}

			while ((nmaches == 0xFFFF || nmaches < p1)) {
				const TRexChar* stop;
				if ((s = trex_matchnode(exp, &exp->_nodes[node->left], s, greedystop)) == NULL)
					break;
				nmaches++;
				good = s;
				if (greedystop) {
					if (greedystop->type != OP_GREEDY || (greedystop->type == OP_GREEDY && ((greedystop->right * 16) & 0x0000FFFF) != 0)) {
						TRexNode* gnext = NULL;
						if (greedystop->next != -1) {
							gnext = &exp->_nodes[greedystop->next];
						}
						else if (next && next->next != -1) {
							gnext = &exp->_nodes[next->next];
						}
						stop = trex_matchnode(exp, greedystop, s, gnext);
						if (stop) {
							if (p0 == p1 && p0 == nmaches)
								break;
							else if (nmaches >= p0 && p1 == 0xFFFF)
								break;
							else if (nmaches >= p0 && nmaches <= p1)
								break;
						}
					}
				}

				if (s >= exp->_eol)
					break;
			}
			if (p0 == p1 && p0 == nmaches)
				return good;
			else if (nmaches >= p0 && p1 == 0xFFFF)
				return good;
			else if (nmaches >= p0 && nmaches <= p1)
				return good;
			return NULL;
		}
		case OP_OR: {
			const TRexChar* asd = str;
			TRexNode* temp = &exp->_nodes[node->left];
			while ((asd = trex_matchnode(exp, temp, asd, NULL)) != NULL) {
				if (temp->next != -1)
					temp = &exp->_nodes[temp->next];
				else
					return asd;
			}
			asd = str;
			temp = &exp->_nodes[node->right];
			while ((asd = trex_matchnode(exp, temp, asd, NULL)) != NULL) {
				if (temp->next != -1)
					temp = &exp->_nodes[temp->next];
				else
					return asd;
			}
			return NULL;
			break;
		}
		case OP_EXPR:
		case OP_NOCAPEXPR: {
			TRexNode* n = &exp->_nodes[node->left];
			const TRexChar* cur = str;
			int capture = -1;
			if (node->type != OP_NOCAPEXPR && node->right == exp->_currsubexp) {
				capture = exp->_currsubexp;
				exp->_matches[capture].begin = cur;
				exp->_currsubexp++;
			}

			do {
				TRexNode* subnext = NULL;
				if (n->next != -1) {
					subnext = &exp->_nodes[n->next];
				}
				else {
					subnext = next;
				}
				if ((cur = trex_matchnode(exp, n, cur, subnext)) == NULL) {
					if (capture != -1) {
						exp->_matches[capture].begin = 0;
						exp->_matches[capture].len = 0;
					}
					return NULL;
				}
			} while ((n->next != -1) && ((n = &exp->_nodes[n->next]) != NULL));

			if (capture != -1)
				exp->_matches[capture].len = (int)(cur - exp->_matches[capture].begin);
			return cur;
		}
		case OP_WB:
			if ((str == exp->_bol && !isspace((int)(*str))) || (str == exp->_eol && !isspace((int)(*(str - 1)))) || (!isspace((int)(*str)) && isspace((int)(*(str + 1)))) ||
				(isspace((int)(*str)) && !isspace((int)(*(str + 1))))) {
				return (node->left == 'b') ? str : NULL;
			}
			return (node->left == 'b') ? NULL : str;
		case OP_BOL:
			if (str == exp->_bol)
				return str;
			return NULL;
		case OP_EOL:
			if (str == exp->_eol)
				return str;
			return NULL;
		case OP_DOT: {
			str++;
		}
				   return str;
		case OP_NCLASS:
		case OP_CLASS:
			if (trex_matchclass(exp, &exp->_nodes[node->left], *str) ? (type == OP_CLASS ? TRex_True : TRex_False)
				: (type == OP_NCLASS ? TRex_True : TRex_False)) {
				str++;
				return str;
			}
			return NULL;
		case OP_CCLASS:
			if (trex_matchcclass(node->left, *str)) {
				str++;
				return str;
			}
			return NULL;
		default:
			if (exp->_flags & TREX_ICASE) {
				if (*str != tolower(node->type) && *str != toupper(node->type))
					return NULL;
			}
			else {
				if (*str != node->type)
					return NULL;
			}
			str++;
			return str;
	}
}

TRex* trex_compile(const TRexChar* pattern, const TRexChar** error, int flags) {
	TRex* exp = (TRex*)malloc(sizeof(TRex));
	exp->_eol = exp->_bol = NULL;
	exp->_p = pattern;
	exp->_nallocated = (int)(strlen(pattern) * sizeof(TRexChar));
	exp->_nodes = (TRexNode*)malloc((size_t)exp->_nallocated * sizeof(TRexNode));
	exp->_nsize = 0;
	exp->_matches = 0;
	exp->_nsubexpr = 0;
	exp->_first = trex_newnode(exp, OP_EXPR);
	exp->_error = error;
	exp->_jmpbuf = malloc(sizeof(jmp_buf));
	exp->_flags = flags;
	if (setjmp(*((jmp_buf*)exp->_jmpbuf)) == 0) {
		int res = trex_list(exp);
		exp->_nodes[exp->_first].left = res;
		if (*exp->_p != '\0')
			trex_error(exp, "unexpected character");
		exp->_matches = (TRexMatch*)malloc((size_t)exp->_nsubexpr * sizeof(TRexMatch));
		memset(exp->_matches, 0, (size_t)exp->_nsubexpr * sizeof(TRexMatch));
	}
	else {
		trex_free(exp);
		return NULL;
	}
	return exp;
}

void trex_free(TRex* exp) {
	if (exp) {
		free(exp->_nodes);
		free(exp->_jmpbuf);
		free(exp->_matches);
		free(exp);
	}
}

TRexBool trex_match(TRex* exp, const TRexChar* text) {
	const TRexChar* res = NULL;
	exp->_bol = text;
	exp->_eol = text + strlen(text);
	exp->_currsubexp = 0;
	res = trex_matchnode(exp, exp->_nodes, text, NULL);
	if (res == NULL || res != exp->_eol)
		return TRex_False;
	return TRex_True;
}

TRexBool trex_searchrange(TRex* exp, const TRexChar* text_begin, const TRexChar* text_end, const TRexChar** out_begin, const TRexChar** out_end) {
	const TRexChar* cur = NULL;
	int node = exp->_first;
	if (text_begin >= text_end)
		return TRex_False;
	exp->_bol = text_begin;
	exp->_eol = text_end;
	do {
		cur = text_begin;
		while (node != -1) {
			exp->_currsubexp = 0;
			cur = trex_matchnode(exp, &exp->_nodes[node], cur, NULL);
			if (!cur)
				break;
			node = exp->_nodes[node].next;
		}
		text_begin++;
	} while (cur == NULL && text_begin != text_end);

	if (cur == NULL)
		return TRex_False;

	--text_begin;

	if (out_begin)
		*out_begin = text_begin;
	if (out_end)
		*out_end = cur;
	return TRex_True;
}

TRexBool trex_search(TRex* exp, const TRexChar* text, const TRexChar** out_begin, const TRexChar** out_end) {
	return trex_searchrange(exp, text, text + strlen(text), out_begin, out_end);
}

int trex_getsubexpcount(TRex* exp) {
	return exp->_nsubexpr;
}

TRexBool trex_getsubexp(TRex* exp, int n, TRexMatch* subexp) {
	if (n < 0 || n >= exp->_nsubexpr)
		return TRex_False;
	*subexp = exp->_matches[n];
	return TRex_True;
}
