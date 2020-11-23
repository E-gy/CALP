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
DEF_SYMBOL_TERMINAL(ε, { return str; })
DEF_SYMBOL_TERMINAL(sp, { return str[0] == ' ' ? str+1 : NULL; })
DEF_GROUP(sps, RULE(SYMBOL_T(sp); SYMBOL_G(sps)); RULE(SYMBOL_T(ε)));
DEF_SYMBOL_TERMINAL(a, { return str[0] == 'a' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(b, { return str[0] == 'b' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(c, { return str[0] == 'c' ? str+1 : NULL; })
DEF_GROUP(a_, RULE(SYMBOL_T(a)); RULE(SYMBOL_T(ε)))
DEF_GROUP(b_, RULE(SYMBOL_T(b)); RULE(SYMBOL_T(ε)))
DEF_GROUP(g0, RULE(SYMBOL_G(a_); SYMBOL_G(sps); SYMBOL_G(b_); SYMBOL_T(c); SYMBOL_T(eof)))
DEF_GRAMMAR(grm_advanced, GROUP(g0); GROUP(a_); GROUP(b_); GROUP(sps))
}

TEST_CASE("advanced", "[advanced]"){
	Grammar g = grm_advanced();
	printf("grammar: %p\n", g);
	grammar_log(g);
	Parser p = parser_build(g);
	ast_log(parseraw(p, "a bc", &g0));
	ast_log(parseraw(p, "ac", &g0));
	ast_log(parseraw(p, "bc", &g0));
	ast_log(parseraw(p, "a   bc", &g0));
	ast_log(parseraw(p, "  bc", &g0));
}
