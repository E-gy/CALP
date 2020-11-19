#include "../main/gramdef.h"

#include <stdlib.h>
#include <stdio.h>

DEF_SYMBOL_TERMINAL(symb_eof, { return !str || !*str; })
DEF_SYMBOL_TERMINAL(symb_a, { return streq(str, "a"); })
DEF_SYMBOL_TERMINAL(symb_b, { return streq(str, "b"); })
DEF_GROUP(grp_0, RULE(SYMBOL_T(symb_a); SYMBOL_T(symb_eof)); RULE(SYMBOL_T(symb_b); SYMBOL_G(grp_0)))
DEF_GRAMMAR(grm_basic, GROUP(grp_0))

int main(){
	Grammar g = grm_basic();
	printf("grammar: %p\n", g);
	grammar_print(g);
}
