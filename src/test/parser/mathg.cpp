#include "../catch2ext.hpp"

extern "C" {
#include <calp/grammar/fun.h>
#include <calp/grammar/define.h>
#include <calp/parser.h>
#include <calp/parser/build.h>
#include <calp/parser/fun.h>
#include <calp/lexers.h>

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
DEF_GROUP(muls, RULE(SYMBOL_G(muls); SYMBOL_G(tOd); SYMBOL_G(ng)); RULE(SYMBOL_G(ng)))
DEF_GROUP(adds, RULE(SYMBOL_G(adds); SYMBOL_G(pOm); SYMBOL_G(muls)); RULE(SYMBOL_G(muls)))
DEF_GROUP(ng, RULE(SYMBOL_T(lpar); SYMBOL_G(adds); SYMBOL_T(rpar)); RULE(SYMBOL_T(number)))
DEF_GROUP(entry, RULE(SYMBOL_G(adds); SYMBOL_T(eof)))
DEF_GRAMMAR(math, GROUP(ng); GROUP(pOm); GROUP(tOd); GROUP(muls); GROUP(adds); GROUP(entry))
}

SCENARIO("math grammar", "[math grammar][parsing][parser construction][grammar]"){
	Grammar g = math();
	GIVEN("maths grammar"){
		printf("grammar: %p\n", g);
		grammar_log(g);
		ParserBuildResult pr = parser_build(g);
			THEN("parser can be built"){
			IfElse_T(pr, p, {
				AND_THEN("what can be parsed - parses"){
					IfElse_T(parser_parse(p, lexer0, "12", &entry), ast, { ast_log(ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer0, "12+25", &entry), ast, { ast_log(ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer0, "(-21*13/2)*((12/2-25*4)-1)", &entry), ast, { ast_log(ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer0, "120-15-29*2-13", &entry), ast, { ast_log(ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
				}
			}, err, { FAIL_FMT("Parser build failed - %s", err.s); });
		}
	}
}
