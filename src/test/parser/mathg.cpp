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

#include <calp/ast.h>

Result_T(matheval_result, long, string_v);
#define MathResult struct matheval_result

static MathResult math_eval(AST ast){
	switch(ast->type){
		case ast->AST_GROUP:
			if(ast->d.group.groupId == ng) switch(ast->d.group.cc){
				case 1:	return math_eval(ast->d.group.children[0]);
				case 3: return math_eval(ast->d.group.children[1]);
				default: return Error_T(matheval_result, {"Invalid construction"});
			} else if(ast->d.group.groupId == adds) switch(ast->d.group.cc){
				case 1:	return math_eval(ast->d.group.children[0]);
				case 3: {
					const AST opast = ast->d.group.children[1];
					const TerminalSymbolId op = opast->type == ast->AST_GROUP && opast->d.group.groupId == pOm ? opast->d.group.children[0]->d.leaf.symbolId : NULL;
					if(op != plus && op != minus) return Error_T(matheval_result, {"Invalid operator (must be +-)"});
					MathResult el = math_eval(ast->d.group.children[0]);
					if(!IsOk_T(el)) return el;
					MathResult er = math_eval(ast->d.group.children[2]);
					if(!IsOk_T(er)) return er;
					return Ok_T(matheval_result, op == plus ? el.r.ok + er.r.ok : el.r.ok - er.r.ok);
				}
				default: return Error_T(matheval_result, {"Invalid construction"});
			} else if(ast->d.group.groupId == muls) switch(ast->d.group.cc){
				case 1:	return math_eval(ast->d.group.children[0]);
				case 3: {
					const AST opast = ast->d.group.children[1];
					const TerminalSymbolId op = opast->type == ast->AST_GROUP && opast->d.group.groupId == tOd ? opast->d.group.children[0]->d.leaf.symbolId : NULL;
					if(op != times && op != divide) return Error_T(matheval_result, {"Invalid operator (must be */)"});
					MathResult el = math_eval(ast->d.group.children[0]);
					if(!IsOk_T(el)) return el;
					MathResult er = math_eval(ast->d.group.children[2]);
					if(!IsOk_T(er)) return er;
					return Ok_T(matheval_result, op == times ? el.r.ok * er.r.ok : el.r.ok / er.r.ok);
				}
				default: return Error_T(matheval_result, {"Invalid construction"});
			} else if(ast->d.group.groupId == entry) return math_eval(ast->d.group.children[0]);
			else return Error_T(matheval_result, {"Unknown group"});
		case ast->AST_LEAF: {
			if(ast->d.leaf.symbolId != number) return Error_T(matheval_result, {"Leaf symbol is not a number"});
			string_mut ep = ast->d.leaf.val;
			long v = strtol(ep, &ep, 10);
			if(ep == ast->d.leaf.val) return Error_T(matheval_result, {"Not a number"});
			return Ok_T(matheval_result, v);
		}
		default: return Error_T(matheval_result, {"._."});
	}
}
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
					IfElse_T(parser_parse(p, lexer0, "12", &entry), ast, {
						ast_log(ast);
						AND_THEN("the result evaluates correctly"){
							IfElse_T(math_eval(ast), v, {
								CHECK(v == 12);
							}, err, { FAIL_CHECK_FMT("Eval error - %s", err.s); });
						}
						ast_destroy(ast);
					}, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer0, "12+25", &entry), ast, {
						ast_log(ast);
						AND_THEN("the result evaluates correctly"){
							IfElse_T(math_eval(ast), v, {
								CHECK(v == (12+25));
							}, err, { FAIL_CHECK_FMT("Eval error - %s", err.s); });
						}
						ast_destroy(ast);
					}, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer0, "(-21*13/2)*((12/2-25*4)-1)", &entry), ast, {
						ast_log(ast);
						AND_THEN("the result evaluates correctly"){
							IfElse_T(math_eval(ast), v, {
								CHECK(v == ((-21*13/2)*((12/2-25*4)-1)));
								THEN("on clone too"){
									AST clone = ast_clone(ast); //yes, i know. the lamest way to test cover ast cloning ¯\_(ツ)_/¯
									REQUIRE(!!clone);
									REQUIRE(OrElse_T(math_eval(clone), 0) == ((-21*13/2)*((12/2-25*4)-1)));
									ast_destroy(clone);
								}
							}, err, { FAIL_CHECK_FMT("Eval error - %s", err.s); });
						}
						ast_destroy(ast);
					}, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
					IfElse_T(parser_parse(p, lexer0, "120-15-29*2-13", &entry), ast, {
						ast_log(ast);
						AND_THEN("the result evaluates correctly"){
							IfElse_T(math_eval(ast), v, {
								CHECK(v == (120-15-29*2-13));
								THEN("on clone too"){
									AST clone = ast_clone(ast); //yes, i know. the lamest way to test cover ast cloning ¯\_(ツ)_/¯
									REQUIRE(!!clone);
									REQUIRE(OrElse_T(math_eval(clone), 0) == (120-15-29*2-13));
									ast_destroy(clone);
								}
							}, err, { FAIL_CHECK_FMT("Eval error - %s", err.s); }); 
						}
						ast_destroy(ast);
					}, err, { FAIL_CHECK_FMT("Parser error - %s", err.s); });
				}
				parser_destroy(p);
			}, err, { FAIL_FMT("Parser build failed - %s", err.s); });
		}
	}
}
