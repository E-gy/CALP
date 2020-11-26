#include "catch2ext.hpp"

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
DEF_SYMBOL_TERMINAL(symb_eof, { return !*str ? str : NULL; })
DEF_SYMBOL_TERMINAL(symb_a, { return str[0] == 'a' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(symb_b, { return str[0] == 'b' ? str+1 : NULL; })
DEF_GROUP(grp_0, RULE(SYMBOL_T(symb_a); SYMBOL_T(symb_eof)); RULE(SYMBOL_T(symb_b); SYMBOL_G(grp_0)))
DEF_GRAMMAR(grm_basic, GROUP(grp_0))
}

TEST_CASE("basic", "[basic][parsing][parser construction][grammar]"){
	Grammar g = grm_basic();
	printf("grammar: %p\n", g);
	grammar_log(g);
	ParserBuildResult pr = parser_build(g);
	IfElse_T(pr, p, {
		IfElse_T(parser_parse(p, lexer0, "a", &grp_0), ast, { ast_log(ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
		IfElse_T(parser_parse(p, lexer0, "bba", &grp_0), ast, { ast_log(ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
	}, err, { FAIL_FMT("Parser build failed"); });
}
