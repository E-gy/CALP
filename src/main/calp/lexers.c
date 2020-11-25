#include <calp/lexer.h>

LexerResult lexer0(string str, SelfLexingToken tok){
	string nom = tok(str);
	if(nom) return Ok_T(lexer_result, {str, nom, nom});
	return Error_T(lexer_result, {"Token refused to eat"});
}
