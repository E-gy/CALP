#pragma once

/**
 * @file fun.h
 * Everything you can do with a parser
 * 
 */

#include <calp/ptypes.h>
#include <calp/parser.h>
#include <calp/lexer.h>
#include <calp/ast.h>

Result_T(parse_result, AST, string_v);
#define ParseResult struct parse_result

/**
 * @param @ref parser
 * @param @ref lexer
 * @param @ref input
 * @param @ref g0
 * @returns @produces result
 */
ParseResult parser_parse(Parser parser, Lexer lexer, string input, GroupId g0);
