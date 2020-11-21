#include "../main/grammaro.h"
#include "../main/gramdef.h"
#include "../main/parser.h"
#include "../main/parserp.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

DEF_SYMBOL_TERMINAL(symb_eof, { return !*str ? str : NULL; })
DEF_SYMBOL_TERMINAL(symb_a, { return str[0] == 'a' ? str+1 : NULL; })
DEF_SYMBOL_TERMINAL(symb_b, { return str[0] == 'b' ? str+1 : NULL; })
DEF_GROUP(grp_0, RULE(SYMBOL_T(symb_a); SYMBOL_T(symb_eof)); RULE(SYMBOL_T(symb_b); SYMBOL_G(grp_0)))
DEF_GRAMMAR(grm_basic, GROUP(grp_0))

int main(){
	Grammar g = grm_basic();
	printf("grammar: %p\n", g);
	grammar_log(g);
	Parser p = parser_build(g);
	if(!p) return 1;
	ast_log(parseraw(p, "a", &grp_0));
	ast_log(parseraw(p, "bba", &grp_0));
}
