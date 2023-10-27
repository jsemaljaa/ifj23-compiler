#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "ast.h"
#include "log.h"

ast_node_t* parser_parse();

#endif // PARSER_H