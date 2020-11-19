#pragma once

#include "grammar.h"

struct symbol {
	enum {
		SYMBOL_TYPE_TERM = 0,
		SYMBOL_TYPE_GROUP,
	} type;
	union {
		struct {
			TerminalSymbolId id;
			string name;
		} term;
		struct {
			GroupId id;
		} group;
	} val;
	Symbol next;
};

struct rule {
	Symbol symbols;
	Rule next;
};

struct group {
	GroupId id;
	string name;
	Rule rules;
	Group next;
};

struct grammar {
	string name;
	Group groups;
};
