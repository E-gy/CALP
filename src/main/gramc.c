#include "gramb.h"

#include <stdlib.h>
#include "null.h"

struct symbol {
	enum {
		TOK_TYPE_TERM = 0,
		TOK_TYPE_GROUP,
	} type;
	union {
		struct {
			TerminalSymbolId id;
		} term;
		struct {
			GroupId id;
		} group;
	} val;
	Symbol next;
};

Symbol symbol_new_term(TerminalSymbolId term){
	new(Symbol, tok);
	*tok = (struct symbol){ TOK_TYPE_TERM, {.term = {term}}, null };
	return tok;
}

Symbol symbol_new_group(GroupId group){
	new(Symbol, tok);
	*tok = (struct symbol){ TOK_TYPE_GROUP, {.group = {group}}, null };
	return tok;
}

struct ruleb {
	Symbol first;
	Symbol last;
};

RuleBuilder ruleb_new(){
	new(RuleBuilder, b);
	*b = (struct ruleb){null, null};
	return b;
}

RuleBuilder ruleb_add(RuleBuilder b, Symbol s){
	if(!b || !s) return b;
	s->next = null;
	if(!b->first) b->first = b->last = s;
	else b->last = (b->last->next = s);
	return b;
}

struct rule {
	Symbol symbols;
	Rule next;
};

Rule ruleb_uild(RuleBuilder b){
	if(!b) return null;
	new(Rule, r);
	*r = (struct rule){b->first, null};
	free(b);
	return r;
}

struct groupb {
	GroupId id;
	Rule first;
	Rule last;
};

GroupBuilder groupb_new(GroupId id){
	new(GroupBuilder, b);
	*b = (struct groupb){id, null, null};
	return b;
}

GroupBuilder groupb_add(GroupBuilder b, Rule r){
	if(!b || !r) return b;
	r->next = null;
	if(!b->first) b->first = b->last = r;
	else b->last = (b->last->next = r);
	return b;
}

struct group {
	GroupId id;
	Rule rules;
	Group next;
};

Group groupb_uild(GroupBuilder b){
	if(!b) return null;
	new(Group, g);
	*g = (struct group){b->id, b->first, null};
	free(b);
	return g;
}

struct gramb {
	Group first;
	Group last;
};

GrammarBuilder gramb_new(){
	new(GrammarBuilder, b);
	*b = (struct gramb){null, null};
	return b;
}

GrammarBuilder gramb_add(GrammarBuilder b, Group g){
	if(!b || !g) return b;
	if(!b->first) b->first = b->last = g;
	else b->last = (b->last->next = g);
	return b;
}

struct grammar {
	Group groups;
};

Grammar gramb_uild(GrammarBuilder b){
	if(!b) return null;
	new(Grammar, g);
	*g = (struct grammar){b->first};
	free(b);
	return g;
}

#endif
