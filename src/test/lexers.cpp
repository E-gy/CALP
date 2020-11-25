#include <catch2/catch.hpp>

extern "C" {
	#include <calp/lexers.h>
	#include <ctype.h>
}

string eat_char_a(string str){
	return *str == 'a' ? str+1 : NULL;
}
string eat_char_b(string str){
	return *str == 'b' ? str+1 : NULL;
}
string eat_char_c(string str){
	return *str == 'c' ? str+1 : NULL;
}
string eat_uint(string str){
	if(!isdigit(*str)) return NULL;
	while(isdigit(*str)) str++;
	return str;
}
string eat_alpha(string str){
	if(!isalpha(*str)) return NULL;
	while(isalpha(*str)) str++;
	return str;
}
string eat_alnum(string str){
	if(!isalnum(*str)) return NULL;
	while(isalnum(*str)) str++;
	return str;
}

#define null ((void*)NULL)

SCENARIO("test eaters", "[self-test]"){
	char str[] = "abc123ddd++";
	WHEN("eating a on a"){
		REQUIRE(eat_char_a(str) == str+1);
	}
	WHEN("eating a on not a"){
		REQUIRE(eat_char_a(str+1) == null);
		REQUIRE(eat_char_a(str+5) == null);
		REQUIRE(eat_char_a(str+7) == null);
		REQUIRE(eat_char_a(str+11) == null);
	}
	WHEN("eating b on b"){
		REQUIRE(eat_char_b(str+1) == str+2);
	}
	WHEN("eating b on not b"){
		REQUIRE(eat_char_b(str) == null);
		REQUIRE(eat_char_b(str+5) == null);
		REQUIRE(eat_char_b(str+7) == null);
		REQUIRE(eat_char_b(str+11) == null);
	}
	WHEN("eating c on c"){
		REQUIRE(eat_char_c(str+2) == str+3);
	}
	WHEN("eating c on not c"){
		REQUIRE(eat_char_b(str) == null);
		REQUIRE(eat_char_b(str+5) == null);
		REQUIRE(eat_char_b(str+7) == null);
		REQUIRE(eat_char_b(str+11) == null);
	}
	WHEN("eating uint"){
		REQUIRE(eat_uint(str+3) == str+6);
		REQUIRE(eat_uint(str+4) == str+6);
		REQUIRE(eat_uint(str+5) == str+6);
	}
	WHEN("eating uint on not digits"){
		REQUIRE(eat_uint(str+6) == null);
		REQUIRE(eat_uint(str) == null);
		REQUIRE(eat_uint(str+11) == null);
	}
	WHEN("eating letters"){
		REQUIRE(eat_alpha(str+0) == str+3);
		REQUIRE(eat_alpha(str+1) == str+3);
		REQUIRE(eat_alpha(str+2) == str+3);
		REQUIRE(eat_alpha(str+6) == str+9);
		REQUIRE(eat_alpha(str+7) == str+9);
		REQUIRE(eat_alpha(str+8) == str+9);
	}
	WHEN("eating letters on not letters"){
		REQUIRE(eat_alpha(str+3) == null);
		REQUIRE(eat_alpha(str+4) == null);
		REQUIRE(eat_alpha(str+5) == null);
		REQUIRE(eat_alpha(str+9) == null);
		REQUIRE(eat_alpha(str+10) == null);
		REQUIRE(eat_alpha(str+11) == null);
	}
	WHEN("eating alnum"){
		for(size_t i = 0; i < 9; i++) REQUIRE(eat_alnum(str+i) == str+9);
	}
	WHEN("eating alnum on not alnum"){
		REQUIRE(eat_alnum(str+9) == null);
		REQUIRE(eat_alnum(str+10) == null);
		REQUIRE(eat_alnum(str+11) == null);
	}
}

