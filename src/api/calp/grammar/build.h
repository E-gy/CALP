#pragma once

/**
 * @file build.h
 * Grammar construction functionality.
 * 
 * Provides functions necessary to construct a grammar at runtime.
 */

#include <calp/grammar.h>

/**
 * @param @ref term
 * @param @ref name
 * @returns @produces symbol
 */
Symbol symbol_new_term(TerminalSymbolId term, string name);
/**
 * @param @ref group
 * @returns @produces symbol
 */
Symbol symbol_new_group(GroupId group);

/**
 * @param @consumes symbol
 */
void symbol_destroy(Symbol symbol);

struct ruleb;
typedef struct ruleb* RuleBuilder;

/**
 * @returns @produces builder
 */
RuleBuilder ruleb_new();
/**
 * @param @refmut builder
 * @param @consumes symbol
 */
RuleBuilder ruleb_add(RuleBuilder builder, Symbol symbol);
/**
 * @param @consumes builder
 * @returns @produces rule
 */
Rule ruleb_uild(RuleBuilder builder);

struct groupb;
typedef struct groupb* GroupBuilder;

/**
 * @param @ref id
 * @param @ref name
 * @returns @produces builder
 */
GroupBuilder groupb_new(GroupId id, string name);
/**
 * @param @refmut builder
 * @param @consumes rule
 */
GroupBuilder groupb_add(GroupBuilder builder, Rule rule);
/**
 * @param @consumes builder
 * @returns @produces group
 */
Group groupb_uild(GroupBuilder builder);

struct gramb;
typedef struct gramb* GrammarBuilder;

/**
 * @returns @produces builder
 */
GrammarBuilder gramb_new(string name);
/**
 * @param @refmut builder
 * @param @consumes group
 */
GrammarBuilder gramb_add(GrammarBuilder builder, Group group);
/**
 * @param @consumes builder
 * @returns @produces grammar
 */
Grammar gramb_uild(GrammarBuilder builder);
