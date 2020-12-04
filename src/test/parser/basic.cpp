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
DEF_SYMBOL_TERMINAL(symb_eof, { return !*str ? str : NULL; })
DEF_SYMBOL_TERMINAL(symb_a, { return str[0] == 'a' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(symb_b, { return str[0] == 'b' ? str+1 : NULL; })
DEF_GROUP(grp_0, RULE(SYMBOL_T(symb_a); SYMBOL_T(symb_eof)); RULE(SYMBOL_T(symb_b); SYMBOL_G(grp_0)))
DEF_GRAMMAR(grm_basic, GROUP(grp_0))
}

SCENARIO("basic", "[basic][parsing][parser construction][grammar]"){
	Grammar g = grm_basic();
	GIVEN("basic grammar example"){
		printf("grammar: %p\n", g);
		grammar_log(g);
		ParserBuildResult pr = parser_build(g);
		THEN("parser can be built"){
			IfElse_T(pr, p, {
				AND_THEN("what can be parsed - parses"){
					IfElse_T(parser_parse(p, lexer0, "a", &grp_0), ast, { ast_log(ast.ast); ast_destroy(ast.ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer0, "bba", &grp_0), ast, { ast_log(ast.ast); ast_destroy(ast.ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
				}
				AND_THEN("what can't be parsed  - errors"){
					IfOk_T(parser_parse(p, lexer0, "", &grp_0), wast, { ast_log(wast.ast); FAIL_CHECK("^ Parsed successfully invalid input ^"); });
					IfOk_T(parser_parse(p, lexer0, "b", &grp_0), wast, { ast_log(wast.ast); FAIL_CHECK("^ Parsed successfully invalid input ^"); });
					IfOk_T(parser_parse(p, lexer0, "cc", &grp_0), wast, { ast_log(wast.ast); FAIL_CHECK("^ Parsed successfully invalid input ^"); });
					IfOk_T(parser_parse(p, lexer0, "--\n  .-.", &grp_0), wast, { ast_log(wast.ast); FAIL_CHECK("^ Parsed successfully invalid input ^"); });
					IfOk_T(parser_parse(p, lexer0, "b b	a", &grp_0), wast, { ast_log(wast.ast); FAIL_CHECK("^ Parsed successfully invalid input ^"); });
				}
				AND_THEN("what can be parsed, with an appropriate lexer - parses"){
					IfElse_T(parser_parse(p, lexer_spacebegone, "a", &grp_0), ast, { ast_log(ast.ast); ast_destroy(ast.ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer_spacebegone, "bba", &grp_0), ast, { ast_log(ast.ast); ast_destroy(ast.ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer_spacebegone, "b b	a", &grp_0), ast, { ast_log(ast.ast); ast_destroy(ast.ast); }, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
				}
				parser_destroy(p);
			}, err, { FAIL_FMT("Parser build failed"); });
		}
	}
}
