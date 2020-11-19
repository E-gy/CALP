#include "grammaro.h"

#include "grammard.h"
#include "null.h"

#ifdef _DEBUG

#include <stdio.h>

void grammar_print(Grammar g){
	if(!g) return;
	printf("%s:\n", g->name);
	for(Group gg = g->groups; gg; gg = gg->next){
		printf("	%s:\n", gg->name);
		for(Rule r = gg->rules; r; r = r->next){
			printf("		| ");
			for(Symbol s = r->symbols; s; s = s->next){
				switch (s->type){
					case SYMBOL_TYPE_TERM:
						printf("'%s'", s->val.term.name);
						break;
					case SYMBOL_TYPE_GROUP: {
						string grn = null;
						for(Group gg = g->groups; gg && !grn; gg = gg->next) if(gg->id == s->val.group.id) grn = gg->name; 
						printf("<%s>", grn ? grn : "[UNKNOWN GROUP]");
						break;
					}
				}
				if(s->next) printf(" ");
			}
			printf("\n");
		}
	}
}

#else 

void grammar_print(Grammar grammar){}

#endif
