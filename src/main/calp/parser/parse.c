#include <calp/parser/fun.h>

#include "internals.h"
#include <calp/util/buffer.h>

static AST parser_makast(Parser p, Symbol symb, string* str){
	switch(symb->type){
		case SYMBOL_TYPE_TERM: {
			string nom = symb->val.term.id(*str);
			if(!nom){
				logdebug("'%s' failed to match \"%s\"", symb->val.term.name, *str);
				return null;
			}
			AST ret = ast_new_leaf(symb, buffer_destr(buffer_new_from(*str, nom-*str)));
			*str = nom;
			return ret;
		}
		case SYMBOL_TYPE_GROUP: {
			EntityInfo gi = entimap_get(p->ents, entinf_blank_group(symb->val.group.id));
			if(!gi) return null; //internal error
			for(FirstListElement fl = gi->i.group.firsts->first; fl; fl = fl->next){
				if(fl->symbol->type != SYMB_TERM) return null; //internal error
				if(fl->symbol->i.term.symbolId(*str)){
					Rule r = fl->r;
					size_t rsc = 0;
					for(Symbol rs = r->symbols; rs; rs = rs->next) rsc++;
					AST gast = ast_new_group(symb, gi->i.group.group, rsc);
					size_t i = 0;
					string sstr = *str;
					for(Symbol rs = r->symbols; rs; rs = rs->next){
						AST rsast = parser_makast(p, rs, &sstr);
						if(!rsast) break;
						gast->d.group.children[i++] = rsast;
					}
					if(i != rsc) ast_destroy(gast);
					else {
						*str = sstr;
						return gast;
					}
				}
			}
			if(gi->i.group.firsts->fallback){
				Rule r = gi->i.group.firsts->fallback;
				size_t rsc = 0;
				for(Symbol rs = r->symbols; rs; rs = rs->next) rsc++;
				AST gast = ast_new_group(symb, gi->i.group.group, rsc);
				size_t i = 0;
				string sstr = *str;
				for(Symbol rs = r->symbols; rs; rs = rs->next){
					AST rsast = parser_makast(p, rs, &sstr);
					if(!rsast) break;
					gast->d.group.children[i++] = rsast;
				}
				if(i != rsc) ast_destroy(gast);
				else {
					*str = sstr;
					return gast;
				}
			}
			logdebug("<%s> first list exhausted, no matches", gi->i.group.group->name);
			return null; //parser error
		}
		default: return null; //._.
	}
} 

#include <calp/grammar/build.h>

AST parseraw(Parser p, string s, GroupId g0){
	if(!p || !s || !g0) return null;
	Symbol gsymb = symbol_new_group(g0);
	if(!gsymb) return null;
	AST ast = parser_makast(p, gsymb, &s);
	symbol_destroy(gsymb);
	return ast;
}
