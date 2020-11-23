#include <catch2/catch.hpp>

extern "C" {
#include <grammaro.h>
#include <gramdef.h>
#include <parser.h>
#include <parserp.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

DEF_SYMBOL_TERMINAL(eof, { return !*str ? str : NULL; })
DEF_SYMBOL_TERMINAL(eps, { return str; })
DEF_SYMBOL_TERMINAL(number, {
	if(!isdigit(*str)){
		if(*str != '-') return NULL;
		str++;
		if(!isdigit(*str)) return NULL;
	}
	for(; isdigit(*str); str++);
	return str;
})
DEF_SYMBOL_TERMINAL(plus, { return str[0] == '+' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(minus, { return str[0] == '-' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(times, { return str[0] == '*' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(divide, { return str[0] == '/' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(lpar, { return str[0] == '(' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(rpar, { return str[0] == ')' ? str+1 : NULL; })
static Group ng();
static Group muls();
static Group adds();
DEF_GROUP(pOm, RULE(SYMBOL_T(plus)); RULE(SYMBOL_T(minus)))
DEF_GROUP(tOd, RULE(SYMBOL_T(times)); RULE(SYMBOL_T(divide)))
DEF_GROUP(muls_, RULE(SYMBOL_G(tOd); SYMBOL_G(muls)); RULE(SYMBOL_T(eps)))
DEF_GROUP(muls, RULE(SYMBOL_G(ng); SYMBOL_G(muls_)))
DEF_GROUP(adds_, RULE(SYMBOL_G(pOm); SYMBOL_G(adds)); RULE(SYMBOL_T(eps)))
DEF_GROUP(adds, RULE(SYMBOL_G(muls); SYMBOL_G(adds_)))
DEF_GROUP(ng, RULE(SYMBOL_T(lpar); SYMBOL_G(adds); SYMBOL_T(rpar)); RULE(SYMBOL_T(number)))
DEF_GRAMMAR(math, GROUP(ng); GROUP(pOm); GROUP(tOd); GROUP(muls_); GROUP(muls); GROUP(adds_); GROUP(adds);)
}

TEST_CASE("math grammar", "[math grammar]"){
	Grammar g = math();
	printf("grammar: %p\n", g);
	grammar_log(g);
	Parser p = parser_build(g);
	ast_log(parseraw(p, "12", &adds));
	ast_log(parseraw(p, "12+25", &adds));
	ast_log(parseraw(p, "(-21*13/2)*((12/2-25*4)-1)", &adds));
}
