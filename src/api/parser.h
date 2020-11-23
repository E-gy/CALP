#pragma once

#include "grammar.h"

struct parser;
typedef struct parser* Parser;

Parser parser_build(Grammar gr);
