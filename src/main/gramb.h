#pragma once

#include <stdbool.h>
#include "string.h"

struct symbol;
typedef struct symbol* Symbol;

struct ruleb;
typedef struct ruleb* RuleBuilder;
struct rule;
typedef struct rule* Rule;

struct groupb;
typedef struct groupb* GroupBuilder;
struct group;
typedef struct group* Group;

struct gramb;
typedef struct gramb* GrammarBuilder;
struct grammar;
typedef struct grammar* Grammar;


typedef bool (*TerminalSymbolId)(string);
typedef Group (*GroupId)(void);


/**
 * @ref term
 * @ref name
 * @produces symbol
 */
Symbol symbol_new_term(TerminalSymbolId term, string name);
/**
 * @ref group
 * @produces symbol
 */
Symbol symbol_new_group(GroupId group);

/**
 * @produces builder
 */
RuleBuilder ruleb_new();
/**
 * @refmut builder
 * @consumes symbol
 */
RuleBuilder ruleb_add(RuleBuilder builder, Symbol symbol);
/**
 * @consumes builder
 * @produces rule
 */
Rule ruleb_uild(RuleBuilder builder);

/**
 * @ref id
 * @ref name
 * @produces builder
 */
GroupBuilder groupb_new(GroupId id, string name);
/**
 * @refmut builder
 * @consumes rule
 */
GroupBuilder groupb_add(GroupBuilder builder, Rule rule);
/**
 * @consumes builder
 * @produces group
 */
Group groupb_uild(GroupBuilder builder);

/**
 * @produces builder
 */
GrammarBuilder gramb_new(string name);
/**
 * @refmut builder
 * @consumes group
 */
GrammarBuilder gramb_add(GrammarBuilder builder, Group group);
/**
 * @consumes builder
 * @produces grammar
 */
Grammar gramb_uild(GrammarBuilder builder);

#endif
