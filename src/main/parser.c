#include "parser.h"

#include "grammard.h"
#include <util/null.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/log.h>
#include <util/string.h>
#include <util/result.h>

struct entinf;
typedef struct entinf* EntityInfo;

struct groupfl;
typedef struct groupfl* FirstList;

struct groupfle;
typedef struct groupfle* FirstListElement;

struct groupfl {
	FirstListElement first;
	Rule fallback;
};

struct groupfle {
	/** @ref */ EntityInfo symbol;
	/** @ref */ Rule r;
	FirstListElement next;
};

static FirstList FirstList_new(){
	new(FirstList, l);
	*l = (struct groupfl){null, null};
	return l;
}

static FirstList FirstList_add(FirstList l, EntityInfo symbol, Rule r){
	if(!l) return null;
	new(FirstListElement, e);
	*e = (struct groupfle){symbol, r, l->first};
	l->first = e;
	return l;
}

struct entinf {
	enum {
		SYMB_TERM,
		SYMB_GROUP,
		RULE,
	} type;
	bool init;
	bool nullable;
	union {
		struct {
			TerminalSymbolId symbolId;
			Symbol symbol;
		} term;
		struct {
			GroupId groupId;
			Group group;
			FirstList firsts;
		} group;
		struct {
			Rule rule;
		} rule;
	} i;
	EntityInfo mapnext;
};

#define entinf_blank_term(s) ((struct entinf){SYMB_TERM, false, false, {.term = {s, null}}, null})
#define entinf_blank_group(g) ((struct entinf){SYMB_GROUP, false, false, {.group = {g, null, null}}, null})
#define entinf_blank_rule(r) ((struct entinf){RULE, false, false, {.rule = {r}}, null})

#define entinf_blank_symbol(_s) (_s ? _s->type == SYMBOL_TYPE_TERM ? entinf_blank_term(_s->val.term.id) : _s->type == SYMBOL_TYPE_GROUP ? entinf_blank_group(_s->val.group.id) : entinf_blank_rule(null) : entinf_blank_rule(null))

#define ENTIMAPS 256

typedef size_t hash_t;

static hash_t ghash(void* p){
	const char* pc = p;
	const char* p0 = p;
	const size_t red = pc-p0;
	hash_t h = 0;
	for(size_t b = 0; b < sizeof(size_t); b++) h ^= (red>>(b*8ULL))&0xFFULL;
	return h;
}

static hash_t entinf_hash(struct entinf i){
	switch(i.type){
		case SYMB_TERM: return ghash(i.i.term.symbol);
		case SYMB_GROUP: return ghash(i.i.group.group);
		case RULE: return ghash(i.i.rule.rule);
		default: return 0;
	}
}

static bool entinf_eq(EntityInfo i, struct entinf ii){
	if(!i) return false;
	if(i->type != ii.type) return false;
	switch(i->type){
		case SYMB_TERM: return i->i.term.symbolId == ii.i.term.symbolId;
		case SYMB_GROUP: return i->i.group.groupId == ii.i.group.groupId;
		case RULE: return i->i.rule.rule == ii.i.rule.rule;
		default: return false;
	}
}

struct entimap {
	EntityInfo ents[ENTIMAPS];
};
typedef struct entimap* EntitiesMap;

static EntitiesMap entimap_new(){
	new(EntitiesMap, m);
	memset(m->ents, 0, ENTIMAPS*sizeof(*m->ents));
	return m;
}

static EntityInfo entimap_get(EntitiesMap m, struct entinf ii){
	if(!m) return null;
	const hash_t h = entinf_hash(ii);
	for(EntityInfo i = m->ents[h]; i; i = i->mapnext) if(entinf_eq(i, ii)) return i;
	return null;
}

static EntityInfo entimap_add(EntitiesMap m, struct entinf ii){
	if(!m) return null;
	const hash_t h = entinf_hash(ii);
	EntityInfo* i = &m->ents[h];
	for(; *i; i = &((*i)->mapnext)) if(entinf_eq(*i, ii)) return *i;
	new(EntityInfo, nii);
	*nii = ii;
	return *i = nii;
}

#ifdef _DEBUG

#include "log.h"
#include "grammaro.h"

static void entimap_log(Grammar gr, EntitiesMap m){
	logf("%s:", gr->name);
	for(size_t j = 0; j < ENTIMAPS; j++) for(EntityInfo i = m->ents[j]; i; i = i->mapnext) if(i->type == SYMB_TERM){
		logf("	'%s':", i->i.term.symbol->val.term.name);
		logf("		init: %s", i->init ? "true" : "false");
		logf("		nullable: %s", i->nullable ? "true" : "false");
	}
	for(size_t j = 0; j < ENTIMAPS; j++) for(EntityInfo i = m->ents[j]; i; i = i->mapnext) if(i->type == SYMB_GROUP){
		logf("	<%s>:", i->i.group.group->name);
		logf("		init: %s", i->init ? "true" : "false");
		logf("		nullable: %s", i->nullable ? "true" : "false");
		log("		firsts: ");
		for(FirstListElement f = i->i.group.firsts->first; f; f = f->next) logf("			'%s' -> %p", f->symbol->i.term.symbol->val.term.name, f->r);
		logf("		fallback: %p", i->i.group.firsts->fallback);
		log("		rules:");
		for(Rule r = i->i.group.group->rules; r; r = r->next){
			logif("			%p: ", r);
			EntityInfo ri = entimap_get(m, entinf_blank_rule(r));
			if(!ri){
				log("<UNKNOWN RULE>");
				continue;
			}
			symbols_logi(gr, r->symbols);
			log("");
			logf("				init: %s", ri->init ? "true" : "false");
			logf("				nullable: %s", ri->nullable ? "true" : "false");
		}
	}
}

#else

static void entimap_log(Grammar gr, EntitiesMap m){}

#endif

#define whilechanges(blk) do { bool changes; do { changes = false; blk } while(changes); } while(0)

static Result parser_build_firsts(Grammar gr, EntitiesMap m, EntityInfo gi){
	if(gi->type != SYMB_GROUP) return Error;
	if(gi->i.group.firsts) return Ok;
	if(!(gi->i.group.firsts = FirstList_new())) return Error;
	Group g = gi->i.group.group;
	for(Rule r = g->rules; r; r = r->next){
		Symbol fnn = r->symbols;
		for(; fnn; fnn = fnn->next){
			EntityInfo fnni = entimap_get(m, entinf_blank_symbol(fnn));
			if(!fnni) return Error;
			switch(fnni->type){
				case SYMB_TERM:
					if(!fnni->nullable && !FirstList_add(gi->i.group.firsts, fnni, r)) return Error;
					break;
				case SYMB_GROUP:
					if(parser_build_firsts(gr, m, fnni) != Ok) return Error;
					for(FirstListElement cpfl = fnni->i.group.firsts->first; cpfl; cpfl = cpfl->next) if(!FirstList_add(gi->i.group.firsts, cpfl->symbol, r)) return Error;
					break;
				default: break;
			}
			if(!fnni->nullable) break;
		}
		EntityInfo ri = entimap_get(m, entinf_blank_rule(r));
		if(ri && ri->nullable) gi->i.group.firsts->fallback = r;
	}
	return Ok;
}

struct parser {
	Grammar grammar;
	EntitiesMap ents;
};

Parser parser_build(Grammar gr){
	const EntitiesMap m = entimap_new();
	if(!m) return null;
	//population & nullable:symbols
	for(Group g = gr->groups; g; g = g->next){
		EntityInfo gi = entimap_add(m, entinf_blank_group(g->id));
		if(!gi) return null;
		gi->i.group.group = g;
		for(Rule r = g->rules; r; r = r->next){
			if(!entimap_add(m, entinf_blank_rule(r))) return null;
			for(Symbol s = r->symbols; s; s = s->next){
				EntityInfo si = entimap_add(m, entinf_blank_symbol(s));
				if(!si->init && si->type == SYMB_TERM){
					si->init = true;
					si->nullable = !!si->i.term.symbolId("");
					si->i.term.symbol = s;
				}
			}
		}
	}
	//nullable:rules & nullable:groups
	whilechanges({
		for(Group g = gr->groups; g; g = g->next){
			EntityInfo gi = entimap_get(m, entinf_blank_group(g->id));
			if(!gi) return null;
			if(!gi->init) changes = true;
			bool newgnul = false;
			for(Rule r = g->rules; r; r = r->next){
				EntityInfo ri = entimap_get(m, entinf_blank_rule(r));
				if(!ri) return null;
				if(!ri->init) changes = true;
				bool newrnul = true;
				for(Symbol s = r->symbols; s && newrnul; s = s->next){
					EntityInfo si = entimap_get(m, entinf_blank_symbol(s));
					if(!si) return null;
					if(!si->init) break;
					newrnul &= si->nullable;
				}
				if(newrnul != ri->nullable) changes = (ri->nullable = newrnul) || true;
				ri->init = true;
				newgnul |= ri->nullable;
			}
			if(newgnul != gi->nullable) changes = (gi->nullable = newgnul) || true;
			gi->init = true;
		}
	});
	//first(:groups)
	for(Group g = gr->groups; g; g = g->next){
		EntityInfo gi = entimap_get(m, entinf_blank_group(g->id));
		if(!gi) return null;
		if(parser_build_firsts(gr, m, gi) != Ok) return null;
	}
	entimap_log(gr, m);
	new(Parser, p);
	*p = (struct parser){gr, m};
	return p;
}

#include "parserp.h"
#include <util/buffer.h>

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
					for(Symbol rs = r->symbols; rs; rs = rs->next){
						AST rsast = parser_makast(p, rs, str);
						if(!rsast) break;
						gast->d.group.children[i++] = rsast;
					}
					if(i != rsc) ast_destroy(gast);
					else return gast;
				}
			}
			if(gi->i.group.firsts->fallback){
				Rule r = gi->i.group.firsts->fallback;
				size_t rsc = 0;
				for(Symbol rs = r->symbols; rs; rs = rs->next) rsc++;
				AST gast = ast_new_group(symb, gi->i.group.group, rsc);
				size_t i = 0;
				for(Symbol rs = r->symbols; rs; rs = rs->next){
					AST rsast = parser_makast(p, rs, str);
					if(!rsast) break;
					gast->d.group.children[i++] = rsast;
				}
				if(i != rsc) ast_destroy(gast);
				else return gast;
			}
			logdebug("<%s> first list exhausted, no matches", gi->i.group.group->name);
			return null; //parser error
		}
		default: return null; //._.
	}
} 

#include "grammarb.h"

AST parseraw(Parser p, string s, GroupId g0){
	if(!p || !s || !g0) return null;
	Symbol gsymb = symbol_new_group(g0);
	if(!gsymb) return null;
	AST ast = parser_makast(p, gsymb, &s);
	symbol_destroy(gsymb);
	return ast;
}
