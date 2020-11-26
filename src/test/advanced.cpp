#include <catch2/catch.hpp>

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
DEF_SYMBOL_TERMINAL(sp, { return str[0] == ' ' ? str+1 : NULL; })
DEF_GROUP(sps, RULE(SYMBOL_T(sp); SYMBOL_G(sps)); RULE(SYMBOL_T(eps)));
DEF_SYMBOL_TERMINAL(a, { return str[0] == 'a' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(b, { return str[0] == 'b' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(c, { return str[0] == 'c' ? str+1 : NULL; })
DEF_GROUP(a_, RULE(SYMBOL_T(a)); RULE(SYMBOL_T(eps)))
DEF_GROUP(b_, RULE(SYMBOL_T(b)); RULE(SYMBOL_T(eps)))
DEF_GROUP(g0, RULE(SYMBOL_G(a_); SYMBOL_G(sps); SYMBOL_G(b_); SYMBOL_T(c); SYMBOL_T(eof)))
DEF_GRAMMAR(grm_advanced, GROUP(g0); GROUP(a_); GROUP(b_); GROUP(sps))
}

TEST_CASE("advanced", "[advanced][parsing][parser construction][grammar]"){
	Grammar g = grm_advanced();
	printf("grammar: %p\n", g);
	grammar_log(g);
	ParserBuildResult pr = parser_build(g);
	IfElse_T(pr, p, {
		IfElse_T(parser_parse(p, lexer0, "a bc", &g0), ast, { ast_log(ast); }, err, { FAIL_CHECK("Parser error"); });
		IfElse_T(parser_parse(p, lexer0, "ac", &g0), ast, { ast_log(ast); }, err, { FAIL_CHECK("Parser error"); });
		IfElse_T(parser_parse(p, lexer0, "bc", &g0), ast, { ast_log(ast); }, err, { FAIL_CHECK("Parser error"); });
		IfElse_T(parser_parse(p, lexer0, "a   bc", &g0), ast, { ast_log(ast); }, err, { FAIL_CHECK("Parser error"); });
		IfElse_T(parser_parse(p, lexer0, "  bc", &g0), ast, { ast_log(ast); }, err, { FAIL_CHECK("Parser error"); });
	}, err, { FAIL("Parser build failed"); });
}
