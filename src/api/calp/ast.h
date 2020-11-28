#pragma once

/**
 * @file ast.h
 * AST structure & operations
 */

#include "grammar.h"

#include "ptypes.h"

struct ast;
typedef struct ast* AST;
struct ast {
	enum {
		AST_GROUP,
		AST_LEAF,
	} type;
	Symbol symbol;
	union {
		struct {
			/** @ref */ GroupId groupId;
			/** @ref */ Group group;
			size_t cc;
			AST children[];
		} group;
		struct {
			/** @ref */ TerminalSymbolId symbolId;
			string_mut val;
		} leaf;
	} d;
};

/**
 * @param @ref symbol
 * @returns @produces ast
 */
AST ast_new_group(Symbol symbol, Group g, size_t children_count);
/**
 * @param @ref symbol
 * @param @consumes val
 */
AST ast_new_leaf(Symbol symbol, string_mut val);

/**
 * @param @consumes ast
 */
void ast_destroy(AST ast);

/**
 * @param @ref ast
 */
void ast_log(AST ast);
