#pragma once

/**
 * @file fun.h
 * Grammar functions.
 * 
 * Everything you can do with the grammar itself.
 * 
 */

#include <calp/grammar.h>

/**
 * @param @ref grammar
 * @param @ref symbols
 */
void symbols_logi(Grammar grammar, Symbol symbols);

/**
 * @param @ref grammar
 */
void grammar_log(Grammar grammar);
