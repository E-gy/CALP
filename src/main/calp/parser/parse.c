#include <calp/parser/fun.h>

#include "internals.h"
#include <calp/util/buffer.h>

static ParseResult parser_makast(Parser p, Lexer l, Symbol symb, string* str);
static ParseResult parser_makastr(Parser p, Lexer l, string* str, Rule r, Symbol symb, Group gr);

static ParseResult parser_makastr(Parser p, Lexer l, string* str, Rule r, Symbol symb, Group gr){
	size_t rsc = 0;
	for(Symbol rs = r->symbols; rs; rs = rs->next) rsc++;
	AST gast = ast_new_group(symb, gr, rsc);
	size_t i = 0;
	string sstr = *str;
	for(Symbol rs = r->symbols; rs; rs = rs->next){
		ParseResult rsast = parser_makast(p, l, rs, &sstr);
		if(!IsOk_T(rsast)) break;
		gast->d.group.children[i++] = rsast.r.ok;
	}
	if(i != rsc) ast_destroy(gast);
	else {
		*str = sstr;
		return Ok_T(parse_result, gast);
	}
}

static ParseResult parser_makast(Parser p, Lexer l, Symbol symb, string* str){
	switch(symb->type){
		case SYMBOL_TYPE_TERM: {
			IfElse_T(l(*str, symb->val.term.id), ok, {
				AST ret = ast_new_leaf(symb, buffer_destr(buffer_new_from(ok.start, ok.end-ok.start)));
				if(!ret) return Error_T(parse_result, {"[INTERNAL] AST leaf construction failed"});
				*str = ok.next;
				return Ok_T(parse_result, ret);
			}, err, {
				logdebug("'%s' failed to match \"%s\"", symb->val.term.name, *str);
				return Error_T(parse_result, {"Terminal symbol failed to match"});
			});
		}
		case SYMBOL_TYPE_GROUP: {
			EntityInfo gi = entimap_get(p->ents, entinf_blank_group(symb->val.group.id));
			if(!gi) return Error_T(parse_result, {"[INTERNAL] Invalid state: group not in map"});
			for(FirstListElement fl = gi->i.group.firsts->first; fl; fl = fl->next){
				if(fl->symbol->type != SYMB_TERM) return Error_T(parse_result, {"[INTERNAL] Invalid state: non-terminal first list element"});
				if(fl->symbol->i.term.symbolId(*str)){
					ParseResult res = parser_makastr(p, l, str, fl->r, symb, gi->i.group.group);
					if(IsOk_T(res)) return res;
				}
			}
			if(gi->i.group.firsts->fallback){
				Rule r = gi->i.group.firsts->fallback;
				ParseResult res = parser_makastr(p, l, str, gi->i.group.firsts->fallback, symb, gi->i.group.group);
				if(IsOk_T(res)) return res;
			}
			logdebug("<%s> first list exhausted, no matches", gi->i.group.group->name);
			return Error_T(parse_result, "First list exhausted, no matches found");
		}
		default: return Error_T(parse_result, "._."); //._.
	}
} 

#include <calp/grammar/build.h>

ParseResult parser_parse(Parser p, Lexer l, string s, GroupId g0){
	if(!p || !l || !s || !g0) return Error_T(parse_result, {"Invalid args"});
	Symbol gsymb = symbol_new_group(g0);
	if(!gsymb) return Error_T(parse_result, {"Allocation failed"});
	ParseResult result = parser_makast(p, l, gsymb, &s);
	symbol_destroy(gsymb);
	return result;
}