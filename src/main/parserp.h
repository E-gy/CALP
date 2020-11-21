#pragma once

#include "parser.h"
#include "ast.h"

AST parseraw(Parser parser, string input, GroupId g0);
