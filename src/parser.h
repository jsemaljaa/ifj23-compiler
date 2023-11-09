#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "log.h"

bool parser_parse(token_type_t endWhen, token_t *token);

#endif // PARSER_H