// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.
#line 67 "ansic.y"

    #include "pch.h"
    #include "scanner.h"
    #include "parser.hpp"
    #include "interpreter.h"
    #include "location.hh"
    
    // yylex() arguments are defined in parser.y
    static WadeSpace::Parser::symbol_type yylex(WadeSpace::Scanner &scanner, WadeSpace::Interpreter &driver) {
        return scanner.get_next_token();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace WadeSpace;

#line 58 "parser.cpp"




#include "parser.hpp"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 9 "ansic.y"
namespace  WadeSpace  {
#line 158 "parser.cpp"

  /// Build a parser object.
   Parser :: Parser  (WadeSpace::Scanner &scanner_yyarg, WadeSpace::Interpreter &driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

   Parser ::~ Parser  ()
  {}

   Parser ::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
   Parser ::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

   Parser ::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
   Parser ::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
   Parser ::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

   Parser ::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

   Parser ::symbol_kind_type
   Parser ::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

   Parser ::stack_symbol_type::stack_symbol_type ()
  {}

   Parser ::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_abstract_declarator: // abstract_declarator
        value.YY_MOVE_OR_COPY< shared_ptr<AbstractDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
      case symbol_kind::S_compound_statement: // compound_statement
      case symbol_kind::S_expression_statement: // expression_statement
      case symbol_kind::S_selection_statement: // selection_statement
      case symbol_kind::S_iteration_statement: // iteration_statement
      case symbol_kind::S_jump_statement: // jump_statement
        value.YY_MOVE_OR_COPY< shared_ptr<BaseStatement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_I_CONST: // "i_const"
      case symbol_kind::S_F_CONST: // "f_const"
      case symbol_kind::S_STRING_LITERAL: // "sting_literal"
      case symbol_kind::S_TYPE_NAME: // "type name"
      case symbol_kind::S_AUTO: // "auto"
      case symbol_kind::S_BREAK: // "break"
      case symbol_kind::S_CASE: // "case"
      case symbol_kind::S_CHAR: // "char"
      case symbol_kind::S_CONSTT: // "const"
      case symbol_kind::S_CONTINUE: // "continue"
      case symbol_kind::S_DEFAULT: // "default"
      case symbol_kind::S_DO: // "do"
      case symbol_kind::S_DOUBLE: // "double"
      case symbol_kind::S_LONG_DOUBLE: // "long double"
      case symbol_kind::S_ELSE: // "else"
      case symbol_kind::S_ENUM: // "enum"
      case symbol_kind::S_EXTERN: // "extern"
      case symbol_kind::S_FLOAT: // "float"
      case symbol_kind::S_FOR: // "for"
      case symbol_kind::S_GOTO: // "goto"
      case symbol_kind::S_IF: // "if"
      case symbol_kind::S_INLINE: // "inline"
      case symbol_kind::S_INT: // INT
      case symbol_kind::S_LONG: // "long"
      case symbol_kind::S_LONG_LONG: // "long long"
      case symbol_kind::S_REGISTER: // "register"
      case symbol_kind::S_RESTRICT: // "restrict"
      case symbol_kind::S_RETURN: // "return"
      case symbol_kind::S_SHORTT: // "short"
      case symbol_kind::S_SIGNED: // "signed"
      case symbol_kind::S_SIZEOF: // "sizeof"
      case symbol_kind::S_STATIC: // "static"
      case symbol_kind::S_STRUCT: // "struct"
      case symbol_kind::S_SWITCH: // "switch"
      case symbol_kind::S_TYPEDEF: // "typedef"
      case symbol_kind::S_UNION: // "union"
      case symbol_kind::S_UNSIGNED: // "unsigned"
      case symbol_kind::S_VOIDT: // "void"
      case symbol_kind::S_VOLATILE: // "volatile"
      case symbol_kind::S_WHILE: // "while"
      case symbol_kind::S_BOOLT: // "bool"
      case symbol_kind::S_ELLIPSIS: // "ellipsis"
      case symbol_kind::S_QUESTION: // "question"
      case symbol_kind::S_SEMICOLON: // ";"
      case symbol_kind::S_OCURLY: // "{"
      case symbol_kind::S_CCURLY: // "}"
      case symbol_kind::S_COMMA: // ","
      case symbol_kind::S_COLON: // ":"
      case symbol_kind::S_OPAREN: // "("
      case symbol_kind::S_CPAREN: // ")"
      case symbol_kind::S_OBRACE: // "["
      case symbol_kind::S_CBRACE: // "]"
      case symbol_kind::S_PERIOD: // "."
      case symbol_kind::S_TILDE: // "~"
      case symbol_kind::S_EQUAL: // "="
      case symbol_kind::S_RIGHT_ASSIGN: // ">>="
      case symbol_kind::S_LEFT_ASSIGN: // "<<="
      case symbol_kind::S_ADD_ASSIGN: // "+="
      case symbol_kind::S_SUB_ASSIGN: // "-="
      case symbol_kind::S_MUL_ASSIGN: // "*="
      case symbol_kind::S_DIV_ASSIGN: // "/="
      case symbol_kind::S_MOD_ASSIGN: // "%="
      case symbol_kind::S_AND_ASSIGN: // "&="
      case symbol_kind::S_XOR_ASSIGN: // "^="
      case symbol_kind::S_OR_ASSIGN: // "|="
      case symbol_kind::S_RIGHT_OP: // ">>"
      case symbol_kind::S_LEFT_OP: // "<<"
      case symbol_kind::S_INC_OP: // "++"
      case symbol_kind::S_DEC_OP: // "--"
      case symbol_kind::S_PTR_OP: // "->"
      case symbol_kind::S_AND_OP: // "&&"
      case symbol_kind::S_OR_OP: // "||"
      case symbol_kind::S_GREATER_EQUAL: // ">="
      case symbol_kind::S_LESS_EQUAL: // "<="
      case symbol_kind::S_GREATER: // ">"
      case symbol_kind::S_LESS: // "<"
      case symbol_kind::S_EQUAL_EQUAL: // "=="
      case symbol_kind::S_NOT_EQUAL: // "!="
      case symbol_kind::S_NOT_OP: // "!"
      case symbol_kind::S_XOR_OP: // "^"
      case symbol_kind::S_BIT_AND: // "&"
      case symbol_kind::S_BIT_OR: // "|"
      case symbol_kind::S_MINUS_OP: // "-"
      case symbol_kind::S_PLUS_OP: // "+"
      case symbol_kind::S_TIMES_OP: // "*"
      case symbol_kind::S_DIV_OP: // "/"
      case symbol_kind::S_MOD_OP: // "%"
      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assignment_operator: // assignment_operator
      case symbol_kind::S_struct_or_union: // struct_or_union
        value.YY_MOVE_OR_COPY< shared_ptr<CToken> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constant: // constant
        value.YY_MOVE_OR_COPY< shared_ptr<Constant> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.YY_MOVE_OR_COPY< shared_ptr<Declaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration_specifiers: // declaration_specifiers
        value.YY_MOVE_OR_COPY< shared_ptr<DeclarationSpecifiers> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declarator: // declarator
        value.YY_MOVE_OR_COPY< shared_ptr<Declarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_direct_abstract_declarator: // direct_abstract_declarator
        value.YY_MOVE_OR_COPY< shared_ptr<DirectAbstractDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_direct_declarator: // direct_declarator
        value.YY_MOVE_OR_COPY< shared_ptr<DirectDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.YY_MOVE_OR_COPY< shared_ptr<EnumSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.YY_MOVE_OR_COPY< shared_ptr<Enumerator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_primary_expression: // primary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_cast_expression: // cast_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_and_expression: // and_expression
      case symbol_kind::S_exclusive_or_expression: // exclusive_or_expression
      case symbol_kind::S_inclusive_or_expression: // inclusive_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_conditional_expression: // conditional_expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_constant_expression: // constant_expression
        value.YY_MOVE_OR_COPY< shared_ptr<ExpressionTree> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_external_declaration: // external_declaration
        value.YY_MOVE_OR_COPY< shared_ptr<ExternalDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.YY_MOVE_OR_COPY< shared_ptr<FunctionDefinition> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_declarator: // init_declarator
        value.YY_MOVE_OR_COPY< shared_ptr<InitDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_initializer: // initializer
        value.YY_MOVE_OR_COPY< shared_ptr<Initializer> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_labeled_statement: // labeled_statement
        value.YY_MOVE_OR_COPY< shared_ptr<LabeledStatement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.YY_MOVE_OR_COPY< shared_ptr<ParameterDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_type_list: // parameter_type_list
        value.YY_MOVE_OR_COPY< shared_ptr<ParameterTypeList> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pointer: // pointer
        value.YY_MOVE_OR_COPY< shared_ptr<Pointer> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specifier_qualifier_list: // specifier_qualifier_list
        value.YY_MOVE_OR_COPY< shared_ptr<SpecifierQualifierList> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
        value.YY_MOVE_OR_COPY< shared_ptr<StorageClassSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.YY_MOVE_OR_COPY< shared_ptr<StructDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.YY_MOVE_OR_COPY< shared_ptr<StructDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_or_union_specifier: // struct_or_union_specifier
        value.YY_MOVE_OR_COPY< shared_ptr<StructOrUnionSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_name: // type_name
        value.YY_MOVE_OR_COPY< shared_ptr<TypeName> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_qualifier: // type_qualifier
        value.YY_MOVE_OR_COPY< shared_ptr<TypeQualifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.YY_MOVE_OR_COPY< shared_ptr<TypeSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement_list: // statement_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<BaseStatement>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_identifier_list: // identifier_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<CToken>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration_list: // declaration_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<Declaration>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<Enumerator>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_expression_list: // argument_expression_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<ExpressionTree>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_declarator_list: // init_declarator_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<InitDeclarator>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_initializer_list: // initializer_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<Initializer>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declaration_list: // struct_declaration_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<StructDeclaration>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<StructDeclarator>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_qualifier_list: // type_qualifier_list
        value.YY_MOVE_OR_COPY< shared_ptr<vector<shared_ptr<TypeQualifier>>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

   Parser ::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_abstract_declarator: // abstract_declarator
        value.move< shared_ptr<AbstractDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement: // statement
      case symbol_kind::S_compound_statement: // compound_statement
      case symbol_kind::S_expression_statement: // expression_statement
      case symbol_kind::S_selection_statement: // selection_statement
      case symbol_kind::S_iteration_statement: // iteration_statement
      case symbol_kind::S_jump_statement: // jump_statement
        value.move< shared_ptr<BaseStatement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_I_CONST: // "i_const"
      case symbol_kind::S_F_CONST: // "f_const"
      case symbol_kind::S_STRING_LITERAL: // "sting_literal"
      case symbol_kind::S_TYPE_NAME: // "type name"
      case symbol_kind::S_AUTO: // "auto"
      case symbol_kind::S_BREAK: // "break"
      case symbol_kind::S_CASE: // "case"
      case symbol_kind::S_CHAR: // "char"
      case symbol_kind::S_CONSTT: // "const"
      case symbol_kind::S_CONTINUE: // "continue"
      case symbol_kind::S_DEFAULT: // "default"
      case symbol_kind::S_DO: // "do"
      case symbol_kind::S_DOUBLE: // "double"
      case symbol_kind::S_LONG_DOUBLE: // "long double"
      case symbol_kind::S_ELSE: // "else"
      case symbol_kind::S_ENUM: // "enum"
      case symbol_kind::S_EXTERN: // "extern"
      case symbol_kind::S_FLOAT: // "float"
      case symbol_kind::S_FOR: // "for"
      case symbol_kind::S_GOTO: // "goto"
      case symbol_kind::S_IF: // "if"
      case symbol_kind::S_INLINE: // "inline"
      case symbol_kind::S_INT: // INT
      case symbol_kind::S_LONG: // "long"
      case symbol_kind::S_LONG_LONG: // "long long"
      case symbol_kind::S_REGISTER: // "register"
      case symbol_kind::S_RESTRICT: // "restrict"
      case symbol_kind::S_RETURN: // "return"
      case symbol_kind::S_SHORTT: // "short"
      case symbol_kind::S_SIGNED: // "signed"
      case symbol_kind::S_SIZEOF: // "sizeof"
      case symbol_kind::S_STATIC: // "static"
      case symbol_kind::S_STRUCT: // "struct"
      case symbol_kind::S_SWITCH: // "switch"
      case symbol_kind::S_TYPEDEF: // "typedef"
      case symbol_kind::S_UNION: // "union"
      case symbol_kind::S_UNSIGNED: // "unsigned"
      case symbol_kind::S_VOIDT: // "void"
      case symbol_kind::S_VOLATILE: // "volatile"
      case symbol_kind::S_WHILE: // "while"
      case symbol_kind::S_BOOLT: // "bool"
      case symbol_kind::S_ELLIPSIS: // "ellipsis"
      case symbol_kind::S_QUESTION: // "question"
      case symbol_kind::S_SEMICOLON: // ";"
      case symbol_kind::S_OCURLY: // "{"
      case symbol_kind::S_CCURLY: // "}"
      case symbol_kind::S_COMMA: // ","
      case symbol_kind::S_COLON: // ":"
      case symbol_kind::S_OPAREN: // "("
      case symbol_kind::S_CPAREN: // ")"
      case symbol_kind::S_OBRACE: // "["
      case symbol_kind::S_CBRACE: // "]"
      case symbol_kind::S_PERIOD: // "."
      case symbol_kind::S_TILDE: // "~"
      case symbol_kind::S_EQUAL: // "="
      case symbol_kind::S_RIGHT_ASSIGN: // ">>="
      case symbol_kind::S_LEFT_ASSIGN: // "<<="
      case symbol_kind::S_ADD_ASSIGN: // "+="
      case symbol_kind::S_SUB_ASSIGN: // "-="
      case symbol_kind::S_MUL_ASSIGN: // "*="
      case symbol_kind::S_DIV_ASSIGN: // "/="
      case symbol_kind::S_MOD_ASSIGN: // "%="
      case symbol_kind::S_AND_ASSIGN: // "&="
      case symbol_kind::S_XOR_ASSIGN: // "^="
      case symbol_kind::S_OR_ASSIGN: // "|="
      case symbol_kind::S_RIGHT_OP: // ">>"
      case symbol_kind::S_LEFT_OP: // "<<"
      case symbol_kind::S_INC_OP: // "++"
      case symbol_kind::S_DEC_OP: // "--"
      case symbol_kind::S_PTR_OP: // "->"
      case symbol_kind::S_AND_OP: // "&&"
      case symbol_kind::S_OR_OP: // "||"
      case symbol_kind::S_GREATER_EQUAL: // ">="
      case symbol_kind::S_LESS_EQUAL: // "<="
      case symbol_kind::S_GREATER: // ">"
      case symbol_kind::S_LESS: // "<"
      case symbol_kind::S_EQUAL_EQUAL: // "=="
      case symbol_kind::S_NOT_EQUAL: // "!="
      case symbol_kind::S_NOT_OP: // "!"
      case symbol_kind::S_XOR_OP: // "^"
      case symbol_kind::S_BIT_AND: // "&"
      case symbol_kind::S_BIT_OR: // "|"
      case symbol_kind::S_MINUS_OP: // "-"
      case symbol_kind::S_PLUS_OP: // "+"
      case symbol_kind::S_TIMES_OP: // "*"
      case symbol_kind::S_DIV_OP: // "/"
      case symbol_kind::S_MOD_OP: // "%"
      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assignment_operator: // assignment_operator
      case symbol_kind::S_struct_or_union: // struct_or_union
        value.move< shared_ptr<CToken> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constant: // constant
        value.move< shared_ptr<Constant> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< shared_ptr<Declaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration_specifiers: // declaration_specifiers
        value.move< shared_ptr<DeclarationSpecifiers> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declarator: // declarator
        value.move< shared_ptr<Declarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_direct_abstract_declarator: // direct_abstract_declarator
        value.move< shared_ptr<DirectAbstractDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_direct_declarator: // direct_declarator
        value.move< shared_ptr<DirectDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.move< shared_ptr<EnumSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.move< shared_ptr<Enumerator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_primary_expression: // primary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_cast_expression: // cast_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_and_expression: // and_expression
      case symbol_kind::S_exclusive_or_expression: // exclusive_or_expression
      case symbol_kind::S_inclusive_or_expression: // inclusive_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_conditional_expression: // conditional_expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_constant_expression: // constant_expression
        value.move< shared_ptr<ExpressionTree> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_external_declaration: // external_declaration
        value.move< shared_ptr<ExternalDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.move< shared_ptr<FunctionDefinition> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_declarator: // init_declarator
        value.move< shared_ptr<InitDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_initializer: // initializer
        value.move< shared_ptr<Initializer> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_labeled_statement: // labeled_statement
        value.move< shared_ptr<LabeledStatement> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.move< shared_ptr<ParameterDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_type_list: // parameter_type_list
        value.move< shared_ptr<ParameterTypeList> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_pointer: // pointer
        value.move< shared_ptr<Pointer> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specifier_qualifier_list: // specifier_qualifier_list
        value.move< shared_ptr<SpecifierQualifierList> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
        value.move< shared_ptr<StorageClassSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.move< shared_ptr<StructDeclaration> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.move< shared_ptr<StructDeclarator> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_or_union_specifier: // struct_or_union_specifier
        value.move< shared_ptr<StructOrUnionSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_name: // type_name
        value.move< shared_ptr<TypeName> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_qualifier: // type_qualifier
        value.move< shared_ptr<TypeQualifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.move< shared_ptr<TypeSpecifier> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_statement_list: // statement_list
        value.move< shared_ptr<vector<shared_ptr<BaseStatement>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_identifier_list: // identifier_list
        value.move< shared_ptr<vector<shared_ptr<CToken>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_declaration_list: // declaration_list
        value.move< shared_ptr<vector<shared_ptr<Declaration>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.move< shared_ptr<vector<shared_ptr<Enumerator>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_argument_expression_list: // argument_expression_list
        value.move< shared_ptr<vector<shared_ptr<ExpressionTree>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_init_declarator_list: // init_declarator_list
        value.move< shared_ptr<vector<shared_ptr<InitDeclarator>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_initializer_list: // initializer_list
        value.move< shared_ptr<vector<shared_ptr<Initializer>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.move< shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declaration_list: // struct_declaration_list
        value.move< shared_ptr<vector<shared_ptr<StructDeclaration>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.move< shared_ptr<vector<shared_ptr<StructDeclarator>>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_qualifier_list: // type_qualifier_list
        value.move< shared_ptr<vector<shared_ptr<TypeQualifier>>> > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
   Parser ::stack_symbol_type&
   Parser ::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_abstract_declarator: // abstract_declarator
        value.copy< shared_ptr<AbstractDeclarator> > (that.value);
        break;

      case symbol_kind::S_statement: // statement
      case symbol_kind::S_compound_statement: // compound_statement
      case symbol_kind::S_expression_statement: // expression_statement
      case symbol_kind::S_selection_statement: // selection_statement
      case symbol_kind::S_iteration_statement: // iteration_statement
      case symbol_kind::S_jump_statement: // jump_statement
        value.copy< shared_ptr<BaseStatement> > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_I_CONST: // "i_const"
      case symbol_kind::S_F_CONST: // "f_const"
      case symbol_kind::S_STRING_LITERAL: // "sting_literal"
      case symbol_kind::S_TYPE_NAME: // "type name"
      case symbol_kind::S_AUTO: // "auto"
      case symbol_kind::S_BREAK: // "break"
      case symbol_kind::S_CASE: // "case"
      case symbol_kind::S_CHAR: // "char"
      case symbol_kind::S_CONSTT: // "const"
      case symbol_kind::S_CONTINUE: // "continue"
      case symbol_kind::S_DEFAULT: // "default"
      case symbol_kind::S_DO: // "do"
      case symbol_kind::S_DOUBLE: // "double"
      case symbol_kind::S_LONG_DOUBLE: // "long double"
      case symbol_kind::S_ELSE: // "else"
      case symbol_kind::S_ENUM: // "enum"
      case symbol_kind::S_EXTERN: // "extern"
      case symbol_kind::S_FLOAT: // "float"
      case symbol_kind::S_FOR: // "for"
      case symbol_kind::S_GOTO: // "goto"
      case symbol_kind::S_IF: // "if"
      case symbol_kind::S_INLINE: // "inline"
      case symbol_kind::S_INT: // INT
      case symbol_kind::S_LONG: // "long"
      case symbol_kind::S_LONG_LONG: // "long long"
      case symbol_kind::S_REGISTER: // "register"
      case symbol_kind::S_RESTRICT: // "restrict"
      case symbol_kind::S_RETURN: // "return"
      case symbol_kind::S_SHORTT: // "short"
      case symbol_kind::S_SIGNED: // "signed"
      case symbol_kind::S_SIZEOF: // "sizeof"
      case symbol_kind::S_STATIC: // "static"
      case symbol_kind::S_STRUCT: // "struct"
      case symbol_kind::S_SWITCH: // "switch"
      case symbol_kind::S_TYPEDEF: // "typedef"
      case symbol_kind::S_UNION: // "union"
      case symbol_kind::S_UNSIGNED: // "unsigned"
      case symbol_kind::S_VOIDT: // "void"
      case symbol_kind::S_VOLATILE: // "volatile"
      case symbol_kind::S_WHILE: // "while"
      case symbol_kind::S_BOOLT: // "bool"
      case symbol_kind::S_ELLIPSIS: // "ellipsis"
      case symbol_kind::S_QUESTION: // "question"
      case symbol_kind::S_SEMICOLON: // ";"
      case symbol_kind::S_OCURLY: // "{"
      case symbol_kind::S_CCURLY: // "}"
      case symbol_kind::S_COMMA: // ","
      case symbol_kind::S_COLON: // ":"
      case symbol_kind::S_OPAREN: // "("
      case symbol_kind::S_CPAREN: // ")"
      case symbol_kind::S_OBRACE: // "["
      case symbol_kind::S_CBRACE: // "]"
      case symbol_kind::S_PERIOD: // "."
      case symbol_kind::S_TILDE: // "~"
      case symbol_kind::S_EQUAL: // "="
      case symbol_kind::S_RIGHT_ASSIGN: // ">>="
      case symbol_kind::S_LEFT_ASSIGN: // "<<="
      case symbol_kind::S_ADD_ASSIGN: // "+="
      case symbol_kind::S_SUB_ASSIGN: // "-="
      case symbol_kind::S_MUL_ASSIGN: // "*="
      case symbol_kind::S_DIV_ASSIGN: // "/="
      case symbol_kind::S_MOD_ASSIGN: // "%="
      case symbol_kind::S_AND_ASSIGN: // "&="
      case symbol_kind::S_XOR_ASSIGN: // "^="
      case symbol_kind::S_OR_ASSIGN: // "|="
      case symbol_kind::S_RIGHT_OP: // ">>"
      case symbol_kind::S_LEFT_OP: // "<<"
      case symbol_kind::S_INC_OP: // "++"
      case symbol_kind::S_DEC_OP: // "--"
      case symbol_kind::S_PTR_OP: // "->"
      case symbol_kind::S_AND_OP: // "&&"
      case symbol_kind::S_OR_OP: // "||"
      case symbol_kind::S_GREATER_EQUAL: // ">="
      case symbol_kind::S_LESS_EQUAL: // "<="
      case symbol_kind::S_GREATER: // ">"
      case symbol_kind::S_LESS: // "<"
      case symbol_kind::S_EQUAL_EQUAL: // "=="
      case symbol_kind::S_NOT_EQUAL: // "!="
      case symbol_kind::S_NOT_OP: // "!"
      case symbol_kind::S_XOR_OP: // "^"
      case symbol_kind::S_BIT_AND: // "&"
      case symbol_kind::S_BIT_OR: // "|"
      case symbol_kind::S_MINUS_OP: // "-"
      case symbol_kind::S_PLUS_OP: // "+"
      case symbol_kind::S_TIMES_OP: // "*"
      case symbol_kind::S_DIV_OP: // "/"
      case symbol_kind::S_MOD_OP: // "%"
      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assignment_operator: // assignment_operator
      case symbol_kind::S_struct_or_union: // struct_or_union
        value.copy< shared_ptr<CToken> > (that.value);
        break;

      case symbol_kind::S_constant: // constant
        value.copy< shared_ptr<Constant> > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.copy< shared_ptr<Declaration> > (that.value);
        break;

      case symbol_kind::S_declaration_specifiers: // declaration_specifiers
        value.copy< shared_ptr<DeclarationSpecifiers> > (that.value);
        break;

      case symbol_kind::S_declarator: // declarator
        value.copy< shared_ptr<Declarator> > (that.value);
        break;

      case symbol_kind::S_direct_abstract_declarator: // direct_abstract_declarator
        value.copy< shared_ptr<DirectAbstractDeclarator> > (that.value);
        break;

      case symbol_kind::S_direct_declarator: // direct_declarator
        value.copy< shared_ptr<DirectDeclarator> > (that.value);
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.copy< shared_ptr<EnumSpecifier> > (that.value);
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.copy< shared_ptr<Enumerator> > (that.value);
        break;

      case symbol_kind::S_primary_expression: // primary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_cast_expression: // cast_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_and_expression: // and_expression
      case symbol_kind::S_exclusive_or_expression: // exclusive_or_expression
      case symbol_kind::S_inclusive_or_expression: // inclusive_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_conditional_expression: // conditional_expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_constant_expression: // constant_expression
        value.copy< shared_ptr<ExpressionTree> > (that.value);
        break;

      case symbol_kind::S_external_declaration: // external_declaration
        value.copy< shared_ptr<ExternalDeclaration> > (that.value);
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.copy< shared_ptr<FunctionDefinition> > (that.value);
        break;

      case symbol_kind::S_init_declarator: // init_declarator
        value.copy< shared_ptr<InitDeclarator> > (that.value);
        break;

      case symbol_kind::S_initializer: // initializer
        value.copy< shared_ptr<Initializer> > (that.value);
        break;

      case symbol_kind::S_labeled_statement: // labeled_statement
        value.copy< shared_ptr<LabeledStatement> > (that.value);
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.copy< shared_ptr<ParameterDeclaration> > (that.value);
        break;

      case symbol_kind::S_parameter_type_list: // parameter_type_list
        value.copy< shared_ptr<ParameterTypeList> > (that.value);
        break;

      case symbol_kind::S_pointer: // pointer
        value.copy< shared_ptr<Pointer> > (that.value);
        break;

      case symbol_kind::S_specifier_qualifier_list: // specifier_qualifier_list
        value.copy< shared_ptr<SpecifierQualifierList> > (that.value);
        break;

      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
        value.copy< shared_ptr<StorageClassSpecifier> > (that.value);
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.copy< shared_ptr<StructDeclaration> > (that.value);
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.copy< shared_ptr<StructDeclarator> > (that.value);
        break;

      case symbol_kind::S_struct_or_union_specifier: // struct_or_union_specifier
        value.copy< shared_ptr<StructOrUnionSpecifier> > (that.value);
        break;

      case symbol_kind::S_type_name: // type_name
        value.copy< shared_ptr<TypeName> > (that.value);
        break;

      case symbol_kind::S_type_qualifier: // type_qualifier
        value.copy< shared_ptr<TypeQualifier> > (that.value);
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.copy< shared_ptr<TypeSpecifier> > (that.value);
        break;

      case symbol_kind::S_statement_list: // statement_list
        value.copy< shared_ptr<vector<shared_ptr<BaseStatement>>> > (that.value);
        break;

      case symbol_kind::S_identifier_list: // identifier_list
        value.copy< shared_ptr<vector<shared_ptr<CToken>>> > (that.value);
        break;

      case symbol_kind::S_declaration_list: // declaration_list
        value.copy< shared_ptr<vector<shared_ptr<Declaration>>> > (that.value);
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.copy< shared_ptr<vector<shared_ptr<Enumerator>>> > (that.value);
        break;

      case symbol_kind::S_argument_expression_list: // argument_expression_list
        value.copy< shared_ptr<vector<shared_ptr<ExpressionTree>>> > (that.value);
        break;

      case symbol_kind::S_init_declarator_list: // init_declarator_list
        value.copy< shared_ptr<vector<shared_ptr<InitDeclarator>>> > (that.value);
        break;

      case symbol_kind::S_initializer_list: // initializer_list
        value.copy< shared_ptr<vector<shared_ptr<Initializer>>> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.copy< shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > (that.value);
        break;

      case symbol_kind::S_struct_declaration_list: // struct_declaration_list
        value.copy< shared_ptr<vector<shared_ptr<StructDeclaration>>> > (that.value);
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.copy< shared_ptr<vector<shared_ptr<StructDeclarator>>> > (that.value);
        break;

      case symbol_kind::S_type_qualifier_list: // type_qualifier_list
        value.copy< shared_ptr<vector<shared_ptr<TypeQualifier>>> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

   Parser ::stack_symbol_type&
   Parser ::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_abstract_declarator: // abstract_declarator
        value.move< shared_ptr<AbstractDeclarator> > (that.value);
        break;

      case symbol_kind::S_statement: // statement
      case symbol_kind::S_compound_statement: // compound_statement
      case symbol_kind::S_expression_statement: // expression_statement
      case symbol_kind::S_selection_statement: // selection_statement
      case symbol_kind::S_iteration_statement: // iteration_statement
      case symbol_kind::S_jump_statement: // jump_statement
        value.move< shared_ptr<BaseStatement> > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_I_CONST: // "i_const"
      case symbol_kind::S_F_CONST: // "f_const"
      case symbol_kind::S_STRING_LITERAL: // "sting_literal"
      case symbol_kind::S_TYPE_NAME: // "type name"
      case symbol_kind::S_AUTO: // "auto"
      case symbol_kind::S_BREAK: // "break"
      case symbol_kind::S_CASE: // "case"
      case symbol_kind::S_CHAR: // "char"
      case symbol_kind::S_CONSTT: // "const"
      case symbol_kind::S_CONTINUE: // "continue"
      case symbol_kind::S_DEFAULT: // "default"
      case symbol_kind::S_DO: // "do"
      case symbol_kind::S_DOUBLE: // "double"
      case symbol_kind::S_LONG_DOUBLE: // "long double"
      case symbol_kind::S_ELSE: // "else"
      case symbol_kind::S_ENUM: // "enum"
      case symbol_kind::S_EXTERN: // "extern"
      case symbol_kind::S_FLOAT: // "float"
      case symbol_kind::S_FOR: // "for"
      case symbol_kind::S_GOTO: // "goto"
      case symbol_kind::S_IF: // "if"
      case symbol_kind::S_INLINE: // "inline"
      case symbol_kind::S_INT: // INT
      case symbol_kind::S_LONG: // "long"
      case symbol_kind::S_LONG_LONG: // "long long"
      case symbol_kind::S_REGISTER: // "register"
      case symbol_kind::S_RESTRICT: // "restrict"
      case symbol_kind::S_RETURN: // "return"
      case symbol_kind::S_SHORTT: // "short"
      case symbol_kind::S_SIGNED: // "signed"
      case symbol_kind::S_SIZEOF: // "sizeof"
      case symbol_kind::S_STATIC: // "static"
      case symbol_kind::S_STRUCT: // "struct"
      case symbol_kind::S_SWITCH: // "switch"
      case symbol_kind::S_TYPEDEF: // "typedef"
      case symbol_kind::S_UNION: // "union"
      case symbol_kind::S_UNSIGNED: // "unsigned"
      case symbol_kind::S_VOIDT: // "void"
      case symbol_kind::S_VOLATILE: // "volatile"
      case symbol_kind::S_WHILE: // "while"
      case symbol_kind::S_BOOLT: // "bool"
      case symbol_kind::S_ELLIPSIS: // "ellipsis"
      case symbol_kind::S_QUESTION: // "question"
      case symbol_kind::S_SEMICOLON: // ";"
      case symbol_kind::S_OCURLY: // "{"
      case symbol_kind::S_CCURLY: // "}"
      case symbol_kind::S_COMMA: // ","
      case symbol_kind::S_COLON: // ":"
      case symbol_kind::S_OPAREN: // "("
      case symbol_kind::S_CPAREN: // ")"
      case symbol_kind::S_OBRACE: // "["
      case symbol_kind::S_CBRACE: // "]"
      case symbol_kind::S_PERIOD: // "."
      case symbol_kind::S_TILDE: // "~"
      case symbol_kind::S_EQUAL: // "="
      case symbol_kind::S_RIGHT_ASSIGN: // ">>="
      case symbol_kind::S_LEFT_ASSIGN: // "<<="
      case symbol_kind::S_ADD_ASSIGN: // "+="
      case symbol_kind::S_SUB_ASSIGN: // "-="
      case symbol_kind::S_MUL_ASSIGN: // "*="
      case symbol_kind::S_DIV_ASSIGN: // "/="
      case symbol_kind::S_MOD_ASSIGN: // "%="
      case symbol_kind::S_AND_ASSIGN: // "&="
      case symbol_kind::S_XOR_ASSIGN: // "^="
      case symbol_kind::S_OR_ASSIGN: // "|="
      case symbol_kind::S_RIGHT_OP: // ">>"
      case symbol_kind::S_LEFT_OP: // "<<"
      case symbol_kind::S_INC_OP: // "++"
      case symbol_kind::S_DEC_OP: // "--"
      case symbol_kind::S_PTR_OP: // "->"
      case symbol_kind::S_AND_OP: // "&&"
      case symbol_kind::S_OR_OP: // "||"
      case symbol_kind::S_GREATER_EQUAL: // ">="
      case symbol_kind::S_LESS_EQUAL: // "<="
      case symbol_kind::S_GREATER: // ">"
      case symbol_kind::S_LESS: // "<"
      case symbol_kind::S_EQUAL_EQUAL: // "=="
      case symbol_kind::S_NOT_EQUAL: // "!="
      case symbol_kind::S_NOT_OP: // "!"
      case symbol_kind::S_XOR_OP: // "^"
      case symbol_kind::S_BIT_AND: // "&"
      case symbol_kind::S_BIT_OR: // "|"
      case symbol_kind::S_MINUS_OP: // "-"
      case symbol_kind::S_PLUS_OP: // "+"
      case symbol_kind::S_TIMES_OP: // "*"
      case symbol_kind::S_DIV_OP: // "/"
      case symbol_kind::S_MOD_OP: // "%"
      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assignment_operator: // assignment_operator
      case symbol_kind::S_struct_or_union: // struct_or_union
        value.move< shared_ptr<CToken> > (that.value);
        break;

      case symbol_kind::S_constant: // constant
        value.move< shared_ptr<Constant> > (that.value);
        break;

      case symbol_kind::S_declaration: // declaration
        value.move< shared_ptr<Declaration> > (that.value);
        break;

      case symbol_kind::S_declaration_specifiers: // declaration_specifiers
        value.move< shared_ptr<DeclarationSpecifiers> > (that.value);
        break;

      case symbol_kind::S_declarator: // declarator
        value.move< shared_ptr<Declarator> > (that.value);
        break;

      case symbol_kind::S_direct_abstract_declarator: // direct_abstract_declarator
        value.move< shared_ptr<DirectAbstractDeclarator> > (that.value);
        break;

      case symbol_kind::S_direct_declarator: // direct_declarator
        value.move< shared_ptr<DirectDeclarator> > (that.value);
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        value.move< shared_ptr<EnumSpecifier> > (that.value);
        break;

      case symbol_kind::S_enumerator: // enumerator
        value.move< shared_ptr<Enumerator> > (that.value);
        break;

      case symbol_kind::S_primary_expression: // primary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_cast_expression: // cast_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_and_expression: // and_expression
      case symbol_kind::S_exclusive_or_expression: // exclusive_or_expression
      case symbol_kind::S_inclusive_or_expression: // inclusive_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_conditional_expression: // conditional_expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_constant_expression: // constant_expression
        value.move< shared_ptr<ExpressionTree> > (that.value);
        break;

      case symbol_kind::S_external_declaration: // external_declaration
        value.move< shared_ptr<ExternalDeclaration> > (that.value);
        break;

      case symbol_kind::S_function_definition: // function_definition
        value.move< shared_ptr<FunctionDefinition> > (that.value);
        break;

      case symbol_kind::S_init_declarator: // init_declarator
        value.move< shared_ptr<InitDeclarator> > (that.value);
        break;

      case symbol_kind::S_initializer: // initializer
        value.move< shared_ptr<Initializer> > (that.value);
        break;

      case symbol_kind::S_labeled_statement: // labeled_statement
        value.move< shared_ptr<LabeledStatement> > (that.value);
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        value.move< shared_ptr<ParameterDeclaration> > (that.value);
        break;

      case symbol_kind::S_parameter_type_list: // parameter_type_list
        value.move< shared_ptr<ParameterTypeList> > (that.value);
        break;

      case symbol_kind::S_pointer: // pointer
        value.move< shared_ptr<Pointer> > (that.value);
        break;

      case symbol_kind::S_specifier_qualifier_list: // specifier_qualifier_list
        value.move< shared_ptr<SpecifierQualifierList> > (that.value);
        break;

      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
        value.move< shared_ptr<StorageClassSpecifier> > (that.value);
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        value.move< shared_ptr<StructDeclaration> > (that.value);
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        value.move< shared_ptr<StructDeclarator> > (that.value);
        break;

      case symbol_kind::S_struct_or_union_specifier: // struct_or_union_specifier
        value.move< shared_ptr<StructOrUnionSpecifier> > (that.value);
        break;

      case symbol_kind::S_type_name: // type_name
        value.move< shared_ptr<TypeName> > (that.value);
        break;

      case symbol_kind::S_type_qualifier: // type_qualifier
        value.move< shared_ptr<TypeQualifier> > (that.value);
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        value.move< shared_ptr<TypeSpecifier> > (that.value);
        break;

      case symbol_kind::S_statement_list: // statement_list
        value.move< shared_ptr<vector<shared_ptr<BaseStatement>>> > (that.value);
        break;

      case symbol_kind::S_identifier_list: // identifier_list
        value.move< shared_ptr<vector<shared_ptr<CToken>>> > (that.value);
        break;

      case symbol_kind::S_declaration_list: // declaration_list
        value.move< shared_ptr<vector<shared_ptr<Declaration>>> > (that.value);
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        value.move< shared_ptr<vector<shared_ptr<Enumerator>>> > (that.value);
        break;

      case symbol_kind::S_argument_expression_list: // argument_expression_list
        value.move< shared_ptr<vector<shared_ptr<ExpressionTree>>> > (that.value);
        break;

      case symbol_kind::S_init_declarator_list: // init_declarator_list
        value.move< shared_ptr<vector<shared_ptr<InitDeclarator>>> > (that.value);
        break;

      case symbol_kind::S_initializer_list: // initializer_list
        value.move< shared_ptr<vector<shared_ptr<Initializer>>> > (that.value);
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        value.move< shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > (that.value);
        break;

      case symbol_kind::S_struct_declaration_list: // struct_declaration_list
        value.move< shared_ptr<vector<shared_ptr<StructDeclaration>>> > (that.value);
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        value.move< shared_ptr<vector<shared_ptr<StructDeclarator>>> > (that.value);
        break;

      case symbol_kind::S_type_qualifier_list: // type_qualifier_list
        value.move< shared_ptr<vector<shared_ptr<TypeQualifier>>> > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
   Parser ::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
   Parser ::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
   Parser ::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
   Parser ::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
   Parser ::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
   Parser ::debug_stream () const
  {
    return *yycdebug_;
  }

  void
   Parser ::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


   Parser ::debug_level_type
   Parser ::debug_level () const
  {
    return yydebug_;
  }

  void
   Parser ::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

   Parser ::state_type
   Parser ::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
   Parser ::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
   Parser ::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
   Parser ::operator() ()
  {
    return parse ();
  }

  int
   Parser ::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (scanner, driver));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_abstract_declarator: // abstract_declarator
        yylhs.value.emplace< shared_ptr<AbstractDeclarator> > ();
        break;

      case symbol_kind::S_statement: // statement
      case symbol_kind::S_compound_statement: // compound_statement
      case symbol_kind::S_expression_statement: // expression_statement
      case symbol_kind::S_selection_statement: // selection_statement
      case symbol_kind::S_iteration_statement: // iteration_statement
      case symbol_kind::S_jump_statement: // jump_statement
        yylhs.value.emplace< shared_ptr<BaseStatement> > ();
        break;

      case symbol_kind::S_IDENTIFIER: // "identifier"
      case symbol_kind::S_I_CONST: // "i_const"
      case symbol_kind::S_F_CONST: // "f_const"
      case symbol_kind::S_STRING_LITERAL: // "sting_literal"
      case symbol_kind::S_TYPE_NAME: // "type name"
      case symbol_kind::S_AUTO: // "auto"
      case symbol_kind::S_BREAK: // "break"
      case symbol_kind::S_CASE: // "case"
      case symbol_kind::S_CHAR: // "char"
      case symbol_kind::S_CONSTT: // "const"
      case symbol_kind::S_CONTINUE: // "continue"
      case symbol_kind::S_DEFAULT: // "default"
      case symbol_kind::S_DO: // "do"
      case symbol_kind::S_DOUBLE: // "double"
      case symbol_kind::S_LONG_DOUBLE: // "long double"
      case symbol_kind::S_ELSE: // "else"
      case symbol_kind::S_ENUM: // "enum"
      case symbol_kind::S_EXTERN: // "extern"
      case symbol_kind::S_FLOAT: // "float"
      case symbol_kind::S_FOR: // "for"
      case symbol_kind::S_GOTO: // "goto"
      case symbol_kind::S_IF: // "if"
      case symbol_kind::S_INLINE: // "inline"
      case symbol_kind::S_INT: // INT
      case symbol_kind::S_LONG: // "long"
      case symbol_kind::S_LONG_LONG: // "long long"
      case symbol_kind::S_REGISTER: // "register"
      case symbol_kind::S_RESTRICT: // "restrict"
      case symbol_kind::S_RETURN: // "return"
      case symbol_kind::S_SHORTT: // "short"
      case symbol_kind::S_SIGNED: // "signed"
      case symbol_kind::S_SIZEOF: // "sizeof"
      case symbol_kind::S_STATIC: // "static"
      case symbol_kind::S_STRUCT: // "struct"
      case symbol_kind::S_SWITCH: // "switch"
      case symbol_kind::S_TYPEDEF: // "typedef"
      case symbol_kind::S_UNION: // "union"
      case symbol_kind::S_UNSIGNED: // "unsigned"
      case symbol_kind::S_VOIDT: // "void"
      case symbol_kind::S_VOLATILE: // "volatile"
      case symbol_kind::S_WHILE: // "while"
      case symbol_kind::S_BOOLT: // "bool"
      case symbol_kind::S_ELLIPSIS: // "ellipsis"
      case symbol_kind::S_QUESTION: // "question"
      case symbol_kind::S_SEMICOLON: // ";"
      case symbol_kind::S_OCURLY: // "{"
      case symbol_kind::S_CCURLY: // "}"
      case symbol_kind::S_COMMA: // ","
      case symbol_kind::S_COLON: // ":"
      case symbol_kind::S_OPAREN: // "("
      case symbol_kind::S_CPAREN: // ")"
      case symbol_kind::S_OBRACE: // "["
      case symbol_kind::S_CBRACE: // "]"
      case symbol_kind::S_PERIOD: // "."
      case symbol_kind::S_TILDE: // "~"
      case symbol_kind::S_EQUAL: // "="
      case symbol_kind::S_RIGHT_ASSIGN: // ">>="
      case symbol_kind::S_LEFT_ASSIGN: // "<<="
      case symbol_kind::S_ADD_ASSIGN: // "+="
      case symbol_kind::S_SUB_ASSIGN: // "-="
      case symbol_kind::S_MUL_ASSIGN: // "*="
      case symbol_kind::S_DIV_ASSIGN: // "/="
      case symbol_kind::S_MOD_ASSIGN: // "%="
      case symbol_kind::S_AND_ASSIGN: // "&="
      case symbol_kind::S_XOR_ASSIGN: // "^="
      case symbol_kind::S_OR_ASSIGN: // "|="
      case symbol_kind::S_RIGHT_OP: // ">>"
      case symbol_kind::S_LEFT_OP: // "<<"
      case symbol_kind::S_INC_OP: // "++"
      case symbol_kind::S_DEC_OP: // "--"
      case symbol_kind::S_PTR_OP: // "->"
      case symbol_kind::S_AND_OP: // "&&"
      case symbol_kind::S_OR_OP: // "||"
      case symbol_kind::S_GREATER_EQUAL: // ">="
      case symbol_kind::S_LESS_EQUAL: // "<="
      case symbol_kind::S_GREATER: // ">"
      case symbol_kind::S_LESS: // "<"
      case symbol_kind::S_EQUAL_EQUAL: // "=="
      case symbol_kind::S_NOT_EQUAL: // "!="
      case symbol_kind::S_NOT_OP: // "!"
      case symbol_kind::S_XOR_OP: // "^"
      case symbol_kind::S_BIT_AND: // "&"
      case symbol_kind::S_BIT_OR: // "|"
      case symbol_kind::S_MINUS_OP: // "-"
      case symbol_kind::S_PLUS_OP: // "+"
      case symbol_kind::S_TIMES_OP: // "*"
      case symbol_kind::S_DIV_OP: // "/"
      case symbol_kind::S_MOD_OP: // "%"
      case symbol_kind::S_unary_operator: // unary_operator
      case symbol_kind::S_assignment_operator: // assignment_operator
      case symbol_kind::S_struct_or_union: // struct_or_union
        yylhs.value.emplace< shared_ptr<CToken> > ();
        break;

      case symbol_kind::S_constant: // constant
        yylhs.value.emplace< shared_ptr<Constant> > ();
        break;

      case symbol_kind::S_declaration: // declaration
        yylhs.value.emplace< shared_ptr<Declaration> > ();
        break;

      case symbol_kind::S_declaration_specifiers: // declaration_specifiers
        yylhs.value.emplace< shared_ptr<DeclarationSpecifiers> > ();
        break;

      case symbol_kind::S_declarator: // declarator
        yylhs.value.emplace< shared_ptr<Declarator> > ();
        break;

      case symbol_kind::S_direct_abstract_declarator: // direct_abstract_declarator
        yylhs.value.emplace< shared_ptr<DirectAbstractDeclarator> > ();
        break;

      case symbol_kind::S_direct_declarator: // direct_declarator
        yylhs.value.emplace< shared_ptr<DirectDeclarator> > ();
        break;

      case symbol_kind::S_enum_specifier: // enum_specifier
        yylhs.value.emplace< shared_ptr<EnumSpecifier> > ();
        break;

      case symbol_kind::S_enumerator: // enumerator
        yylhs.value.emplace< shared_ptr<Enumerator> > ();
        break;

      case symbol_kind::S_primary_expression: // primary_expression
      case symbol_kind::S_postfix_expression: // postfix_expression
      case symbol_kind::S_unary_expression: // unary_expression
      case symbol_kind::S_cast_expression: // cast_expression
      case symbol_kind::S_multiplicative_expression: // multiplicative_expression
      case symbol_kind::S_additive_expression: // additive_expression
      case symbol_kind::S_shift_expression: // shift_expression
      case symbol_kind::S_relational_expression: // relational_expression
      case symbol_kind::S_equality_expression: // equality_expression
      case symbol_kind::S_and_expression: // and_expression
      case symbol_kind::S_exclusive_or_expression: // exclusive_or_expression
      case symbol_kind::S_inclusive_or_expression: // inclusive_or_expression
      case symbol_kind::S_logical_and_expression: // logical_and_expression
      case symbol_kind::S_logical_or_expression: // logical_or_expression
      case symbol_kind::S_conditional_expression: // conditional_expression
      case symbol_kind::S_assignment_expression: // assignment_expression
      case symbol_kind::S_expression: // expression
      case symbol_kind::S_constant_expression: // constant_expression
        yylhs.value.emplace< shared_ptr<ExpressionTree> > ();
        break;

      case symbol_kind::S_external_declaration: // external_declaration
        yylhs.value.emplace< shared_ptr<ExternalDeclaration> > ();
        break;

      case symbol_kind::S_function_definition: // function_definition
        yylhs.value.emplace< shared_ptr<FunctionDefinition> > ();
        break;

      case symbol_kind::S_init_declarator: // init_declarator
        yylhs.value.emplace< shared_ptr<InitDeclarator> > ();
        break;

      case symbol_kind::S_initializer: // initializer
        yylhs.value.emplace< shared_ptr<Initializer> > ();
        break;

      case symbol_kind::S_labeled_statement: // labeled_statement
        yylhs.value.emplace< shared_ptr<LabeledStatement> > ();
        break;

      case symbol_kind::S_parameter_declaration: // parameter_declaration
        yylhs.value.emplace< shared_ptr<ParameterDeclaration> > ();
        break;

      case symbol_kind::S_parameter_type_list: // parameter_type_list
        yylhs.value.emplace< shared_ptr<ParameterTypeList> > ();
        break;

      case symbol_kind::S_pointer: // pointer
        yylhs.value.emplace< shared_ptr<Pointer> > ();
        break;

      case symbol_kind::S_specifier_qualifier_list: // specifier_qualifier_list
        yylhs.value.emplace< shared_ptr<SpecifierQualifierList> > ();
        break;

      case symbol_kind::S_storage_class_specifier: // storage_class_specifier
        yylhs.value.emplace< shared_ptr<StorageClassSpecifier> > ();
        break;

      case symbol_kind::S_struct_declaration: // struct_declaration
        yylhs.value.emplace< shared_ptr<StructDeclaration> > ();
        break;

      case symbol_kind::S_struct_declarator: // struct_declarator
        yylhs.value.emplace< shared_ptr<StructDeclarator> > ();
        break;

      case symbol_kind::S_struct_or_union_specifier: // struct_or_union_specifier
        yylhs.value.emplace< shared_ptr<StructOrUnionSpecifier> > ();
        break;

      case symbol_kind::S_type_name: // type_name
        yylhs.value.emplace< shared_ptr<TypeName> > ();
        break;

      case symbol_kind::S_type_qualifier: // type_qualifier
        yylhs.value.emplace< shared_ptr<TypeQualifier> > ();
        break;

      case symbol_kind::S_type_specifier: // type_specifier
        yylhs.value.emplace< shared_ptr<TypeSpecifier> > ();
        break;

      case symbol_kind::S_statement_list: // statement_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<BaseStatement>>> > ();
        break;

      case symbol_kind::S_identifier_list: // identifier_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<CToken>>> > ();
        break;

      case symbol_kind::S_declaration_list: // declaration_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<Declaration>>> > ();
        break;

      case symbol_kind::S_enumerator_list: // enumerator_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<Enumerator>>> > ();
        break;

      case symbol_kind::S_argument_expression_list: // argument_expression_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<ExpressionTree>>> > ();
        break;

      case symbol_kind::S_init_declarator_list: // init_declarator_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<InitDeclarator>>> > ();
        break;

      case symbol_kind::S_initializer_list: // initializer_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<Initializer>>> > ();
        break;

      case symbol_kind::S_parameter_list: // parameter_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > ();
        break;

      case symbol_kind::S_struct_declaration_list: // struct_declaration_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<StructDeclaration>>> > ();
        break;

      case symbol_kind::S_struct_declarator_list: // struct_declarator_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<StructDeclarator>>> > ();
        break;

      case symbol_kind::S_type_qualifier_list: // type_qualifier_list
        yylhs.value.emplace< shared_ptr<vector<shared_ptr<TypeQualifier>>> > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // primary_expression: "identifier"
#line 258 "ansic.y"
                                { yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<ExpressionTree> > () = createPrimaryExpression(yystack_[0].value.as < shared_ptr<CToken> > (),NULL);      cout << "IDENTIFIER REDUCE to primary_expression" << endl; }
#line 1893 "parser.cpp"
    break;

  case 3: // primary_expression: constant
#line 259 "ansic.y"
                                { yylhs.value.as < shared_ptr<ExpressionTree> > () = createPrimaryExpression(nullptr,yystack_[0].value.as < shared_ptr<Constant> > ());   cout << "constant REDUCE to primary_expression" << endl; }
#line 1899 "parser.cpp"
    break;

  case 4: // primary_expression: "(" expression ")"
#line 260 "ansic.y"
                                { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[1].value.as < shared_ptr<ExpressionTree> > ();                                    cout << "OPAREN expression CPAREN REDUCE to primary_expression" << endl; }
#line 1905 "parser.cpp"
    break;

  case 5: // constant: "f_const"
#line 264 "ansic.y"
                      { yylhs.value.as < shared_ptr<Constant> > () = createConstant(nullptr,yystack_[0].value.as < shared_ptr<CToken> > (),nullptr,FLOAT_CONSTANT);   cout << "F_CONST REDUCE to constant " << endl; }
#line 1911 "parser.cpp"
    break;

  case 6: // constant: "i_const"
#line 265 "ansic.y"
                      { yylhs.value.as < shared_ptr<Constant> > () = createConstant(yystack_[0].value.as < shared_ptr<CToken> > (),nullptr,nullptr,INTEGER_CONSTANT); cout << "I_CONST REDUCE to constant " << endl; }
#line 1917 "parser.cpp"
    break;

  case 7: // constant: "sting_literal"
#line 266 "ansic.y"
                      { yylhs.value.as < shared_ptr<Constant> > () = createConstant(nullptr,nullptr,yystack_[0].value.as < shared_ptr<CToken> > (),STRING_CONSTANT);  cout << "STRING_LITERAL REDUCE to constant  " << endl; }
#line 1923 "parser.cpp"
    break;

  case 8: // postfix_expression: primary_expression
#line 269 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "primary_expression REDUCE to postfix_expression" << endl; }
#line 1929 "parser.cpp"
    break;

  case 9: // postfix_expression: postfix_expression "[" expression "]"
#line 270 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_ARRAY,yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),yystack_[1].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,  yystack_[3].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr); cout << "postfix_expression OBRACE expression CBRACE REDUCE to postfix_expression" << endl; }
#line 1935 "parser.cpp"
    break;

  case 10: // postfix_expression: postfix_expression "(" ")"
#line 271 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_FUNCTION_CALL,yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr); cout << "postfix_expression OPAREN CPAREN REDUCE to postfix_expression" << endl; }
#line 1941 "parser.cpp"
    break;

  case 11: // postfix_expression: postfix_expression "(" argument_expression_list ")"
#line 272 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_FUNCTION_CALL,yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<vector<shared_ptr<ExpressionTree>>> > (),nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[3].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr); cout << "postfix_expression OPAREN argument_expression_list CPAREN REDUCE to postfix_expression" << endl; }
#line 1947 "parser.cpp"
    break;

  case 12: // postfix_expression: postfix_expression "." "identifier"
#line 273 "ansic.y"
                                                                   { yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_VAR_ACCESS,yystack_[1].value.as < shared_ptr<CToken> > (),nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<CToken> > (),nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr); cout << "postfix_expression PERIOD_OP IDENTIFIER REDUCE to postfix_expression" << endl; }
#line 1953 "parser.cpp"
    break;

  case 13: // postfix_expression: postfix_expression "->" "identifier"
#line 274 "ansic.y"
                                                                   { yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_VAR_ACCESS,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<CToken> > (),nullptr,nullptr,yystack_[1].value.as < shared_ptr<CToken> > (),nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr); cout << "postfix_expression PTR_OP IDENTIFIER REDUCE to postfix_expression" << endl; }
#line 1959 "parser.cpp"
    break;

  case 14: // postfix_expression: postfix_expression "++"
#line 275 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_INC,nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<CToken> > (),nullptr, nullptr,nullptr,nullptr); cout << "postfix_expression INC_OP REDUCE to postfix_expression" << endl; }
#line 1965 "parser.cpp"
    break;

  case 15: // postfix_expression: postfix_expression "--"
#line 276 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_DEC,nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<CToken> > (),nullptr, nullptr,nullptr,nullptr); cout << "postfix_expression DEC_OP REDUCE to postfix_expression" << endl; }
#line 1971 "parser.cpp"
    break;

  case 16: // postfix_expression: "(" type_name ")" "{" initializer_list "}"
#line 277 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_TYPECAST,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<vector<shared_ptr<Initializer>>> > (),yystack_[4].value.as < shared_ptr<TypeName> > (),nullptr,nullptr, nullptr,nullptr,nullptr); cout << "OPAREN type_name CPAREN_OP OCURLY_OP initializer_list CCURLY REDUCE to postfix_expression" << endl; }
#line 1977 "parser.cpp"
    break;

  case 17: // postfix_expression: "(" type_name ")" "{" initializer_list "," "}"
#line 278 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_TYPECAST,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[2].value.as < shared_ptr<vector<shared_ptr<Initializer>>> > (),yystack_[5].value.as < shared_ptr<TypeName> > (),nullptr,nullptr, nullptr,nullptr,nullptr); cout << "OPAREN type_name CPAREN_OP OCURLY_OP initializer_list COMMA CCURLY REDUCE to postfix_expression" << endl; }
#line 1983 "parser.cpp"
    break;

  case 18: // argument_expression_list: assignment_expression
#line 282 "ansic.y"
                                                           { yylhs.value.as < shared_ptr<vector<shared_ptr<ExpressionTree>>> > () = createArgumentExpressionList(yystack_[0].value.as < shared_ptr<ExpressionTree> > (),nullptr); cout << "assignment_expression REDUCE argument_expression_list" << endl; }
#line 1989 "parser.cpp"
    break;

  case 19: // argument_expression_list: argument_expression_list "," assignment_expression
#line 283 "ansic.y"
                                                           { yylhs.value.as < shared_ptr<vector<shared_ptr<ExpressionTree>>> > () = createArgumentExpressionList(yystack_[0].value.as < shared_ptr<ExpressionTree> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<ExpressionTree>>> > ()); cout << "argument_expression_list COMMA assignment_expression REDUCE argument_expression_list" << endl; }
#line 1995 "parser.cpp"
    break;

  case 20: // unary_expression: postfix_expression
#line 287 "ansic.y"
                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > (); cout << "postfix_expression REDUCE unary_expression" << endl;}
#line 2001 "parser.cpp"
    break;

  case 21: // unary_expression: "++" unary_expression
#line 288 "ansic.y"
                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_INC,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<CToken> > (),nullptr, nullptr,nullptr,nullptr); cout << "INC_OP unary_expression REDUCE unary_expression" << endl;}
#line 2007 "parser.cpp"
    break;

  case 22: // unary_expression: "--" unary_expression
#line 289 "ansic.y"
                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_DEC,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<CToken> > (),nullptr, nullptr,nullptr,nullptr); cout << "DEC_OP unary_expression REDUCE unary_expression" << endl;}
#line 2013 "parser.cpp"
    break;

  case 23: // unary_expression: unary_operator cast_expression
#line 290 "ansic.y"
                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_UNARY,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<CToken> > (),nullptr, nullptr,nullptr,nullptr); cout << "unary_operator cast_expression REDUCE unary_expression" << endl;}
#line 2019 "parser.cpp"
    break;

  case 24: // unary_expression: "sizeof" unary_expression
#line 291 "ansic.y"
                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_SIZEOF,yystack_[1].value.as < shared_ptr<CToken> > (),nullptr,nullptr,yystack_[0].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, nullptr,nullptr,nullptr); cout << "SIZEOF unary_expression REDUCE unary_expression" << endl;}
#line 2025 "parser.cpp"
    break;

  case 25: // unary_expression: "sizeof" "(" type_name ")"
#line 292 "ansic.y"
                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_SIZEOF,yystack_[3].value.as < shared_ptr<CToken> > (),nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,yystack_[1].value.as < shared_ptr<TypeName> > (),nullptr,nullptr, nullptr,nullptr,nullptr); cout << "SIZEOF OPAREN type_name CPAREN REDUCE unary_expression" << endl;}
#line 2031 "parser.cpp"
    break;

  case 26: // unary_operator: "&"
#line 296 "ansic.y"
                { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "BIT_AND REDUCE to unary_operator" << endl;}
#line 2037 "parser.cpp"
    break;

  case 27: // unary_operator: "*"
#line 297 "ansic.y"
                { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "TIMES_OP REDUCE to unary_operator" << endl;}
#line 2043 "parser.cpp"
    break;

  case 28: // unary_operator: "+"
#line 298 "ansic.y"
                { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "PLUS_OP REDUCE to unary_operator" << endl;}
#line 2049 "parser.cpp"
    break;

  case 29: // unary_operator: "-"
#line 299 "ansic.y"
                { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "MINUS_OP REDUCE to unary_operator" << endl;}
#line 2055 "parser.cpp"
    break;

  case 30: // unary_operator: "~"
#line 300 "ansic.y"
                { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "TILDE REDUCE to unary_operator" << endl;}
#line 2061 "parser.cpp"
    break;

  case 31: // unary_operator: "!"
#line 301 "ansic.y"
                { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "NOT_OP REDUCE to unary_operator" << endl;}
#line 2067 "parser.cpp"
    break;

  case 32: // cast_expression: unary_expression
#line 305 "ansic.y"
                                               { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "unary_expression REDUCE to cast_expression" << endl;}
#line 2073 "parser.cpp"
    break;

  case 33: // cast_expression: "(" type_name ")" cast_expression
#line 306 "ansic.y"
                                               { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_TYPECAST,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,yystack_[2].value.as < shared_ptr<TypeName> > (),nullptr,nullptr, nullptr,nullptr,nullptr);  cout << "unary_expression REDUCE to cast_expression" << endl;}
#line 2079 "parser.cpp"
    break;

  case 34: // multiplicative_expression: cast_expression
#line 310 "ansic.y"
                                                         { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "cast_expression REDUCE to multiplicative_expression" << endl;}
#line 2085 "parser.cpp"
    break;

  case 35: // multiplicative_expression: multiplicative_expression "*" cast_expression
#line 311 "ansic.y"
                                                         { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "multiplicative_expression TIMES_OP cast_expression REDUCE to multiplicative_expression" << endl;}
#line 2091 "parser.cpp"
    break;

  case 36: // multiplicative_expression: multiplicative_expression "/" cast_expression
#line 312 "ansic.y"
                                                         { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "multiplicative_expression DIV_OP cast_expression REDUCE to multiplicative_expression" << endl;}
#line 2097 "parser.cpp"
    break;

  case 37: // multiplicative_expression: multiplicative_expression "%" cast_expression
#line 313 "ansic.y"
                                                         { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "multiplicative_expression MOD_OP cast_expression REDUCE to multiplicative_expression" << endl;}
#line 2103 "parser.cpp"
    break;

  case 38: // additive_expression: multiplicative_expression
#line 317 "ansic.y"
                                                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "multiplicative_expression REDUCE to additive_expression" << endl;}
#line 2109 "parser.cpp"
    break;

  case 39: // additive_expression: additive_expression "+" multiplicative_expression
#line 318 "ansic.y"
                                                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "additive_expression REDUCE to multiplicative_expression" << endl;}
#line 2115 "parser.cpp"
    break;

  case 40: // additive_expression: additive_expression "-" multiplicative_expression
#line 319 "ansic.y"
                                                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "additive_expression REDUCE to multiplicative_expression" << endl;}
#line 2121 "parser.cpp"
    break;

  case 41: // shift_expression: additive_expression
#line 323 "ansic.y"
                                                    { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "additive_expression REDUCE to shift_expression" << endl;}
#line 2127 "parser.cpp"
    break;

  case 42: // shift_expression: shift_expression "<<" additive_expression
#line 324 "ansic.y"
                                                    { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ());; cout << "shift_expression LEFT_OP additive_expression REDUCE to shift_expression" << endl;}
#line 2133 "parser.cpp"
    break;

  case 43: // shift_expression: shift_expression ">>" additive_expression
#line 325 "ansic.y"
                                                    { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ());; cout << "shift_expression RIGHT_OP additive_expression REDUCE to shift_expression" << endl;}
#line 2139 "parser.cpp"
    break;

  case 44: // relational_expression: shift_expression
#line 329 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "shift_expression REDUCE to relational_expression" << endl;}
#line 2145 "parser.cpp"
    break;

  case 45: // relational_expression: relational_expression "<" shift_expression
#line 330 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "relational_expression LESS shift_expression REDUCE to shift_expression" << endl;}
#line 2151 "parser.cpp"
    break;

  case 46: // relational_expression: relational_expression ">" shift_expression
#line 331 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "relational_expression GREATER shift_expression REDUCE to shift_expression" << endl;}
#line 2157 "parser.cpp"
    break;

  case 47: // relational_expression: relational_expression "<=" shift_expression
#line 332 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "relational_expression LESS_EQUAL shift_expression REDUCE to shift_expression" << endl;}
#line 2163 "parser.cpp"
    break;

  case 48: // relational_expression: relational_expression ">=" shift_expression
#line 333 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "relational_expression GREATER_EQUAL shift_expression REDUCE to shift_expression" << endl;}
#line 2169 "parser.cpp"
    break;

  case 49: // equality_expression: relational_expression
#line 337 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "relational_expression REDUCE to equality_expression" << endl;}
#line 2175 "parser.cpp"
    break;

  case 50: // equality_expression: equality_expression "==" relational_expression
#line 338 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "equality_expression EQUAL_EQUAL relational_expression REDUCE to equality_expression" << endl;}
#line 2181 "parser.cpp"
    break;

  case 51: // equality_expression: equality_expression "!=" relational_expression
#line 339 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "equality_expression NOT_EQUAL relational_expression REDUCE to equality_expression" << endl;}
#line 2187 "parser.cpp"
    break;

  case 52: // and_expression: equality_expression
#line 343 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "equality_expression REDUCE to and_expression" << endl;}
#line 2193 "parser.cpp"
    break;

  case 53: // and_expression: and_expression "&" equality_expression
#line 344 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "and_expression BIT_AND equality_expression REDUCE to and_expression" << endl;}
#line 2199 "parser.cpp"
    break;

  case 54: // exclusive_or_expression: and_expression
#line 348 "ansic.y"
                                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "and_expression REDUCE to exclusive_or_expression" << endl;}
#line 2205 "parser.cpp"
    break;

  case 55: // exclusive_or_expression: exclusive_or_expression "^" and_expression
#line 349 "ansic.y"
                                                     { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "exclusive_or_expression XOR_OP and_expression REDUCE to exclusive_or_expression" << endl;}
#line 2211 "parser.cpp"
    break;

  case 56: // inclusive_or_expression: exclusive_or_expression
#line 353 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "exclusive_or_expression REDUCE to inclusive_or_expression" << endl;}
#line 2217 "parser.cpp"
    break;

  case 57: // inclusive_or_expression: inclusive_or_expression "|" exclusive_or_expression
#line 354 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "inclusive_or_expression BIT_OR exclusive_or_expression REDUCE to inclusive_or_expression" << endl;}
#line 2223 "parser.cpp"
    break;

  case 58: // logical_and_expression: inclusive_or_expression
#line 358 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "inclusive_or_expression REDUCE to logical_and_expression" << endl;}
#line 2229 "parser.cpp"
    break;

  case 59: // logical_and_expression: logical_and_expression "&&" inclusive_or_expression
#line 359 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "inclusive_or_expression REDUCE to logical_and_expression" << endl;}
#line 2235 "parser.cpp"
    break;

  case 60: // logical_or_expression: logical_and_expression
#line 363 "ansic.y"
                                                          { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "inclusive_and_expression REDUCE to logical_or_expression" << endl;}
#line 2241 "parser.cpp"
    break;

  case 61: // logical_or_expression: logical_or_expression "||" logical_and_expression
#line 364 "ansic.y"
                                                          { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "logical_or_expression OR_OP logical_and_expression REDUCE to logical_or_expression" << endl;}
#line 2247 "parser.cpp"
    break;

  case 62: // conditional_expression: logical_or_expression
#line 368 "ansic.y"
                                                                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "logical_or_expression REDUCE to conditional_expression" << endl;}
#line 2253 "parser.cpp"
    break;

  case 63: // conditional_expression: logical_or_expression "question" expression ":" conditional_expression
#line 369 "ansic.y"
                                                                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_QUESTION,nullptr,nullptr,yystack_[4].value.as < shared_ptr<ExpressionTree> > (),yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > (),nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,  nullptr,nullptr,nullptr); cout << "logical_or_expression QUESTION expression COLON conditional_expression REDUCE to conditional_expression" << endl;}
#line 2259 "parser.cpp"
    break;

  case 64: // assignment_expression: conditional_expression
#line 373 "ansic.y"
                                                                  { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "conditional_expression REDUCE to assignment_expression" << endl;}
#line 2265 "parser.cpp"
    break;

  case 65: // assignment_expression: unary_expression assignment_operator assignment_expression
#line 374 "ansic.y"
                                                                  { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "unary_expression assignment_operator assignment_expression REDUCE to assignment_expression" << endl;}
#line 2271 "parser.cpp"
    break;

  case 66: // assignment_operator: "="
#line 378 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "EQUAL_OP REDUCE to assignment_operator" << endl;}
#line 2277 "parser.cpp"
    break;

  case 67: // assignment_operator: "*="
#line 379 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "MUL_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2283 "parser.cpp"
    break;

  case 68: // assignment_operator: "/="
#line 380 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "DIV_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2289 "parser.cpp"
    break;

  case 69: // assignment_operator: "%="
#line 381 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "MOG_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2295 "parser.cpp"
    break;

  case 70: // assignment_operator: "+="
#line 382 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "ADD_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2301 "parser.cpp"
    break;

  case 71: // assignment_operator: "-="
#line 383 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "SUB_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2307 "parser.cpp"
    break;

  case 72: // assignment_operator: "<<="
#line 384 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "LEFT_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2313 "parser.cpp"
    break;

  case 73: // assignment_operator: ">>="
#line 385 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "RIGHT_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2319 "parser.cpp"
    break;

  case 74: // assignment_operator: "&="
#line 386 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "AND_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2325 "parser.cpp"
    break;

  case 75: // assignment_operator: "^="
#line 387 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "XOR_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2331 "parser.cpp"
    break;

  case 76: // assignment_operator: "|="
#line 388 "ansic.y"
                     { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > ();  cout << "OR_ASSIGN REDUCE to assignment_operator" << endl;}
#line 2337 "parser.cpp"
    break;

  case 77: // expression: assignment_expression
#line 392 "ansic.y"
                                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "asignment_expression REDUCE to expression" << endl;}
#line 2343 "parser.cpp"
    break;

  case 78: // expression: expression "," assignment_expression
#line 393 "ansic.y"
                                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = createExpression(NT_OP,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr, yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "expression COMMA assignment_expression COMMA  REDUCE to expression" << endl;}
#line 2349 "parser.cpp"
    break;

  case 79: // constant_expression: conditional_expression
#line 397 "ansic.y"
                              { yylhs.value.as < shared_ptr<ExpressionTree> > () = yystack_[0].value.as < shared_ptr<ExpressionTree> > ();  cout << "conditional_expression REDUCE to constant_expression" << endl;}
#line 2355 "parser.cpp"
    break;

  case 80: // declaration: declaration_specifiers ";"
#line 401 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<Declaration> > () = createDeclaration(yystack_[1].value.as < shared_ptr<DeclarationSpecifiers> > (),nullptr); cout << "declaration_specifiers SEMICOLON REDUCE to declaration" << endl;}
#line 2361 "parser.cpp"
    break;

  case 81: // declaration: declaration_specifiers init_declarator_list ";"
#line 402 "ansic.y"
                                                             { yylhs.value.as < shared_ptr<Declaration> > () = createDeclaration(yystack_[2].value.as < shared_ptr<DeclarationSpecifiers> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<InitDeclarator>>> > ()); cout << "declaration_specifiers init_declarator_list SEMICOLON REDUCE to declaration" << endl;}
#line 2367 "parser.cpp"
    break;

  case 82: // declaration_specifiers: storage_class_specifier
#line 406 "ansic.y"
                                                      { yylhs.value.as < shared_ptr<DeclarationSpecifiers> > () = createDeclarationSpecifiers(nullptr,nullptr,yystack_[0].value.as < shared_ptr<StorageClassSpecifier> > (),nullptr,nullptr,nullptr); cout << "storage_class_specifier REDUCE to declaration_specifiers" << endl;}
#line 2373 "parser.cpp"
    break;

  case 83: // declaration_specifiers: storage_class_specifier declaration_specifiers
#line 407 "ansic.y"
                                                      { yylhs.value.as < shared_ptr<DeclarationSpecifiers> > () = createDeclarationSpecifiers(nullptr,yystack_[0].value.as < shared_ptr<DeclarationSpecifiers> > (),yystack_[1].value.as < shared_ptr<StorageClassSpecifier> > (),nullptr,nullptr,nullptr);      cout << "storage_class_specifier declaration_specifiers REDUCE to declaration_specifiers" << endl;}
#line 2379 "parser.cpp"
    break;

  case 84: // declaration_specifiers: type_specifier
#line 408 "ansic.y"
                                                      { yylhs.value.as < shared_ptr<DeclarationSpecifiers> > () = createDeclarationSpecifiers(nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<TypeSpecifier> > (),nullptr,nullptr); cout << "type_specifier REDUCE to declaration_specifiers" << endl;}
#line 2385 "parser.cpp"
    break;

  case 85: // declaration_specifiers: type_specifier declaration_specifiers
#line 409 "ansic.y"
                                                      { yylhs.value.as < shared_ptr<DeclarationSpecifiers> > () = createDeclarationSpecifiers(nullptr,yystack_[0].value.as < shared_ptr<DeclarationSpecifiers> > (),nullptr,yystack_[1].value.as < shared_ptr<TypeSpecifier> > (),nullptr,nullptr);      cout << "type_specifier declaration_specifiers REDUCE to declaration_specifiers" << endl;}
#line 2391 "parser.cpp"
    break;

  case 86: // declaration_specifiers: type_qualifier
#line 410 "ansic.y"
                                                      { yylhs.value.as < shared_ptr<DeclarationSpecifiers> > () = createDeclarationSpecifiers(nullptr,nullptr,nullptr,nullptr,yystack_[0].value.as < shared_ptr<TypeQualifier> > (),nullptr); cout << "type_qualifier REDUCE to declaration_specifiers" << endl;}
#line 2397 "parser.cpp"
    break;

  case 87: // declaration_specifiers: type_qualifier declaration_specifiers
#line 411 "ansic.y"
                                                      { yylhs.value.as < shared_ptr<DeclarationSpecifiers> > () = createDeclarationSpecifiers(nullptr,yystack_[0].value.as < shared_ptr<DeclarationSpecifiers> > (),nullptr,nullptr,yystack_[1].value.as < shared_ptr<TypeQualifier> > (),nullptr);      cout << "type_qualifier declaration_specifiers REDUCE to declaration_specifiers" << endl;}
#line 2403 "parser.cpp"
    break;

  case 88: // init_declarator_list: init_declarator
#line 415 "ansic.y"
                                                 { yylhs.value.as < shared_ptr<vector<shared_ptr<InitDeclarator>>> > () = createInitDeclaratorList(yystack_[0].value.as < shared_ptr<InitDeclarator> > (),nullptr); cout << "init_declarator REDUCE to init_declarator_list" << endl;}
#line 2409 "parser.cpp"
    break;

  case 89: // init_declarator_list: init_declarator_list "," init_declarator
#line 416 "ansic.y"
                                                 { yylhs.value.as < shared_ptr<vector<shared_ptr<InitDeclarator>>> > () = createInitDeclaratorList(yystack_[0].value.as < shared_ptr<InitDeclarator> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<InitDeclarator>>> > ()); cout << "init_declarator_list COMMA init_declarator REDUCE to init_declarator_list" << endl;}
#line 2415 "parser.cpp"
    break;

  case 90: // init_declarator: declarator
#line 420 "ansic.y"
                                    { yylhs.value.as < shared_ptr<InitDeclarator> > () = createInitDeclarator(yystack_[0].value.as < shared_ptr<Declarator> > (),nullptr); cout << "declarator REDUCE to init_declarator" << endl;}
#line 2421 "parser.cpp"
    break;

  case 91: // init_declarator: declarator "=" initializer
#line 421 "ansic.y"
                                    { yylhs.value.as < shared_ptr<InitDeclarator> > () = createInitDeclarator(yystack_[2].value.as < shared_ptr<Declarator> > (),yystack_[0].value.as < shared_ptr<Initializer> > ()); cout << "declarator EQUAL initializer REDUCE to init_declarator" << endl;}
#line 2427 "parser.cpp"
    break;

  case 92: // storage_class_specifier: "typedef"
#line 425 "ansic.y"
                { yylhs.value.as < shared_ptr<StorageClassSpecifier> > () = createStorageClassSpecifier(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "TYPEDEF REDUCE to storage_class_specifier" << endl;}
#line 2433 "parser.cpp"
    break;

  case 93: // storage_class_specifier: "extern"
#line 426 "ansic.y"
                { yylhs.value.as < shared_ptr<StorageClassSpecifier> > () = createStorageClassSpecifier(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "EXTERN REDUCE to storage_class_specifier" << endl;}
#line 2439 "parser.cpp"
    break;

  case 94: // storage_class_specifier: "static"
#line 427 "ansic.y"
                { yylhs.value.as < shared_ptr<StorageClassSpecifier> > () = createStorageClassSpecifier(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "STATIC REDUCE to storage_class_specifier" << endl;}
#line 2445 "parser.cpp"
    break;

  case 95: // storage_class_specifier: "auto"
#line 428 "ansic.y"
                { yylhs.value.as < shared_ptr<StorageClassSpecifier> > () = createStorageClassSpecifier(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "AUTO REDUCE to storage_class_specifier" << endl;}
#line 2451 "parser.cpp"
    break;

  case 96: // storage_class_specifier: "register"
#line 429 "ansic.y"
                { yylhs.value.as < shared_ptr<StorageClassSpecifier> > () = createStorageClassSpecifier(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "REGISTER REDUCE to storage_class_specifier" << endl;}
#line 2457 "parser.cpp"
    break;

  case 97: // type_specifier: "void"
#line 433 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(VOIDT, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "VOIDT REDUCE to type_specifier" << endl;}
#line 2463 "parser.cpp"
    break;

  case 98: // type_specifier: "char"
#line 434 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(CHAR, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "CHAR REDUCE to type_specifier" << endl;}
#line 2469 "parser.cpp"
    break;

  case 99: // type_specifier: "short"
#line 435 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(SHORTT, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "SHORT REDUCE to type_specifier" << endl;}
#line 2475 "parser.cpp"
    break;

  case 100: // type_specifier: INT
#line 436 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(INT, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "INT REDUCE to type_specifier" << endl;}
#line 2481 "parser.cpp"
    break;

  case 101: // type_specifier: "long"
#line 437 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(LONG, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "LONG REDUCE to type_specifier" << endl;}
#line 2487 "parser.cpp"
    break;

  case 102: // type_specifier: "long long"
#line 438 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(LONG_LONG, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "LONG_LONG REDUCE to type_specifier" << endl;}
#line 2493 "parser.cpp"
    break;

  case 103: // type_specifier: "float"
#line 439 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(FLOAT, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "FLOAT REDUCE to type_specifier" << endl;}
#line 2499 "parser.cpp"
    break;

  case 104: // type_specifier: "double"
#line 440 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(DOUBLE, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "DOUBLE REDUCE to type_specifier" << endl;}
#line 2505 "parser.cpp"
    break;

  case 105: // type_specifier: "long double"
#line 441 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(LONG_DOUBLE, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "LONG_DOUBLE REDUCE to type_specifier" << endl;}
#line 2511 "parser.cpp"
    break;

  case 106: // type_specifier: "bool"
#line 442 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(BOOLT, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "BOOL REDUCE to type_specifier" << endl;}
#line 2517 "parser.cpp"
    break;

  case 107: // type_specifier: "signed"
#line 443 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(SIGNED, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "SIGNED REDUCE to type_specifier" << endl;}
#line 2523 "parser.cpp"
    break;

  case 108: // type_specifier: "unsigned"
#line 444 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(UNSIGNED, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "UNIGNED REDUCE to type_specifier" << endl;}
#line 2529 "parser.cpp"
    break;

  case 109: // type_specifier: struct_or_union_specifier
#line 445 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(UNKNOWN, nullptr, yystack_[0].value.as < shared_ptr<StructOrUnionSpecifier> > (), nullptr, nullptr); cout << "struct_or_union_specifier REDUCE to type_specifier" << endl;}
#line 2535 "parser.cpp"
    break;

  case 110: // type_specifier: enum_specifier
#line 446 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(ENUM, nullptr, nullptr, yystack_[0].value.as < shared_ptr<EnumSpecifier> > (), nullptr); cout << "enum_specifier REDUCE to type_specifier" << endl;}
#line 2541 "parser.cpp"
    break;

  case 111: // type_specifier: "type name"
#line 447 "ansic.y"
                                { yylhs.value.as < shared_ptr<TypeSpecifier> > () = make_shared<TypeSpecifier>(TYPE_NAME, yystack_[0].value.as < shared_ptr<CToken> > (), nullptr, nullptr, nullptr); cout << "TYPE_NAME REDUCE to type_specifier" << endl;}
#line 2547 "parser.cpp"
    break;

  case 112: // struct_or_union_specifier: struct_or_union "identifier" "{" struct_declaration_list "}"
#line 451 "ansic.y"
                                                                         { yylhs.value.as < shared_ptr<StructOrUnionSpecifier> > () = make_shared<StructOrUnionSpecifier>(yystack_[4].value.as < shared_ptr<CToken> > (),yystack_[3].value.as < shared_ptr<CToken> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<StructDeclaration>>> > ()); cout << "struct_or_union IDENTIFIER OCURLY struct_declaration_list CCURLY REDUCE to struct_or_union_specifier" << endl;}
#line 2553 "parser.cpp"
    break;

  case 113: // struct_or_union_specifier: struct_or_union "{" struct_declaration_list "}"
#line 452 "ansic.y"
                                                                         { yylhs.value.as < shared_ptr<StructOrUnionSpecifier> > () = make_shared<StructOrUnionSpecifier>(yystack_[3].value.as < shared_ptr<CToken> > (),nullptr,yystack_[1].value.as < shared_ptr<vector<shared_ptr<StructDeclaration>>> > ());  cout << "struct_or_union OCURLY struct_declaration_list CCURLY REDUCE to struct_or_union_specifier" << endl;}
#line 2559 "parser.cpp"
    break;

  case 114: // struct_or_union_specifier: struct_or_union "identifier"
#line 453 "ansic.y"
                                                                         { yylhs.value.as < shared_ptr<StructOrUnionSpecifier> > () = make_shared<StructOrUnionSpecifier>(yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),nullptr); cout << "struct_or_union IDENTIFIER REDUCE to struct_or_union_specifier" << endl;}
#line 2565 "parser.cpp"
    break;

  case 115: // struct_or_union: "struct"
#line 457 "ansic.y"
               { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "STRUCT REDUCE to struct_or_union" << endl;}
#line 2571 "parser.cpp"
    break;

  case 116: // struct_or_union: "union"
#line 458 "ansic.y"
               { yylhs.value.as < shared_ptr<CToken> > () = yystack_[0].value.as < shared_ptr<CToken> > (); cout << "UNION REDUCE to struct_or_union" << endl;}
#line 2577 "parser.cpp"
    break;

  case 117: // struct_declaration_list: struct_declaration
#line 462 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<vector<shared_ptr<StructDeclaration>>> > () = createStructDeclarationList(yystack_[0].value.as < shared_ptr<StructDeclaration> > (),nullptr); cout << "struct_declaration REDUCE to struct_declaration_list" << endl;}
#line 2583 "parser.cpp"
    break;

  case 118: // struct_declaration_list: struct_declaration_list struct_declaration
#line 463 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<vector<shared_ptr<StructDeclaration>>> > () = createStructDeclarationList(yystack_[0].value.as < shared_ptr<StructDeclaration> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<StructDeclaration>>> > ()); cout << "struct_declaration_list struct_declaration REDUCE to struct_declaration_list" << endl;}
#line 2589 "parser.cpp"
    break;

  case 119: // struct_declaration: specifier_qualifier_list struct_declarator_list ";"
#line 467 "ansic.y"
                                                                { yylhs.value.as < shared_ptr<StructDeclaration> > () = make_shared<StructDeclaration>(yystack_[2].value.as < shared_ptr<SpecifierQualifierList> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<StructDeclarator>>> > ()); cout << "specifier_qualifier_list struct_declarator_list SEMICOLON REDUCE to struct_declaration" << endl;}
#line 2595 "parser.cpp"
    break;

  case 120: // specifier_qualifier_list: type_specifier specifier_qualifier_list
#line 471 "ansic.y"
                                              { yylhs.value.as < shared_ptr<SpecifierQualifierList> > () = make_shared<SpecifierQualifierList>(yystack_[0].value.as < shared_ptr<SpecifierQualifierList> > (),yystack_[1].value.as < shared_ptr<TypeSpecifier> > ()); cout << "type_specifier specifier_qualifier_list REDUCE to specifier_qualifier_list" << endl;}
#line 2601 "parser.cpp"
    break;

  case 121: // specifier_qualifier_list: type_specifier
#line 472 "ansic.y"
                                              { yylhs.value.as < shared_ptr<SpecifierQualifierList> > () = make_shared<SpecifierQualifierList>(yystack_[0].value.as < shared_ptr<TypeSpecifier> > ()); cout << "type_specifier REDUCE to specifier_qualifier_list" << endl;}
#line 2607 "parser.cpp"
    break;

  case 122: // specifier_qualifier_list: type_qualifier specifier_qualifier_list
#line 473 "ansic.y"
                                              { yylhs.value.as < shared_ptr<SpecifierQualifierList> > () = make_shared<SpecifierQualifierList>(yystack_[0].value.as < shared_ptr<SpecifierQualifierList> > (),yystack_[1].value.as < shared_ptr<TypeQualifier> > ()); cout << "type_qualifier specifier_qualifier_list REDUCE to specifier_qualifier_list" << endl;}
#line 2613 "parser.cpp"
    break;

  case 123: // specifier_qualifier_list: type_qualifier
#line 474 "ansic.y"
                                              { yylhs.value.as < shared_ptr<SpecifierQualifierList> > () = make_shared<SpecifierQualifierList>(yystack_[0].value.as < shared_ptr<TypeQualifier> > ()); cout << "type_qualifier REDUCE to specifier_qualifier_list" << endl;}
#line 2619 "parser.cpp"
    break;

  case 124: // struct_declarator_list: struct_declarator
#line 478 "ansic.y"
                                                     { yylhs.value.as < shared_ptr<vector<shared_ptr<StructDeclarator>>> > () = createStructDeclaratorList(yystack_[0].value.as < shared_ptr<StructDeclarator> > (),nullptr); cout << "struct_declarator REDUCE to struct_declarator_list" << endl; }
#line 2625 "parser.cpp"
    break;

  case 125: // struct_declarator_list: struct_declarator_list "," struct_declarator
#line 479 "ansic.y"
                                                     { yylhs.value.as < shared_ptr<vector<shared_ptr<StructDeclarator>>> > () = createStructDeclaratorList(yystack_[0].value.as < shared_ptr<StructDeclarator> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<StructDeclarator>>> > ()); cout << "struct_declarator_list COMMA struct_declarator REDUCE to struct_declarator_list" << endl; }
#line 2631 "parser.cpp"
    break;

  case 126: // struct_declarator: declarator
#line 483 "ansic.y"
                                           { yylhs.value.as < shared_ptr<StructDeclarator> > () = make_shared<StructDeclarator>(yystack_[0].value.as < shared_ptr<Declarator> > ()); cout << "declarator REDUCE to struct_declarator" << endl;}
#line 2637 "parser.cpp"
    break;

  case 127: // struct_declarator: ":" constant_expression
#line 484 "ansic.y"
                                           { yylhs.value.as < shared_ptr<StructDeclarator> > () = make_shared<StructDeclarator>(yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "COLON constant_expression REDUCE to struct_declarator" << endl;}
#line 2643 "parser.cpp"
    break;

  case 128: // struct_declarator: declarator ":" constant_expression
#line 485 "ansic.y"
                                           { yylhs.value.as < shared_ptr<StructDeclarator> > () = make_shared<StructDeclarator>(yystack_[2].value.as < shared_ptr<Declarator> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "declarator COLON constant_expression REDUCE to struct_declarator" << endl;}
#line 2649 "parser.cpp"
    break;

  case 129: // enum_specifier: "enum" "{" enumerator_list "}"
#line 489 "ansic.y"
                                                     { yylhs.value.as < shared_ptr<EnumSpecifier> > () = make_shared<EnumSpecifier>(nullptr,yystack_[1].value.as < shared_ptr<vector<shared_ptr<Enumerator>>> > ()); cout << "ENUM OCURLY enumerator_list CCURLY REDUCE to enum_specifier" << endl;}
#line 2655 "parser.cpp"
    break;

  case 130: // enum_specifier: "enum" "identifier" "{" enumerator_list "}"
#line 490 "ansic.y"
                                                     { yylhs.value.as < shared_ptr<EnumSpecifier> > () = make_shared<EnumSpecifier>(yystack_[3].value.as < shared_ptr<CToken> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<Enumerator>>> > ()); cout << "ENUM IDENTIFIER OCURLY enumerator_list CCURLY REDUCE to enum_specifier" << endl;}
#line 2661 "parser.cpp"
    break;

  case 131: // enum_specifier: "enum" "identifier"
#line 491 "ansic.y"
                                                     { yylhs.value.as < shared_ptr<EnumSpecifier> > () = make_shared<EnumSpecifier>(yystack_[0].value.as < shared_ptr<CToken> > (),nullptr); cout << "ENUM IDENTIFIER REDUCE to enum_specifier" << endl;}
#line 2667 "parser.cpp"
    break;

  case 132: // enumerator_list: enumerator
#line 495 "ansic.y"
                                        { yylhs.value.as < shared_ptr<vector<shared_ptr<Enumerator>>> > () = createEnumeratorList(yystack_[0].value.as < shared_ptr<Enumerator> > (),nullptr); cout << "enumerator REDUCE enumerator_list" << endl;}
#line 2673 "parser.cpp"
    break;

  case 133: // enumerator_list: enumerator_list "," enumerator
#line 496 "ansic.y"
                                        { yylhs.value.as < shared_ptr<vector<shared_ptr<Enumerator>>> > () = createEnumeratorList(yystack_[0].value.as < shared_ptr<Enumerator> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<Enumerator>>> > ()); cout << "enumerator_list COMMA enumerator REDUCE enumerator_list" << endl; }
#line 2679 "parser.cpp"
    break;

  case 134: // enumerator: "identifier"
#line 500 "ansic.y"
                                           { yylhs.value.as < shared_ptr<Enumerator> > () = make_shared<Enumerator>(yystack_[0].value.as < shared_ptr<CToken> > (),nullptr); cout << "IDENTIFIER REDUCE to ENUMERATOR" << endl;}
#line 2685 "parser.cpp"
    break;

  case 135: // enumerator: "identifier" "=" constant_expression
#line 501 "ansic.y"
                                           { yylhs.value.as < shared_ptr<Enumerator> > () = make_shared<Enumerator>(yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "IDENTIFIER EQUAL constant_expression REDUCE to ENUMERATOR" << endl;}
#line 2691 "parser.cpp"
    break;

  case 136: // type_qualifier: "const"
#line 505 "ansic.y"
                { yylhs.value.as < shared_ptr<TypeQualifier> > () = make_shared<TypeQualifier>(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "CONSTT REDUCE to type_qualifier" << endl;}
#line 2697 "parser.cpp"
    break;

  case 137: // type_qualifier: "volatile"
#line 506 "ansic.y"
               { yylhs.value.as < shared_ptr<TypeQualifier> > () = make_shared<TypeQualifier>(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "VOLATILE REDUCE to type_qualifier" << endl;}
#line 2703 "parser.cpp"
    break;

  case 138: // declarator: pointer direct_declarator
#line 510 "ansic.y"
                                { yylhs.value.as < shared_ptr<Declarator> > () = createDeclarator(yystack_[1].value.as < shared_ptr<Pointer> > (),yystack_[0].value.as < shared_ptr<DirectDeclarator> > ()); cout << "pointer direct_declarator REDUCE to declarator" << endl;}
#line 2709 "parser.cpp"
    break;

  case 139: // declarator: direct_declarator
#line 511 "ansic.y"
                                { yylhs.value.as < shared_ptr<Declarator> > () = createDeclarator(nullptr,yystack_[0].value.as < shared_ptr<DirectDeclarator> > ()); cout << "direct_declarator REDUCE to declarator" << endl;}
#line 2715 "parser.cpp"
    break;

  case 140: // direct_declarator: "identifier"
#line 515 "ansic.y"
                                                            { yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<DirectDeclarator> > () = createDirectDeclarator(yystack_[0].value.as < shared_ptr<CToken> > (),nullptr,nullptr,NULL,NULL,NULL,NULL,NULL); cout << "IDENTIFIER REDUCE to direct_declarator" << endl;}
#line 2721 "parser.cpp"
    break;

  case 141: // direct_declarator: "(" declarator ")"
#line 516 "ansic.y"
                                                            { yystack_[2].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[2].location.begin.line, yystack_[2].location.begin.column); yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<DirectDeclarator> > () = createDirectDeclarator(nullptr,yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),yystack_[1].value.as < shared_ptr<Declarator> > (),NULL,NULL,NULL,NULL); cout << "OPAREN declarator CPAREN REDUCE to direct_declarator" << endl;}
#line 2727 "parser.cpp"
    break;

  case 142: // direct_declarator: direct_declarator "[" constant_expression "]"
#line 517 "ansic.y"
                                                            { yystack_[2].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[2].location.begin.line, yystack_[2].location.begin.column); yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<DirectDeclarator> > () = createDirectDeclarator(nullptr,yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),NULL,yystack_[3].value.as < shared_ptr<DirectDeclarator> > (),yystack_[1].value.as < shared_ptr<ExpressionTree> > (),NULL,NULL); cout << "direct_declarator OBRACE constant_expression CBRACE REDUCE to direct_declarator" << endl;}
#line 2733 "parser.cpp"
    break;

  case 143: // direct_declarator: direct_declarator "[" "]"
#line 518 "ansic.y"
                                                            { yystack_[1].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[1].location.begin.line, yystack_[1].location.begin.column); yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<DirectDeclarator> > () = createDirectDeclarator(nullptr,yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),NULL,yystack_[2].value.as < shared_ptr<DirectDeclarator> > (),NULL,NULL,NULL); cout << "direct_declarator OBRACE CBRACE REDUCE to direct_declarator" << endl;}
#line 2739 "parser.cpp"
    break;

  case 144: // direct_declarator: direct_declarator "(" parameter_type_list ")"
#line 519 "ansic.y"
                                                            { yystack_[2].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[2].location.begin.line, yystack_[2].location.begin.column); yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<DirectDeclarator> > () = createDirectDeclarator(nullptr,yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),NULL,yystack_[3].value.as < shared_ptr<DirectDeclarator> > (),NULL,yystack_[1].value.as < shared_ptr<ParameterTypeList> > (),NULL); cout << "direct_declarator OPAREN parameter_type_list CPAREN to direct_declarator" << endl;}
#line 2745 "parser.cpp"
    break;

  case 145: // direct_declarator: direct_declarator "(" identifier_list ")"
#line 520 "ansic.y"
                                                            { yystack_[2].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[2].location.begin.line, yystack_[2].location.begin.column); yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<DirectDeclarator> > () = createDirectDeclarator(nullptr,yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),NULL,yystack_[3].value.as < shared_ptr<DirectDeclarator> > (),NULL,NULL,yystack_[1].value.as < shared_ptr<vector<shared_ptr<CToken>>> > ()); cout << "direct_declarator OPAREN identifier_list CPAREN REDUCE to direct_declarator" << endl;}
#line 2751 "parser.cpp"
    break;

  case 146: // direct_declarator: direct_declarator "(" ")"
#line 521 "ansic.y"
                                                            { yystack_[1].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[1].location.begin.line, yystack_[1].location.begin.column); yystack_[0].value.as < shared_ptr<CToken> > ()->setLocation(yystack_[0].location.begin.line, yystack_[0].location.begin.column); yylhs.value.as < shared_ptr<DirectDeclarator> > () = createDirectDeclarator(nullptr,yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<CToken> > (),NULL,yystack_[2].value.as < shared_ptr<DirectDeclarator> > (),NULL,NULL,NULL); cout << "direct_declarator OPAREN CPAREN REDUCE to direct_declarator" << endl;}
#line 2757 "parser.cpp"
    break;

  case 147: // pointer: "*"
#line 525 "ansic.y"
                                            {yylhs.value.as < shared_ptr<Pointer> > () = make_shared<Pointer>(yystack_[0].value.as < shared_ptr<CToken> > ()); cout << "TIMES_OP REDUCE to POINTER" << endl;}
#line 2763 "parser.cpp"
    break;

  case 148: // pointer: "*" type_qualifier_list
#line 526 "ansic.y"
                                            {yylhs.value.as < shared_ptr<Pointer> > () = make_shared<Pointer>(yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<vector<shared_ptr<TypeQualifier>>> > ()); cout << "TIMES_OP type_qualifier_list REDUCE to POINTER" << endl;}
#line 2769 "parser.cpp"
    break;

  case 149: // pointer: "*" pointer
#line 527 "ansic.y"
                                            {yylhs.value.as < shared_ptr<Pointer> > () = make_shared<Pointer>(yystack_[1].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<Pointer> > ()); yylhs.value.as < shared_ptr<Pointer> > ()->inc(); cout << "TIMES_OP pointer REDUCE to POINTER" << endl;}
#line 2775 "parser.cpp"
    break;

  case 150: // pointer: "*" type_qualifier_list pointer
#line 528 "ansic.y"
                                            {yylhs.value.as < shared_ptr<Pointer> > () = make_shared<Pointer>(yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<TypeQualifier>>> > (),yystack_[0].value.as < shared_ptr<Pointer> > ()); yylhs.value.as < shared_ptr<Pointer> > ()->inc(); cout << "TIMES_OP type_qualifier_list pointer REDUCE to POINTER" << endl;}
#line 2781 "parser.cpp"
    break;

  case 151: // type_qualifier_list: type_qualifier
#line 532 "ansic.y"
                                         { yylhs.value.as < shared_ptr<vector<shared_ptr<TypeQualifier>>> > () = createTypeQualifierList(yystack_[0].value.as < shared_ptr<TypeQualifier> > (),nullptr);cout << "type_qualifier REDUCE type_qualifier_list" << endl; }
#line 2787 "parser.cpp"
    break;

  case 152: // type_qualifier_list: type_qualifier_list type_qualifier
#line 533 "ansic.y"
                                         { yylhs.value.as < shared_ptr<vector<shared_ptr<TypeQualifier>>> > () = createTypeQualifierList(yystack_[0].value.as < shared_ptr<TypeQualifier> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<TypeQualifier>>> > ()); cout << "type_qualifier_list type_qualifier REDUCE type_qualifier_list" << endl; }
#line 2793 "parser.cpp"
    break;

  case 153: // parameter_type_list: parameter_list
#line 537 "ansic.y"
                                      { yylhs.value.as < shared_ptr<ParameterTypeList> > () = make_shared<ParameterTypeList>(yystack_[0].value.as < shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > (),false); cout << "parameter_list REDUCE to parameter_type_list" << endl; }
#line 2799 "parser.cpp"
    break;

  case 154: // parameter_type_list: parameter_list "," "ellipsis"
#line 538 "ansic.y"
                                      { yylhs.value.as < shared_ptr<ParameterTypeList> > () = make_shared<ParameterTypeList>(yystack_[2].value.as < shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > (),true); cout << "parameter_list COMMA ELLIPSIS REDUCE to parameter_type_list" << endl; }
#line 2805 "parser.cpp"
    break;

  case 155: // parameter_list: parameter_declaration
#line 542 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > () = createParameterList(yystack_[0].value.as < shared_ptr<ParameterDeclaration> > (),nullptr); cout << "parameter_declaration REDUCE to parameter_list" << endl; }
#line 2811 "parser.cpp"
    break;

  case 156: // parameter_list: parameter_list "," parameter_declaration
#line 543 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > () = createParameterList(yystack_[0].value.as < shared_ptr<ParameterDeclaration> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<ParameterDeclaration>>> > ()); cout << "parameter_list COMMA parameter_declaration REDUCE to parameter_list" << endl; }
#line 2817 "parser.cpp"
    break;

  case 157: // parameter_declaration: declaration_specifiers declarator
#line 547 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<ParameterDeclaration> > () = make_shared<ParameterDeclaration>(yystack_[1].value.as < shared_ptr<DeclarationSpecifiers> > (),yystack_[0].value.as < shared_ptr<Declarator> > ()); cout << "declaration_specifiers declarator REDUCE to parameter_declaration" << endl; }
#line 2823 "parser.cpp"
    break;

  case 158: // parameter_declaration: declaration_specifiers abstract_declarator
#line 548 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<ParameterDeclaration> > () = make_shared<ParameterDeclaration>(yystack_[1].value.as < shared_ptr<DeclarationSpecifiers> > (),yystack_[0].value.as < shared_ptr<AbstractDeclarator> > ()); cout << "declaration_specifiers abstract_declarator REDUCE to parameter_declaration" << endl; }
#line 2829 "parser.cpp"
    break;

  case 159: // parameter_declaration: declaration_specifiers
#line 549 "ansic.y"
                                                  { yylhs.value.as < shared_ptr<ParameterDeclaration> > () = make_shared<ParameterDeclaration>(yystack_[0].value.as < shared_ptr<DeclarationSpecifiers> > ()); cout << "declaration_specifiers REDUCE to parameter_declaration" << endl; }
#line 2835 "parser.cpp"
    break;

  case 160: // identifier_list: "identifier"
#line 553 "ansic.y"
                                       { yylhs.value.as < shared_ptr<vector<shared_ptr<CToken>>> > () = createIdentifierList(yystack_[0].value.as < shared_ptr<CToken> > (),nullptr); cout << "IDENTIFIER REDUCE to identifier_list" << endl; }
#line 2841 "parser.cpp"
    break;

  case 161: // identifier_list: identifier_list "," "identifier"
#line 554 "ansic.y"
                                       { yylhs.value.as < shared_ptr<vector<shared_ptr<CToken>>> > () = createIdentifierList(yystack_[0].value.as < shared_ptr<CToken> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<CToken>>> > ()); cout << "identifier_list COMMA IDENTIFIER REDUCE to identifier_list" << endl; }
#line 2847 "parser.cpp"
    break;

  case 162: // type_name: specifier_qualifier_list
#line 558 "ansic.y"
                                                   { yylhs.value.as < shared_ptr<TypeName> > () = make_shared<TypeName>(yystack_[0].value.as < shared_ptr<SpecifierQualifierList> > ()); cout << "specifier_qualifier_list REDUCE to type_name" << endl; }
#line 2853 "parser.cpp"
    break;

  case 163: // type_name: specifier_qualifier_list abstract_declarator
#line 559 "ansic.y"
                                                   { yylhs.value.as < shared_ptr<TypeName> > () = make_shared<TypeName>(yystack_[1].value.as < shared_ptr<SpecifierQualifierList> > (),yystack_[0].value.as < shared_ptr<AbstractDeclarator> > ()); cout << "specifier_qualifier_list abstract_declarator REDUCE to type_name" << endl; }
#line 2859 "parser.cpp"
    break;

  case 164: // abstract_declarator: pointer
#line 563 "ansic.y"
                                          { yylhs.value.as < shared_ptr<AbstractDeclarator> > () = make_shared<AbstractDeclarator>(yystack_[0].value.as < shared_ptr<Pointer> > (),nullptr); cout << "pointer REDUCE to abstract_declarator" << endl; }
#line 2865 "parser.cpp"
    break;

  case 165: // abstract_declarator: direct_abstract_declarator
#line 564 "ansic.y"
                                          { yylhs.value.as < shared_ptr<AbstractDeclarator> > () = make_shared<AbstractDeclarator>(nullptr,yystack_[0].value.as < shared_ptr<DirectAbstractDeclarator> > ()); cout << "direct_abstract_declarator REDUCE to abstract_declarator" << endl; }
#line 2871 "parser.cpp"
    break;

  case 166: // abstract_declarator: pointer direct_abstract_declarator
#line 565 "ansic.y"
                                          { yylhs.value.as < shared_ptr<AbstractDeclarator> > () = make_shared<AbstractDeclarator>(yystack_[1].value.as < shared_ptr<Pointer> > (),yystack_[0].value.as < shared_ptr<DirectAbstractDeclarator> > ());      cout << "pointer direct_abstract_declarator REDUCE to abstract_declarator" << endl; }
#line 2877 "parser.cpp"
    break;

  case 167: // direct_abstract_declarator: "(" abstract_declarator ")"
#line 569 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(yystack_[1].value.as < shared_ptr<AbstractDeclarator> > (), nullptr); cout << "OPAREN abstract_declarator CPAREN REDUCE to direct_abstract_declarator" << endl; }
#line 2883 "parser.cpp"
    break;

  case 168: // direct_abstract_declarator: "[" "]"
#line 570 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(nullptr, nullptr, nullptr, nullptr, ARRAY); cout << "OBRACE CBRACE REDUCE to direct_abstract_declarator" << endl; }
#line 2889 "parser.cpp"
    break;

  case 169: // direct_abstract_declarator: "(" ")"
#line 571 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(nullptr, nullptr, nullptr, nullptr, FUNCTION); cout << "OPAREN CPAREN REDUCE to direct_abstract_declarator" << endl; }
#line 2895 "parser.cpp"
    break;

  case 170: // direct_abstract_declarator: "[" constant_expression "]"
#line 572 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(nullptr, nullptr, nullptr, yystack_[1].value.as < shared_ptr<ExpressionTree> > (), NONE); cout << "OBRACE constant_expression CBRACE REDUCE to direct_abstract_declarator" << endl; }
#line 2901 "parser.cpp"
    break;

  case 171: // direct_abstract_declarator: "(" parameter_type_list ")"
#line 573 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(nullptr, nullptr, yystack_[1].value.as < shared_ptr<ParameterTypeList> > (), nullptr, FUNCTION); cout << "OPAREN parameter_type_list CPAREN REDUCE to direct_abstract_declarator" << endl; }
#line 2907 "parser.cpp"
    break;

  case 172: // direct_abstract_declarator: direct_abstract_declarator "[" "]"
#line 574 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(yystack_[2].value.as < shared_ptr<DirectAbstractDeclarator> > (), nullptr, nullptr, nullptr, ARRAY); cout << "direct_abstract_declarator OBRACE CBRACE REDUCE to direct_abstract_declarator" << endl; }
#line 2913 "parser.cpp"
    break;

  case 173: // direct_abstract_declarator: direct_abstract_declarator "(" ")"
#line 575 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(yystack_[2].value.as < shared_ptr<DirectAbstractDeclarator> > (), nullptr, nullptr, nullptr, FUNCTION); cout << "direct_abstract_declarator OPAREN CPAREN REDUCE to direct_abstract_declarator" << endl; }
#line 2919 "parser.cpp"
    break;

  case 174: // direct_abstract_declarator: direct_abstract_declarator "[" constant_expression "]"
#line 576 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(yystack_[3].value.as < shared_ptr<DirectAbstractDeclarator> > (), nullptr, nullptr, yystack_[1].value.as < shared_ptr<ExpressionTree> > (), ARRAY); cout << "direct_abstract_declarator OBRACE constant_expression CBRACE REDUCE to direct_abstract_declarator" << endl; }
#line 2925 "parser.cpp"
    break;

  case 175: // direct_abstract_declarator: direct_abstract_declarator "(" parameter_type_list ")"
#line 577 "ansic.y"
                                                                   { yylhs.value.as < shared_ptr<DirectAbstractDeclarator> > () = createDirectAbstractDeclarator(yystack_[3].value.as < shared_ptr<DirectAbstractDeclarator> > (), nullptr, yystack_[1].value.as < shared_ptr<ParameterTypeList> > (), nullptr, FUNCTION); cout << "direct_abstract_declarator OPAREN parameter_type_list CPAREN REDUCE to direct_abstract_declarator" << endl; }
#line 2931 "parser.cpp"
    break;

  case 176: // initializer: assignment_expression
#line 581 "ansic.y"
                                           { yylhs.value.as < shared_ptr<Initializer> > () = make_shared<Initializer>(yystack_[0].value.as < shared_ptr<ExpressionTree> > ()); cout << "assignment_expression REDUCE to initializer" << endl; }
#line 2937 "parser.cpp"
    break;

  case 177: // initializer: "{" initializer_list "}"
#line 582 "ansic.y"
                                           { yylhs.value.as < shared_ptr<Initializer> > () = make_shared<Initializer>(yystack_[1].value.as < shared_ptr<vector<shared_ptr<Initializer>>> > ()); cout << "OCURLY initializer_list CCURLY REDUCE to initializer" << endl; }
#line 2943 "parser.cpp"
    break;

  case 178: // initializer: "{" initializer_list "," "}"
#line 583 "ansic.y"
                                           { yylhs.value.as < shared_ptr<Initializer> > () = make_shared<Initializer>(yystack_[2].value.as < shared_ptr<vector<shared_ptr<Initializer>>> > ()); cout << "OCURLY initializer_list COMMA CCURLY REDUCE to initializer" << endl; }
#line 2949 "parser.cpp"
    break;

  case 179: // initializer_list: initializer
#line 587 "ansic.y"
                                          { yylhs.value.as < shared_ptr<vector<shared_ptr<Initializer>>> > () = createInitializerList(yystack_[0].value.as < shared_ptr<Initializer> > (),nullptr); cout << "initializer REDUCE to initializer_list" << endl; }
#line 2955 "parser.cpp"
    break;

  case 180: // initializer_list: initializer_list "," initializer
#line 588 "ansic.y"
                                          { yylhs.value.as < shared_ptr<vector<shared_ptr<Initializer>>> > () = createInitializerList(yystack_[0].value.as < shared_ptr<Initializer> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<Initializer>>> > ()); cout << "initializer_list COMMA initializer REDUCE to initializer_list" << endl; }
#line 2961 "parser.cpp"
    break;

  case 181: // statement: labeled_statement
#line 592 "ansic.y"
                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<Statement>(labeled_statement,yystack_[0].value.as < shared_ptr<LabeledStatement> > ()); cout << "labeled_statement REDUCE to statement" << endl; }
#line 2967 "parser.cpp"
    break;

  case 182: // statement: compound_statement
#line 593 "ansic.y"
                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<Statement>(compound_statement,yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "compound_statement REDUCE to statement" << endl; }
#line 2973 "parser.cpp"
    break;

  case 183: // statement: expression_statement
#line 594 "ansic.y"
                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<Statement>(expression_statement,yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "expression_statement REDUCE to statement" << endl; }
#line 2979 "parser.cpp"
    break;

  case 184: // statement: selection_statement
#line 595 "ansic.y"
                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<Statement>(selection_statement,yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "selection_statement REDUCE to statement" << endl; }
#line 2985 "parser.cpp"
    break;

  case 185: // statement: iteration_statement
#line 596 "ansic.y"
                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<Statement>(iteration_statement,yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "iteration_statement REDUCE to statement" << endl; }
#line 2991 "parser.cpp"
    break;

  case 186: // statement: jump_statement
#line 597 "ansic.y"
                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<Statement>(jump_statement,yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "jump_statement REDUCE to statement" << endl; }
#line 2997 "parser.cpp"
    break;

  case 187: // labeled_statement: "identifier" ":" statement
#line 601 "ansic.y"
                                               { yylhs.value.as < shared_ptr<LabeledStatement> > () = make_shared<LabeledStatement>(yystack_[2].value.as < shared_ptr<CToken> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "IDENTIFIER COLON statement REDUCE to label_statement" << endl; }
#line 3003 "parser.cpp"
    break;

  case 188: // labeled_statement: "case" constant_expression ":" statement
#line 602 "ansic.y"
                                               { yylhs.value.as < shared_ptr<LabeledStatement> > () = make_shared<LabeledStatement>(CASE,yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "CASE constant_expression COLON statement REDUCE to label_statement" << endl; }
#line 3009 "parser.cpp"
    break;

  case 189: // labeled_statement: "default" ":" statement
#line 603 "ansic.y"
                                               { yylhs.value.as < shared_ptr<LabeledStatement> > () = make_shared<LabeledStatement>(DEFAULT,yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "DEFAULT COLON statement REDUCE to label_statement" << endl; }
#line 3015 "parser.cpp"
    break;

  case 190: // compound_statement: "{" "}"
#line 607 "ansic.y"
                                                    { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<CompoundStatement>(); cout << "OCURLY CCURLY REDUCE to compound_statement" << endl; }
#line 3021 "parser.cpp"
    break;

  case 191: // compound_statement: "{" statement_list "}"
#line 608 "ansic.y"
                                                    { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<CompoundStatement>(yystack_[1].value.as < shared_ptr<vector<shared_ptr<BaseStatement>>> > ()); cout << "OCURLY statement_list CCURLY REDUCE to compound_statement" << endl; }
#line 3027 "parser.cpp"
    break;

  case 192: // compound_statement: "{" declaration_list "}"
#line 609 "ansic.y"
                                                    { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<CompoundStatement>(yystack_[1].value.as < shared_ptr<vector<shared_ptr<Declaration>>> > ()); cout << "OCURLY declaration_list CCURLY REDUCE to compound_statement" << endl; }
#line 3033 "parser.cpp"
    break;

  case 193: // compound_statement: "{" declaration_list statement_list "}"
#line 610 "ansic.y"
                                                    { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<CompoundStatement>(yystack_[1].value.as < shared_ptr<vector<shared_ptr<BaseStatement>>> > (),yystack_[2].value.as < shared_ptr<vector<shared_ptr<Declaration>>> > ()); cout << "OCURLY declaration_list statement_list CCURLY REDUCE to compound_statement" << endl; }
#line 3039 "parser.cpp"
    break;

  case 194: // declaration_list: declaration
#line 614 "ansic.y"
                                    { yylhs.value.as < shared_ptr<vector<shared_ptr<Declaration>>> > () = createDeclarationList(yystack_[0].value.as < shared_ptr<Declaration> > (),nullptr); cout << "declaration REDUCE to declaration_list" << endl; }
#line 3045 "parser.cpp"
    break;

  case 195: // declaration_list: declaration_list declaration
#line 615 "ansic.y"
                                    { yylhs.value.as < shared_ptr<vector<shared_ptr<Declaration>>> > () = createDeclarationList(yystack_[0].value.as < shared_ptr<Declaration> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<Declaration>>> > ()); cout << "declaration_list declaration REDUCE to declaration_list" << endl; }
#line 3051 "parser.cpp"
    break;

  case 196: // statement_list: statement
#line 619 "ansic.y"
                                  { yylhs.value.as < shared_ptr<vector<shared_ptr<BaseStatement>>> > () = createStatementList(yystack_[0].value.as < shared_ptr<BaseStatement> > (),nullptr); cout << "statement REDUCE to statement_list" << endl; }
#line 3057 "parser.cpp"
    break;

  case 197: // statement_list: statement_list statement
#line 620 "ansic.y"
                                  { yylhs.value.as < shared_ptr<vector<shared_ptr<BaseStatement>>> > () = createStatementList(yystack_[0].value.as < shared_ptr<BaseStatement> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<BaseStatement>>> > ()); cout << "statement_list statement REDUCE to statement_list" << endl; }
#line 3063 "parser.cpp"
    break;

  case 198: // expression_statement: ";"
#line 624 "ansic.y"
                            { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<ExpressionStatement>(); cout << "SEMICOLON REDUCE to expression_statement" << endl; }
#line 3069 "parser.cpp"
    break;

  case 199: // expression_statement: expression ";"
#line 625 "ansic.y"
                            { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<ExpressionStatement>(yystack_[1].value.as < shared_ptr<ExpressionTree> > ()); cout << "expression SEMICOLON REDUCE to expression_statement" << endl; }
#line 3075 "parser.cpp"
    break;

  case 200: // selection_statement: "if" "(" expression ")" statement
#line 629 "ansic.y"
                                                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<SelectionStatement>(IF,yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "IF OPAREN expression CPAREN statement REDUCE to selection_statement" << endl; }
#line 3081 "parser.cpp"
    break;

  case 201: // selection_statement: "if" "(" expression ")" statement "else" statement
#line 630 "ansic.y"
                                                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<SelectionStatement>(IF,yystack_[4].value.as < shared_ptr<ExpressionTree> > (),yystack_[2].value.as < shared_ptr<BaseStatement> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "IF OPAREN expression CPAREN statement ELSE statement REDUCE to selection_statement" << endl; }
#line 3087 "parser.cpp"
    break;

  case 202: // selection_statement: "switch" "(" expression ")" statement
#line 631 "ansic.y"
                                                           { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<SelectionStatement>(SWITCH,yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "SWITCH OPAREN expression CPAREN statement REDUCE to selection_statement" << endl; }
#line 3093 "parser.cpp"
    break;

  case 203: // iteration_statement: "while" "(" expression ")" statement
#line 635 "ansic.y"
                                                                                       { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<IterationStatement>(WHILE,yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "WHILE OPAREN expression CPAREN statement REDUCE to iteration_statement" << endl; }
#line 3099 "parser.cpp"
    break;

  case 204: // iteration_statement: "do" statement "while" "(" expression ")" ";"
#line 636 "ansic.y"
                                                                                       { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<IterationStatement>(DO,yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[5].value.as < shared_ptr<BaseStatement> > ()); cout << "DO statement WHILE OPAREN expression CPAREN SEMICOLON REDUCE to iteration_statement" << endl; }
#line 3105 "parser.cpp"
    break;

  case 205: // iteration_statement: "for" "(" expression_statement expression_statement ")" statement
#line 637 "ansic.y"
                                                                                       { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<IterationStatement>(FOR,yystack_[3].value.as < shared_ptr<BaseStatement> > (),yystack_[2].value.as < shared_ptr<BaseStatement> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "FOR OPAREN expression_statement expression_statement CPAREN statement REDUCE to iteration_statement" << endl; }
#line 3111 "parser.cpp"
    break;

  case 206: // iteration_statement: "for" "(" expression_statement expression_statement expression ")" statement
#line 638 "ansic.y"
                                                                                       { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<IterationStatement>(FOR,yystack_[4].value.as < shared_ptr<BaseStatement> > (),yystack_[3].value.as < shared_ptr<BaseStatement> > (),yystack_[2].value.as < shared_ptr<ExpressionTree> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "FOR OPAREN expression_statement expression_statement expression CPAREN statement REDUCE to iteration_statement" << endl; }
#line 3117 "parser.cpp"
    break;

  case 207: // jump_statement: "goto" "identifier" ";"
#line 642 "ansic.y"
                                  { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<JumpStatement>(GOTO,yystack_[1].value.as < shared_ptr<CToken> > ()); cout << "GOTO IDENTIFIER SEMICOLON REDUCE to jump_statement" << endl; }
#line 3123 "parser.cpp"
    break;

  case 208: // jump_statement: "continue" ";"
#line 643 "ansic.y"
                                  { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<JumpStatement>(CONTINUE); cout << "CONTINUE SEMICOLON REDUCE to jump_statement" << endl; }
#line 3129 "parser.cpp"
    break;

  case 209: // jump_statement: "break" ";"
#line 644 "ansic.y"
                                  { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<JumpStatement>(BREAK); cout << "BREAK SEMICOLON REDUCE to jump_statement" << endl; }
#line 3135 "parser.cpp"
    break;

  case 210: // jump_statement: "return" ";"
#line 645 "ansic.y"
                                  { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<JumpStatement>(RETURN); cout << "RETURN SEMICOLON REDUCE to jump_statement" << endl; }
#line 3141 "parser.cpp"
    break;

  case 211: // jump_statement: "return" expression ";"
#line 646 "ansic.y"
                                  { yylhs.value.as < shared_ptr<BaseStatement> > () = make_shared<JumpStatement>(RETURN,yystack_[1].value.as < shared_ptr<ExpressionTree> > ()); cout << "RETURN expression SEMICOLON REDUCE to jump_statement" << endl; }
#line 3147 "parser.cpp"
    break;

  case 212: // translation_unit: external_declaration
#line 650 "ansic.y"
                                            { createTranslationUnit(yystack_[0].value.as < shared_ptr<ExternalDeclaration> > ()); cout << "external_declaration REDUCE to translation_unit" << endl << endl; }
#line 3153 "parser.cpp"
    break;

  case 213: // translation_unit: translation_unit external_declaration
#line 651 "ansic.y"
                                            { createTranslationUnit(yystack_[0].value.as < shared_ptr<ExternalDeclaration> > ()); cout << "translation_unit external_declaration REDUCE to translation_unit" << endl << endl; }
#line 3159 "parser.cpp"
    break;

  case 214: // external_declaration: function_definition
#line 655 "ansic.y"
                           { yylhs.value.as < shared_ptr<ExternalDeclaration> > () = make_shared<ExternalDeclaration>(yystack_[0].value.as < shared_ptr<FunctionDefinition> > ()); cout << "function_definition REDUCE to external_declaration" << endl; }
#line 3165 "parser.cpp"
    break;

  case 215: // external_declaration: declaration
#line 656 "ansic.y"
                           { yylhs.value.as < shared_ptr<ExternalDeclaration> > () = make_shared<ExternalDeclaration>(yystack_[0].value.as < shared_ptr<Declaration> > ()); cout << "declaration REDUCE to external_declaration" << endl; }
#line 3171 "parser.cpp"
    break;

  case 216: // function_definition: declaration_specifiers declarator declaration_list compound_statement
#line 660 "ansic.y"
                                                                            { yylhs.value.as < shared_ptr<FunctionDefinition> > () = make_shared<FunctionDefinition>(yystack_[3].value.as < shared_ptr<DeclarationSpecifiers> > (),yystack_[2].value.as < shared_ptr<Declarator> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<Declaration>>> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "declaration_specifiers declarator declaration_list compound_statement REDUCE to function_definition" << endl; }
#line 3177 "parser.cpp"
    break;

  case 217: // function_definition: declaration_specifiers declarator compound_statement
#line 661 "ansic.y"
                                                                            { yylhs.value.as < shared_ptr<FunctionDefinition> > () = make_shared<FunctionDefinition>(yystack_[2].value.as < shared_ptr<DeclarationSpecifiers> > (),yystack_[1].value.as < shared_ptr<Declarator> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "declaration_specifiers declarator compound_statement REDUCE to function_definition" << endl; }
#line 3183 "parser.cpp"
    break;

  case 218: // function_definition: declarator declaration_list compound_statement
#line 662 "ansic.y"
                                                                            { yylhs.value.as < shared_ptr<FunctionDefinition> > () = make_shared<FunctionDefinition>(yystack_[2].value.as < shared_ptr<Declarator> > (),yystack_[1].value.as < shared_ptr<vector<shared_ptr<Declaration>>> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "declarator declaration_list compound_statement REDUCE to function_definition" << endl; }
#line 3189 "parser.cpp"
    break;

  case 219: // function_definition: declarator compound_statement
#line 663 "ansic.y"
                                                                            { yylhs.value.as < shared_ptr<FunctionDefinition> > () = make_shared<FunctionDefinition>(yystack_[1].value.as < shared_ptr<Declarator> > (),yystack_[0].value.as < shared_ptr<BaseStatement> > ()); cout << "declarator compound_statement REDUCE to function_definition" << endl; }
#line 3195 "parser.cpp"
    break;


#line 3199 "parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
   Parser ::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
   Parser ::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
   Parser ::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  //  Parser ::context.
   Parser ::context::context (const  Parser & yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
   Parser ::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
   Parser ::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
   Parser ::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short  Parser ::yypact_ninf_ = -226;

  const signed char  Parser ::yytable_ninf_ = -1;

  const short
   Parser ::yypact_[] =
  {
     897,  -226,  -226,  -226,  -226,  -226,  -226,  -226,     5,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,  -226,  -226,    60,    36,  -226,   131,  1597,
    1597,  -226,    10,  -226,  1597,  1520,   156,     7,   279,  -226,
    -226,   -29,    30,   -30,  -226,  -226,    36,  -226,    20,  -226,
    1401,  -226,  -226,    12,  1633,  -226,   337,  -226,   131,  -226,
    1520,  1444,   940,   156,  -226,  -226,    30,    -3,   151,  -226,
    -226,  -226,  -226,  -226,    60,   973,  -226,  1520,  1633,  1633,
     629,  -226,    17,  1633,    25,  -226,  -226,  -226,    42,  1225,
      52,    67,   815,    51,   107,    90,  1028,  1247,   103,   143,
    -226,  -226,   509,  -226,  1280,  1280,  -226,  -226,  -226,  -226,
    -226,  -226,  -226,   142,   338,  1225,  -226,    43,   130,   163,
      50,   178,   108,    68,   136,   153,     4,  -226,  -226,    33,
    -226,  -226,  -226,   423,   595,  -226,  -226,  -226,  -226,   177,
    -226,  -226,  -226,  -226,    69,   197,   189,  -226,    87,  -226,
    -226,  -226,  -226,   193,   213,  1225,  -226,    30,  -226,   973,
    -226,  -226,  -226,   715,  -226,  -226,  -226,  1225,    91,  -226,
     209,  -226,   815,  -226,   218,  -226,   815,   242,  1035,   245,
    1225,  -226,    99,   509,  -226,  1225,  1225,   100,    32,   240,
     509,  -226,  -226,  1090,  1225,   291,  -226,  -226,   294,  -226,
    -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,  -226,
    1225,  -226,  1225,  1225,  1225,  1225,  1225,  1225,  1225,  1225,
    1225,  1225,  1225,  1225,  1225,  1225,  1225,  1225,  1225,  1225,
    1225,  -226,  1225,  -226,   681,  -226,  -226,   763,  1098,  -226,
      23,  -226,   186,  -226,  1558,   298,  -226,  -226,  -226,  -226,
    -226,  -226,   216,  -226,  -226,  -226,    17,  1225,  -226,   815,
    -226,   250,  1035,  -226,   140,  -226,   251,   152,   159,  -226,
    1362,   195,  -226,  1131,   257,  -226,   172,  -226,    37,  -226,
    -226,  -226,  -226,  -226,  -226,    43,    43,   130,   130,   163,
     163,   163,   163,    50,    50,   178,   108,    68,   136,   217,
     153,  -226,  -226,  -226,   260,   263,  -226,   248,   186,  1482,
    1153,  -226,  -226,  -226,  -226,   807,  -226,  -226,  -226,  1225,
    1192,   815,   274,   815,   815,   973,  -226,   274,  1225,  -226,
    -226,  1225,  -226,  -226,  -226,  -226,   271,  -226,   270,  -226,
    -226,   173,   815,   181,   308,  -226,  -226,   222,  -226,  -226,
    -226,  -226,   280,  -226,   815,   815,  -226,   916,  -226,  -226,
    -226,  -226
  };

  const unsigned char
   Parser ::yydefact_[] =
  {
       0,   140,   111,    95,    98,   136,   104,   105,     0,    93,
     103,   100,   101,   102,    96,    99,   107,    94,   115,    92,
     116,   108,    97,   137,   106,     0,   147,   215,     0,    82,
      84,   109,     0,   110,    86,     0,   139,     0,     0,   212,
     214,   131,     0,     0,   151,   149,   148,    80,     0,    88,
      90,    83,    85,   114,     0,    87,     0,   194,     0,   219,
       0,     0,     0,   138,     1,   213,     0,   134,     0,   132,
     141,   152,   150,    81,     0,     0,   217,     0,     0,   121,
       0,   117,     0,   123,     2,     6,     5,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     198,   190,     0,    30,     0,     0,    31,    26,    29,    28,
      27,     8,     3,    20,    32,     0,    34,    38,    41,    44,
      49,    52,    54,    56,    58,    60,    62,    64,    77,     0,
     196,   181,   182,     0,     0,   183,   184,   185,   186,    90,
     195,   218,   160,   146,   159,     0,   153,   155,     0,     2,
     143,    32,    79,     0,     0,     0,   129,     0,    89,     0,
     176,    91,   216,     0,   120,   113,   118,     0,     0,   124,
     126,   122,     0,   209,     0,   208,     0,     0,     0,     0,
       0,   210,     0,     0,    24,     0,     0,     0,   162,     0,
       0,    21,    22,     0,     0,     0,    14,    15,     0,    66,
      73,    72,    70,    71,    67,    68,    69,    74,    75,    76,
       0,    23,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   199,     0,   192,     0,   191,   197,     0,     0,   157,
     164,   158,   165,   144,     0,     0,   145,   142,   130,   135,
     133,   179,     0,   112,   127,   119,     0,     0,   187,     0,
     189,     0,     0,   207,     0,   211,     0,     0,     0,     4,
       0,   164,   163,     0,     0,    10,     0,    18,     0,    12,
      13,    65,    35,    36,    37,    40,    39,    43,    42,    48,
      47,    46,    45,    50,    51,    53,    55,    57,    59,     0,
      61,    78,   193,   169,     0,     0,   168,     0,   166,     0,
       0,   154,   156,   161,   177,     0,   125,   128,   188,     0,
       0,     0,    25,     0,     0,     0,    33,     0,     0,    11,
       9,     0,   171,   167,   170,   173,     0,   172,     0,   178,
     180,     0,     0,     0,   200,   202,   203,     0,    19,    63,
     175,   174,     0,   205,     0,     0,    16,     0,   204,   206,
     201,    17
  };

  const short
   Parser ::yypgoto_[] =
  {
    -226,  -226,  -226,  -226,  -226,   -53,  -226,   -97,    59,    63,
     -33,    65,   104,   102,   105,   106,   109,  -226,   -58,   -74,
    -226,   -90,   -54,    11,     0,  -226,   256,  -226,   -38,  -226,
    -226,   255,   -41,   -36,  -226,    79,  -226,   272,   179,    28,
      -1,   -34,    -9,  -226,   -56,  -226,    93,  -226,  -158,  -123,
    -225,   -73,    57,   -78,  -226,   147,   -28,   229,  -171,  -226,
    -226,  -226,  -226,   317,  -226
  };

  const short
   Parser ::yydefgoto_[] =
  {
       0,   111,   112,   113,   276,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     210,   129,   153,    57,    58,    48,    49,    29,    30,    31,
      32,    80,    81,    82,   168,   169,    33,    68,    69,    34,
      35,    36,    37,    46,   304,   146,   147,   148,   189,   305,
     242,   251,   252,   130,   131,   132,    60,   134,   135,   136,
     137,   138,    38,    39,    40
  };

  const short
   Parser ::yytable_[] =
  {
      28,   160,   161,    63,   152,   145,   182,   262,    41,   151,
       1,    27,   187,    53,   177,   308,    79,    45,   211,    66,
       1,   241,    77,    70,    43,   266,     1,    50,   133,    51,
      52,   152,   274,    67,    55,   174,   151,    72,    28,   166,
      79,    79,    79,   164,   184,    79,   308,   171,     5,    27,
     229,   191,   192,    42,    44,   155,   236,   139,    54,    25,
      78,   144,   151,     1,    79,   272,   188,    73,   167,    25,
      74,   140,     1,   139,    71,   237,   172,   238,    23,   230,
     231,   170,    83,   232,   270,   160,   238,   232,   140,   173,
     264,   320,   330,   187,   258,   267,   268,   152,   260,   175,
     187,   249,   151,   178,   278,    26,    83,    83,    83,   152,
     179,    83,    25,   254,   151,   282,   283,   284,   176,   277,
      26,   237,   166,   238,    26,    79,   219,   220,   221,   222,
      83,   212,   213,   214,     1,   240,   281,   245,   255,   299,
     246,   256,   180,   239,   140,    79,   265,   188,    26,   232,
     232,   226,    79,   269,   188,   185,   236,    26,   301,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   326,   151,    47,   271,
     152,   318,    59,    25,   307,   151,   289,   290,   291,   292,
     232,    83,   225,   321,   193,   186,   194,    76,   195,   152,
     156,   157,   232,   317,   151,   323,    63,   141,    61,   232,
      62,    83,   324,   196,   197,   198,   215,   216,    83,    26,
     151,   227,   328,   232,   162,   329,   352,   228,   240,   341,
     343,   232,   217,   218,   354,    75,    43,   144,   309,   244,
     310,   160,   340,   344,   144,   345,   346,   270,   247,   238,
     243,   160,   152,   336,   348,   170,   338,   151,   223,   224,
     257,   271,   248,   157,   353,   314,   315,   232,   331,   259,
     144,   356,   357,   349,   285,   286,   359,   360,   151,    64,
     287,   288,     1,   160,   340,   261,     2,     3,   293,   294,
       4,     5,   263,   273,   279,     6,     7,   280,     8,     9,
      10,   313,   319,   334,   322,    11,    12,    13,    14,   144,
     327,    15,    16,   332,    17,    18,   333,    19,    20,    21,
      22,    23,   325,    24,   350,   351,   355,   358,   296,   295,
     158,    25,   297,   163,   298,   316,   250,   312,   154,   300,
      84,    85,    86,    87,     2,     3,    88,    89,     4,     5,
      90,    91,    92,     6,     7,    65,     8,     9,    10,    93,
      94,    95,   234,    11,    12,    13,    14,    26,    96,    15,
      16,    97,    17,    18,    98,    19,    20,    21,    22,    23,
      99,    24,   347,     0,   100,    56,   101,     0,     0,   102,
       0,     0,     0,     0,   103,     0,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,     0,   104,   105,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   106,
       0,   107,     0,   108,   109,   110,    84,    85,    86,    87,
       2,     3,    88,    89,     4,     5,    90,    91,    92,     6,
       7,     0,     8,     9,    10,    93,    94,    95,     0,    11,
      12,    13,    14,     0,    96,    15,    16,    97,    17,    18,
      98,    19,    20,    21,    22,    23,    99,    24,     0,     0,
     100,    56,   233,     0,     0,   102,     0,     0,     0,     0,
     103,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   104,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   106,     0,   107,     0,   108,
     109,   110,   149,    85,    86,    87,     2,     0,     0,     0,
       4,     5,     0,     0,     0,     6,     7,     0,     8,     0,
      10,     0,     0,     0,     0,    11,    12,    13,     0,     0,
       0,    15,    16,    97,     0,    18,     0,     0,    20,    21,
      22,    23,     0,    24,     0,     0,     0,     0,     0,     0,
       0,   102,     0,     0,     0,     0,   103,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     104,   105,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   106,     0,   107,     0,   108,   109,   110,    84,    85,
      86,    87,     0,     0,    88,    89,     0,     0,    90,    91,
      92,     0,     0,     0,     0,     0,     0,    93,    94,    95,
       0,     0,     0,     0,     0,     0,    96,     0,     0,    97,
       0,     0,    98,     0,     0,     0,     2,     0,    99,     0,
       4,     5,   100,    56,   235,     6,     7,   102,     8,     0,
      10,     0,   103,     0,     0,    11,    12,    13,     0,     0,
       0,    15,    16,     0,     0,    18,   104,   105,    20,    21,
      22,    23,     0,    24,     0,     0,     0,   106,   165,   107,
       0,   108,   109,   110,    84,    85,    86,    87,     0,     0,
      88,    89,     0,     0,    90,    91,    92,     0,     0,     0,
       0,     0,     0,    93,    94,    95,     0,     0,     0,     0,
       0,     0,    96,     0,     0,    97,     0,     0,    98,     0,
       0,     0,     2,     0,    99,     0,     4,     5,   100,    56,
     302,     6,     7,   102,     8,     0,    10,     0,   103,     0,
       0,    11,    12,    13,     0,     0,     0,    15,    16,     0,
       0,    18,   104,   105,    20,    21,    22,    23,     0,    24,
       0,     0,     0,   106,   253,   107,     1,   108,   109,   110,
       2,     3,     0,     0,     4,     5,     0,     0,     0,     6,
       7,     0,     8,     9,    10,     0,     0,     0,     0,    11,
      12,    13,    14,     0,     0,    15,    16,     0,    17,    18,
       0,    19,    20,    21,    22,    23,     0,    24,     0,     0,
     149,    85,    86,    87,     0,   237,   303,   238,    84,    85,
      86,    87,     0,     0,    88,    89,     0,     0,    90,    91,
      92,     0,     0,     0,     0,     0,     0,    93,    94,    95,
       0,    97,     0,     0,     0,     0,    96,     0,     0,    97,
       0,    26,    98,     0,     0,   159,   339,     0,    99,   102,
       0,     0,   100,    56,   103,     0,     0,   102,     0,     0,
       0,     0,   103,     0,     0,     0,     0,     0,   104,   105,
       0,     0,     0,     0,     0,     0,   104,   105,     0,   106,
       0,   107,     0,   108,   109,   110,     0,   106,     0,   107,
       1,   108,   109,   110,     2,     3,     0,     0,     4,     5,
       0,     0,     0,     6,     7,     0,     8,     9,    10,   149,
      85,    86,    87,    11,    12,    13,    14,     0,     0,    15,
      16,     0,    17,    18,     0,    19,    20,    21,    22,    23,
       0,    24,     0,   149,    85,    86,    87,     0,     0,    25,
      97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,   361,     0,     0,   102,     0,
       0,     0,     0,   103,    97,     0,   149,    85,    86,    87,
       0,     0,     0,     0,     0,    26,     0,   104,   105,     0,
       0,     0,   102,     0,     0,   150,     0,   103,   106,     0,
     107,     0,   108,   109,   110,     0,     0,    97,     0,     0,
       0,   104,   105,     0,     0,     0,     0,     0,     0,     0,
       0,   159,   106,     0,   107,   102,   108,   109,   110,     0,
     103,   149,    85,    86,    87,     0,     0,     0,   149,    85,
      86,    87,     0,     0,   104,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   106,     0,   107,     0,   108,
     109,   110,    97,     0,     0,     0,     0,     0,     0,    97,
       0,     0,     0,     0,     0,   181,     0,     0,     0,     0,
     102,     0,   100,     0,     0,   103,     0,   102,     0,     0,
       0,     0,   103,   149,    85,    86,    87,     0,     0,   104,
     105,   149,    85,    86,    87,     0,   104,   105,     0,     0,
     106,     0,   107,     0,   108,   109,   110,   106,     0,   107,
       0,   108,   109,   110,    97,     0,     0,     0,     0,     0,
       0,     0,    97,     0,   149,    85,    86,    87,     0,     0,
       0,     0,   102,   275,     0,     0,     0,   103,     0,     0,
     102,     0,     0,   306,     0,   103,   149,    85,    86,    87,
       0,   104,   105,     0,     0,    97,     0,     0,     0,   104,
     105,     0,   106,     0,   107,     0,   108,   109,   110,   325,
     106,     0,   107,   102,   108,   109,   110,    97,   103,     0,
       0,     0,     0,     0,     0,   149,    85,    86,    87,     0,
       0,     0,   104,   105,     0,   102,     0,     0,   337,     0,
     103,     0,     0,   106,     0,   107,     0,   108,   109,   110,
       0,     0,     0,     0,   104,   105,    97,     0,   149,    85,
      86,    87,     0,     0,     0,   106,     0,   107,     0,   108,
     109,   110,     0,     0,   102,   342,     0,     0,     0,   103,
     149,    85,    86,    87,     0,     0,     0,     0,     0,    97,
       0,     0,     0,   104,   105,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   106,     0,   107,   102,   108,   109,
     110,    97,   103,   149,    85,    86,    87,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   104,   105,     0,   183,
       0,     0,     0,     0,   103,     0,     0,   106,     0,   107,
       0,   108,   109,   110,    97,     0,     0,     0,   104,   105,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   106,
       0,   107,   190,   108,   109,   110,     0,   103,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   104,   105,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   106,     0,   107,     0,   108,   109,   110,     2,
       3,     0,     0,     4,     5,     0,     0,     0,     6,     7,
       0,     8,     9,    10,     0,     0,     0,     0,    11,    12,
      13,    14,     0,     0,    15,    16,     0,    17,    18,     0,
      19,    20,    21,    22,    23,     0,    24,     0,     2,     3,
       0,     0,     4,     5,   270,   303,   238,     6,     7,     0,
       8,     9,    10,     0,     0,     0,     0,    11,    12,    13,
      14,     0,     0,    15,    16,     0,    17,    18,     0,    19,
      20,    21,    22,    23,     0,    24,     0,   142,     0,    56,
      26,     2,     3,     0,     0,     4,     5,     0,     0,    75,
       6,     7,     0,     8,     9,    10,     0,     0,     0,     0,
      11,    12,    13,    14,     0,     0,    15,    16,     0,    17,
      18,     0,    19,    20,    21,    22,    23,     0,    24,     2,
       3,     0,     0,     4,     5,     0,     0,   143,     6,     7,
       0,     8,     9,    10,     0,     0,     0,     0,    11,    12,
      13,    14,     0,     0,    15,    16,     0,    17,    18,     0,
      19,    20,    21,    22,    23,     0,    24,     2,     3,     0,
       0,     4,     5,     0,     0,   335,     6,     7,     0,     8,
       9,    10,     0,     0,     0,     0,    11,    12,    13,    14,
       0,     0,    15,    16,     0,    17,    18,     0,    19,    20,
      21,    22,    23,     0,    24,     2,     3,     0,    56,     4,
       5,     0,     0,     0,     6,     7,     0,     8,     9,    10,
       0,     0,     0,     0,    11,    12,    13,    14,     0,     0,
      15,    16,     0,    17,    18,     0,    19,    20,    21,    22,
      23,     0,    24,   311,     2,     3,     0,     0,     4,     5,
       0,     0,     0,     6,     7,     0,     8,     9,    10,     0,
       0,     0,     0,    11,    12,    13,    14,     0,     0,    15,
      16,     0,    17,    18,     0,    19,    20,    21,    22,    23,
       2,    24,     0,     0,     4,     5,     0,     0,     0,     6,
       7,     0,     8,     0,    10,     0,     0,     0,     0,    11,
      12,    13,     0,     0,     0,    15,    16,     0,     0,    18,
       0,     0,    20,    21,    22,    23,     0,    24
  };

  const short
   Parser ::yycheck_[] =
  {
       0,    75,    75,    37,    62,    61,    96,   178,     3,    62,
       3,     0,   102,     3,    92,   240,    54,    26,   115,    48,
       3,   144,    50,    53,    25,   183,     3,    28,    56,    29,
      30,    89,   190,     3,    34,    89,    89,    46,    38,    80,
      78,    79,    80,    79,    97,    83,   271,    83,    12,    38,
      46,   104,   105,    48,    26,    58,   134,    58,    48,    52,
      48,    61,   115,     3,   102,   188,   102,    47,    51,    52,
      50,    60,     3,    74,    46,    52,    51,    54,    42,    75,
      47,    82,    54,    50,    52,   159,    54,    50,    77,    47,
     180,   262,    55,   183,   172,   185,   186,   155,   176,    47,
     190,   155,   155,    52,   194,    88,    78,    79,    80,   167,
       3,    83,    52,   167,   167,   212,   213,   214,    51,   193,
      88,    52,   163,    54,    88,   163,    76,    77,    78,    79,
     102,    88,    89,    90,     3,   144,   210,    50,    47,   229,
      53,    50,    52,   144,   133,   183,    47,   183,    88,    50,
      50,    83,   190,    53,   190,    52,   234,    88,   232,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   273,   230,    47,   188,
     238,   259,    35,    52,   238,   238,   219,   220,   221,   222,
      50,   163,    84,    53,    52,    52,    54,    50,    56,   257,
      49,    50,    50,   257,   257,    53,   240,    60,    52,    50,
      54,   183,    53,    71,    72,    73,    86,    87,   190,    88,
     273,    85,    50,    50,    77,    53,    53,    74,   237,   319,
     320,    50,    69,    70,    53,    58,   237,   237,    52,    50,
      54,   315,   315,   321,   244,   323,   324,    52,    55,    54,
      53,   325,   310,   309,   328,   256,   310,   310,    80,    81,
      51,   270,    49,    50,   342,    49,    50,    50,    51,    51,
     270,    49,    50,   331,   215,   216,   354,   355,   331,     0,
     217,   218,     3,   357,   357,    43,     7,     8,   223,   224,
      11,    12,    47,    53,     3,    16,    17,     3,    19,    20,
      21,     3,    52,    55,    53,    26,    27,    28,    29,   309,
      53,    32,    33,    53,    35,    36,    53,    38,    39,    40,
      41,    42,    48,    44,    53,    55,    18,    47,   226,   225,
      74,    52,   227,    78,   228,   256,   157,   244,    66,   230,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    38,    19,    20,    21,    22,
      23,    24,   133,    26,    27,    28,    29,    88,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,   325,    -1,    47,    48,    49,    -1,    -1,    52,
      -1,    -1,    -1,    -1,    57,    -1,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    84,    -1,    86,    87,    88,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    -1,    19,    20,    21,    22,    23,    24,    -1,    26,
      27,    28,    29,    -1,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    -1,    -1,
      47,    48,    49,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    84,    -1,    86,
      87,    88,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,    17,    -1,    19,    -1,
      21,    -1,    -1,    -1,    -1,    26,    27,    28,    -1,    -1,
      -1,    32,    33,    34,    -1,    36,    -1,    -1,    39,    40,
      41,    42,    -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    -1,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    84,    -1,    86,    87,    88,     3,     4,
       5,     6,    -1,    -1,     9,    10,    -1,    -1,    13,    14,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    34,
      -1,    -1,    37,    -1,    -1,    -1,     7,    -1,    43,    -1,
      11,    12,    47,    48,    49,    16,    17,    52,    19,    -1,
      21,    -1,    57,    -1,    -1,    26,    27,    28,    -1,    -1,
      -1,    32,    33,    -1,    -1,    36,    71,    72,    39,    40,
      41,    42,    -1,    44,    -1,    -1,    -1,    82,    49,    84,
      -1,    86,    87,    88,     3,     4,     5,     6,    -1,    -1,
       9,    10,    -1,    -1,    13,    14,    15,    -1,    -1,    -1,
      -1,    -1,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    34,    -1,    -1,    37,    -1,
      -1,    -1,     7,    -1,    43,    -1,    11,    12,    47,    48,
      49,    16,    17,    52,    19,    -1,    21,    -1,    57,    -1,
      -1,    26,    27,    28,    -1,    -1,    -1,    32,    33,    -1,
      -1,    36,    71,    72,    39,    40,    41,    42,    -1,    44,
      -1,    -1,    -1,    82,    49,    84,     3,    86,    87,    88,
       7,     8,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,
      27,    28,    29,    -1,    -1,    32,    33,    -1,    35,    36,
      -1,    38,    39,    40,    41,    42,    -1,    44,    -1,    -1,
       3,     4,     5,     6,    -1,    52,    53,    54,     3,     4,
       5,     6,    -1,    -1,     9,    10,    -1,    -1,    13,    14,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    22,    23,    24,
      -1,    34,    -1,    -1,    -1,    -1,    31,    -1,    -1,    34,
      -1,    88,    37,    -1,    -1,    48,    49,    -1,    43,    52,
      -1,    -1,    47,    48,    57,    -1,    -1,    52,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    72,    -1,    82,
      -1,    84,    -1,    86,    87,    88,    -1,    82,    -1,    84,
       3,    86,    87,    88,     7,     8,    -1,    -1,    11,    12,
      -1,    -1,    -1,    16,    17,    -1,    19,    20,    21,     3,
       4,     5,     6,    26,    27,    28,    29,    -1,    -1,    32,
      33,    -1,    35,    36,    -1,    38,    39,    40,    41,    42,
      -1,    44,    -1,     3,     4,     5,     6,    -1,    -1,    52,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    52,    -1,
      -1,    -1,    -1,    57,    34,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    88,    -1,    71,    72,    -1,
      -1,    -1,    52,    -1,    -1,    55,    -1,    57,    82,    -1,
      84,    -1,    86,    87,    88,    -1,    -1,    34,    -1,    -1,
      -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    82,    -1,    84,    52,    86,    87,    88,    -1,
      57,     3,     4,     5,     6,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    84,    -1,    86,
      87,    88,    34,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,
      52,    -1,    47,    -1,    -1,    57,    -1,    52,    -1,    -1,
      -1,    -1,    57,     3,     4,     5,     6,    -1,    -1,    71,
      72,     3,     4,     5,     6,    -1,    71,    72,    -1,    -1,
      82,    -1,    84,    -1,    86,    87,    88,    82,    -1,    84,
      -1,    86,    87,    88,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    52,    53,    -1,    -1,    -1,    57,    -1,    -1,
      52,    -1,    -1,    55,    -1,    57,     3,     4,     5,     6,
      -1,    71,    72,    -1,    -1,    34,    -1,    -1,    -1,    71,
      72,    -1,    82,    -1,    84,    -1,    86,    87,    88,    48,
      82,    -1,    84,    52,    86,    87,    88,    34,    57,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    71,    72,    -1,    52,    -1,    -1,    55,    -1,
      57,    -1,    -1,    82,    -1,    84,    -1,    86,    87,    88,
      -1,    -1,    -1,    -1,    71,    72,    34,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    82,    -1,    84,    -1,    86,
      87,    88,    -1,    -1,    52,    53,    -1,    -1,    -1,    57,
       3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    -1,    -1,    71,    72,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    84,    52,    86,    87,
      88,    34,    57,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    72,    -1,    52,
      -1,    -1,    -1,    -1,    57,    -1,    -1,    82,    -1,    84,
      -1,    86,    87,    88,    34,    -1,    -1,    -1,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    84,    52,    86,    87,    88,    -1,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    84,    -1,    86,    87,    88,     7,
       8,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,    17,
      -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,    27,
      28,    29,    -1,    -1,    32,    33,    -1,    35,    36,    -1,
      38,    39,    40,    41,    42,    -1,    44,    -1,     7,     8,
      -1,    -1,    11,    12,    52,    53,    54,    16,    17,    -1,
      19,    20,    21,    -1,    -1,    -1,    -1,    26,    27,    28,
      29,    -1,    -1,    32,    33,    -1,    35,    36,    -1,    38,
      39,    40,    41,    42,    -1,    44,    -1,     3,    -1,    48,
      88,     7,     8,    -1,    -1,    11,    12,    -1,    -1,    58,
      16,    17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,
      26,    27,    28,    29,    -1,    -1,    32,    33,    -1,    35,
      36,    -1,    38,    39,    40,    41,    42,    -1,    44,     7,
       8,    -1,    -1,    11,    12,    -1,    -1,    53,    16,    17,
      -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,    27,
      28,    29,    -1,    -1,    32,    33,    -1,    35,    36,    -1,
      38,    39,    40,    41,    42,    -1,    44,     7,     8,    -1,
      -1,    11,    12,    -1,    -1,    53,    16,    17,    -1,    19,
      20,    21,    -1,    -1,    -1,    -1,    26,    27,    28,    29,
      -1,    -1,    32,    33,    -1,    35,    36,    -1,    38,    39,
      40,    41,    42,    -1,    44,     7,     8,    -1,    48,    11,
      12,    -1,    -1,    -1,    16,    17,    -1,    19,    20,    21,
      -1,    -1,    -1,    -1,    26,    27,    28,    29,    -1,    -1,
      32,    33,    -1,    35,    36,    -1,    38,    39,    40,    41,
      42,    -1,    44,    45,     7,     8,    -1,    -1,    11,    12,
      -1,    -1,    -1,    16,    17,    -1,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    27,    28,    29,    -1,    -1,    32,
      33,    -1,    35,    36,    -1,    38,    39,    40,    41,    42,
       7,    44,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
      17,    -1,    19,    -1,    21,    -1,    -1,    -1,    -1,    26,
      27,    28,    -1,    -1,    -1,    32,    33,    -1,    -1,    36,
      -1,    -1,    39,    40,    41,    42,    -1,    44
  };

  const unsigned char
   Parser ::yystos_[] =
  {
       0,     3,     7,     8,    11,    12,    16,    17,    19,    20,
      21,    26,    27,    28,    29,    32,    33,    35,    36,    38,
      39,    40,    41,    42,    44,    52,    88,   115,   116,   119,
     120,   121,   122,   128,   131,   132,   133,   134,   154,   155,
     156,     3,    48,   132,   131,   134,   135,    47,   117,   118,
     132,   116,   116,     3,    48,   116,    48,   115,   116,   147,
     148,    52,    54,   133,     0,   155,    48,     3,   129,   130,
      53,   131,   134,    47,    50,    58,   147,   148,    48,   120,
     123,   124,   125,   131,     3,     4,     5,     6,     9,    10,
      13,    14,    15,    22,    23,    24,    31,    34,    37,    43,
      47,    49,    52,    57,    71,    72,    82,    84,    86,    87,
      88,    93,    94,    95,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   113,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   132,
     115,   147,     3,    53,   116,   136,   137,   138,   139,     3,
      55,    97,   110,   114,   129,    58,    49,    50,   118,    48,
     111,   143,   147,   123,   125,    49,   124,    51,   126,   127,
     132,   125,    51,    47,   114,    47,    51,   145,    52,     3,
      52,    47,   113,    52,    97,    52,    52,   113,   125,   140,
      52,    97,    97,    52,    54,    56,    71,    72,    73,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
     112,    99,    88,    89,    90,    86,    87,    69,    70,    76,
      77,    78,    79,    80,    81,    84,    83,    85,    74,    46,
      75,    47,    50,    49,   149,    49,   145,    52,    54,   132,
     134,   141,   142,    53,    50,    50,    53,    55,    49,   114,
     130,   143,   144,    49,   114,    47,    50,    51,   145,    51,
     145,    43,   150,    47,   113,    47,   140,   113,   113,    53,
      52,   134,   141,    53,   140,    53,    96,   111,   113,     3,
       3,   111,    99,    99,    99,   100,   100,   101,   101,   102,
     102,   102,   102,   103,   103,   104,   105,   106,   107,   113,
     108,   111,    49,    53,   136,   141,    55,   114,   142,    52,
      54,    45,   138,     3,    49,    50,   127,   114,   145,    52,
     150,    53,    53,    53,    53,    48,    99,    53,    50,    53,
      55,    51,    53,    53,    55,    53,   136,    55,   114,    49,
     143,   113,    53,   113,   145,   145,   145,   144,   111,   110,
      53,    55,    53,   145,    53,    18,    49,    50,    47,   145,
     145,    49
  };

  const unsigned char
   Parser ::yyr1_[] =
  {
       0,    92,    93,    93,    93,    94,    94,    94,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    96,    96,
      97,    97,    97,    97,    97,    97,    98,    98,    98,    98,
      98,    98,    99,    99,   100,   100,   100,   100,   101,   101,
     101,   102,   102,   102,   103,   103,   103,   103,   103,   104,
     104,   104,   105,   105,   106,   106,   107,   107,   108,   108,
     109,   109,   110,   110,   111,   111,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   113,   113,   114,
     115,   115,   116,   116,   116,   116,   116,   116,   117,   117,
     118,   118,   119,   119,   119,   119,   119,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   121,   121,   121,   122,   122,   123,   123,   124,
     125,   125,   125,   125,   126,   126,   127,   127,   127,   128,
     128,   128,   129,   129,   130,   130,   131,   131,   132,   132,
     133,   133,   133,   133,   133,   133,   133,   134,   134,   134,
     134,   135,   135,   136,   136,   137,   137,   138,   138,   138,
     139,   139,   140,   140,   141,   141,   141,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   143,   143,   143,   144,
     144,   145,   145,   145,   145,   145,   145,   146,   146,   146,
     147,   147,   147,   147,   148,   148,   149,   149,   150,   150,
     151,   151,   151,   152,   152,   152,   152,   153,   153,   153,
     153,   153,   154,   154,   155,   155,   156,   156,   156,   156
  };

  const signed char
   Parser ::yyr2_[] =
  {
       0,     2,     1,     1,     3,     1,     1,     1,     1,     4,
       3,     4,     3,     3,     2,     2,     6,     7,     1,     3,
       1,     2,     2,     2,     2,     4,     1,     1,     1,     1,
       1,     1,     1,     4,     1,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     1,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     5,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       2,     3,     1,     2,     1,     2,     1,     2,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     4,     2,     1,     1,     1,     2,     3,
       2,     1,     2,     1,     1,     3,     1,     2,     3,     4,
       5,     2,     1,     3,     1,     3,     1,     1,     2,     1,
       1,     3,     4,     3,     4,     4,     3,     1,     2,     2,
       3,     1,     2,     1,     3,     1,     3,     2,     2,     1,
       1,     3,     1,     2,     1,     1,     2,     3,     2,     2,
       3,     3,     3,     3,     4,     4,     1,     3,     4,     1,
       3,     1,     1,     1,     1,     1,     1,     3,     4,     3,
       2,     3,     3,     4,     1,     2,     1,     2,     1,     2,
       5,     7,     5,     5,     7,     6,     7,     3,     2,     2,
       2,     3,     1,     2,     1,     1,     4,     3,     3,     2
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const  Parser ::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"identifier\"",
  "\"i_const\"", "\"f_const\"", "\"sting_literal\"", "\"type name\"",
  "\"auto\"", "\"break\"", "\"case\"", "\"char\"", "\"const\"",
  "\"continue\"", "\"default\"", "\"do\"", "\"double\"", "\"long double\"",
  "\"else\"", "\"enum\"", "\"extern\"", "\"float\"", "\"for\"", "\"goto\"",
  "\"if\"", "\"inline\"", "INT", "\"long\"", "\"long long\"",
  "\"register\"", "\"restrict\"", "\"return\"", "\"short\"", "\"signed\"",
  "\"sizeof\"", "\"static\"", "\"struct\"", "\"switch\"", "\"typedef\"",
  "\"union\"", "\"unsigned\"", "\"void\"", "\"volatile\"", "\"while\"",
  "\"bool\"", "\"ellipsis\"", "\"question\"", "\";\"", "\"{\"", "\"}\"",
  "\",\"", "\":\"", "\"(\"", "\")\"", "\"[\"", "\"]\"", "\".\"", "\"~\"",
  "\"=\"", "\">>=\"", "\"<<=\"", "\"+=\"", "\"-=\"", "\"*=\"", "\"/=\"",
  "\"%=\"", "\"&=\"", "\"^=\"", "\"|=\"", "\">>\"", "\"<<\"", "\"++\"",
  "\"--\"", "\"->\"", "\"&&\"", "\"||\"", "\">=\"", "\"<=\"", "\">\"",
  "\"<\"", "\"==\"", "\"!=\"", "\"!\"", "\"^\"", "\"&\"", "\"|\"", "\"-\"",
  "\"+\"", "\"*\"", "\"/\"", "\"%\"", "\"then\"", "$accept",
  "primary_expression", "constant", "postfix_expression",
  "argument_expression_list", "unary_expression", "unary_operator",
  "cast_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "type_specifier", "struct_or_union_specifier",
  "struct_or_union", "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enumerator_list", "enumerator",
  "type_qualifier", "declarator", "direct_declarator", "pointer",
  "type_qualifier_list", "parameter_type_list", "parameter_list",
  "parameter_declaration", "identifier_list", "type_name",
  "abstract_declarator", "direct_abstract_declarator", "initializer",
  "initializer_list", "statement", "labeled_statement",
  "compound_statement", "declaration_list", "statement_list",
  "expression_statement", "selection_statement", "iteration_statement",
  "jump_statement", "translation_unit", "external_declaration",
  "function_definition", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
   Parser ::yyrline_[] =
  {
       0,   258,   258,   259,   260,   264,   265,   266,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   282,   283,
     287,   288,   289,   290,   291,   292,   296,   297,   298,   299,
     300,   301,   305,   306,   310,   311,   312,   313,   317,   318,
     319,   323,   324,   325,   329,   330,   331,   332,   333,   337,
     338,   339,   343,   344,   348,   349,   353,   354,   358,   359,
     363,   364,   368,   369,   373,   374,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   392,   393,   397,
     401,   402,   406,   407,   408,   409,   410,   411,   415,   416,
     420,   421,   425,   426,   427,   428,   429,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   451,   452,   453,   457,   458,   462,   463,   467,
     471,   472,   473,   474,   478,   479,   483,   484,   485,   489,
     490,   491,   495,   496,   500,   501,   505,   506,   510,   511,
     515,   516,   517,   518,   519,   520,   521,   525,   526,   527,
     528,   532,   533,   537,   538,   542,   543,   547,   548,   549,
     553,   554,   558,   559,   563,   564,   565,   569,   570,   571,
     572,   573,   574,   575,   576,   577,   581,   582,   583,   587,
     588,   592,   593,   594,   595,   596,   597,   601,   602,   603,
     607,   608,   609,   610,   614,   615,   619,   620,   624,   625,
     629,   630,   631,   635,   636,   637,   638,   642,   643,   644,
     645,   646,   650,   651,   655,   656,   660,   661,   662,   663
  };

  void
   Parser ::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
   Parser ::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 9 "ansic.y"
} //  WadeSpace 
#line 4212 "parser.cpp"

#line 666 "ansic.y"


void WadeSpace::Parser::error(const location_type& loc, const string& message)  
{
    std::ostringstream ss;
    ss << "Error at line " << loc.begin.line << ", column " << loc.begin.column << ": " << message;
    cout << ss.str() << endl;
    messages.addMessage(CompilerMessagesType::ERROR_MSG, ss.str());
}
