#pragma once

#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>

typedef bool Result;
#define Ok false
#define Error true
#define IsOk(result) (result == Ok)
#define IsNotOk(result) !IsOk(result)

#define Result_T(T, okt, errt) struct T { Result result; union { okt ok; errt error; } r; } 
#define Ok_T(T,...) ((struct T){Ok, {.ok=__VA_ARGS__}})
#define Error_T(T,...) ((struct T){Error, {.error=__VA_ARGS__}})
#define IsOk_T(resultt) IsOk(resultt.result)
#define IsNotOk_T(resultt) IsNotOk(resultt.result)
#define IfOk_T(result, var, closure) do{ if(IsOk_T(result)){ typeof(result.r.ok) var = result.r.ok; closure } }while(0)
#define IfError_T(result, var, closure) do{ if(IsNotOk_T(result)){ typeof(result.r.error) var = result.r.error; closure } }while(0)

typedef const char* string;
typedef char* string_mut;
struct string_v {
	char s[128];
};
#define string_v struct string_v
