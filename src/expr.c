#include "expr.h"

extern htable gTable;
extern ht_stack_t localTables;
extern int scope;
extern int code;

extern token_t token;

prec_stack_t stack;
prec_symbs_t symb;
prec_stack_item_t *head;

datatype_t symbDt;
htable *workingTable;
int from;


datatype_t fill_none_datatype() {
    datatype_t dt;

    dt.type = NONE_DT;
    dt.nullable = false;

    return dt;
}

int get_symbol(prec_symbs_t *symbol) {
    if(!is_token_allowed()) return SYNTAX_ERROR;
    prec_symbs_t tmp;
    switch (token.type) {
        case TYPE_INT:
        case TYPE_DOUBLE:
        case TYPE_STRING:
        case TYPE_ID:
            tmp = ID;
            break;
        case TYPE_LPAR:
            tmp = LPAR;
            break;
        case TYPE_RPAR:
            tmp = RPAR;
            break;
        case TYPE_PLUS:
            tmp = PLUS;
            break;
        case TYPE_MUL:
            tmp = MUL;
            break;
        case TYPE_MINUS:
            tmp = MINUS;
            break;
        case TYPE_DIV:
            tmp = DIV;
            break;
        case TYPE_NILCOAL:
            tmp = NILCOAL;
            break;
        case TYPE_EXCL:
            tmp = EXCL;
            break;
        case TYPE_EQ:
            tmp = EQ;
            break;
        case TYPE_NEQ:
            tmp = NEQ;
            break;
        case TYPE_GT:
            tmp = GT;
            break;
        case TYPE_GE:
            tmp = GE;
            break;
        case TYPE_LT:
            tmp = LT;
            break;
        case TYPE_LE:
            tmp = LE;
            break;
        default:
            return SYNTAX_ERROR;
    }

    *symbol = tmp;
    return NO_ERRORS;
}

int get_data_type(datatype_t *datatype) {
    if(!is_token_allowed()) return SYNTAX_ERROR;
    ht_item_t *symbol;
    datatype->nullable = false;
    switch (token.type) {
        case TYPE_ID:
            symbol = symt_search(workingTable, &token.attribute.id);
            if (symbol == NULL || symbol->type == func) {
                return SEMANTIC_OTHER_ERROR;
            } else {
                datatype = &symbol->data.var->type;
                return NO_ERRORS;
            }
        case TYPE_INT:
            datatype->type = INTEGER_DT;
            break;
        case TYPE_DOUBLE:
            datatype->type = DOUBLE_DT;
            break;
        case TYPE_STRING:
            datatype->type = STRING_DT;
            break;
        default:
            datatype->type = NONE_DT;
            return NO_ERRORS;
    }
    
    return NO_ERRORS;
}

bool end_of_expression() {
    bool end;
    if (from == 0) { // from expression
        bool isFunc = false;
        if (token.type == TYPE_ID) {
            ht_item_t *it = symt_search(&gTable, &token.attribute.id);
            if (it != NULL) isFunc = it->type == func ? true : false;
        }
        bool isKeyword = token.type == TYPE_KW;
        end = isKeyword || isFunc;
    } else if (from == 1) {// from if or while statement
        end = token.type == TYPE_LBRACKET ? true : false;
    } else if (from == 2) { // from return statement
        end = token.type == TYPE_RBRACKET ? true : false;
    }

    bool isEOF = token.type == TYPE_EOF;

    return end || isEOF;
}

bool is_token_allowed() {
//    bool operator = (token.type >= TYPE_MUL && token.type <= TYPE_NILCOAL);
//    bool parentheses = token.type == TYPE_LPAR || token.type == TYPE_RPAR;
//    bool id = token.type == TYPE_ID;
//    bool consts = token.type >= TYPE_INT && token.type <= TYPE_STRING;

    bool forbidden = (token.type >= TYPE_COLON && token.type <= TYPE_UNDERSCORE) || token.type == TYPE_RBRACKET;
    return !forbidden;
}

bool is_symbol_operator(prec_symbs_t symbol) {
    return symbol > 2;
}

int shift() {
    head = prec_stack_head(&stack);
    // push stop if input symbol is id
    if (symb == ID) PUSH_STOP();

    if (head->symb == NONTERM) { // if head is nonterm, pop and push stop sign and then nonterm back
        POP_N(1);
        PUSH_STOP();
        PUSH_SYMBOL(NONTERM, fill_none_datatype());
    } else if (head->symb == LPAR && is_symbol_operator(symb)) { // if top is ) and symb is operator
        return SYNTAX_ERROR;
    }

    PUSH_SYMBOL(symb, symbDt);
    head = prec_stack_first_terminal(&stack);

    return NO_ERRORS;
}

int get_rule(prec_rules_t *rule, int symbsCnt) {
    // head - first, head->next - second, head->next->next - third
    prec_stack_item_t *first = head;
    prec_stack_item_t *second = head->next;
    prec_rules_t tmpRule;
    
    if (symbsCnt == 1) {
        if (first->symb == ID || first->type.type == INTEGER_DT || first->type.type == DOUBLE_DT || first->type.type == STRING_DT) {
            tmpRule = ID_R;
            rule = &tmpRule;
            return NO_ERRORS;
        } else return SYNTAX_ERROR;
    } else if (symbsCnt == 2) {
        if (first->symb == NONTERM && second->symb == EXCL) {
            tmpRule = EXCL_R;
        }
    } else if (symbsCnt == 3) {
        prec_stack_item_t *third = head->next->next;
        if ((third->symb == NONTERM || first->symb == NONTERM) && is_symbol_operator(second->symb)) {
            switch (second->symb) {
                case PLUS: 			    // E -> E + E
                    tmpRule = PLUS_R;
                    break;
                case MUL: 			    // E -> E * E
                    tmpRule = MUL_R;
                    break;
                case MINUS: 		    // E -> E - E
                    tmpRule = MINUS_R;
                    break;
                case DIV: 			    // E -> E / E
                    tmpRule = DIV_R;
                    break;
                case NILCOAL:           // E -> E ?? E
                    tmpRule = NILCOAL_R;
                    break;
                case EQ: 		        // E -> E == E
                    tmpRule = EQ_R;
                    break;
                case NEQ: 		        // E -> E != E
                    tmpRule = NEQ_R;
                    break;
                case GT: 		        // E -> E > E
                    tmpRule = GT_R;
                    break;
                case GE: 			    // E -> E >= E
                    tmpRule = GE_R;
                    break;
                case LT: 			    // E -> E < E
                    tmpRule = LT_R;
                    break;
                case LE: 			    // E -> E <= E
                    tmpRule = LE_R;
                    break;
                default:
                    return SYNTAX_ERROR;
            }
        } else return SYNTAX_ERROR;
    } else {
        return SYNTAX_ERROR;
    }
    *rule = tmpRule;
    genStackPush(tmpRule);      ////////////////
    return NO_ERRORS;
}

int types_are_equal(datatype_t first, datatype_t second) {
    return (first.type == second.type) && (first.nullable == second.nullable);
}

int compatibility(prec_rules_t rule, prec_stack_item_t *first, prec_stack_item_t *second) {
    if (rule == PLUS_R || rule == MUL_R || rule == MINUS_R || rule == DIV_R) {
        if (rule != PLUS_R) {
            // *, - and / can't operate with datatype string
            if (first->type.type == STRING_DT || second->type.type == STRING_DT) return SEMANTIC_TYPE_COMP_ERROR;
        }
        if (rule == PLUS_R) {
            if (types_are_equal(first->type, second->type)) {
                return NO_ERRORS;
            }
            if (first->type.type == STRING_DT || second->type.type == STRING_DT) {
                return SEMANTIC_TYPE_COMP_ERROR;
            }
        }
    }
    return NO_ERRORS;
}

datatype_t determine_result_type(prec_rules_t rule, prec_stack_item_t *first, prec_stack_item_t *second) {
    datatype_t final;

    if (rule == PLUS_R || rule == MUL_R || rule == MINUS_R || rule == DIV_R) {
        // single type for whole expression
        // we're here only in case if one or both operands are either Int or Double type
        if (types_are_equal(first->type, second->type)) final = first->type;
        else {
            if (first->type.type == DOUBLE_DT) final = first->type;
            else if (second->type.type == DOUBLE_DT) final = second->type;
        }
    }

    return final;
}

int reduce() {
    int cnt = 0;
    head = prec_stack_head(&stack);
    prec_stack_item_t *tmp = head;
    while (head->symb != STOP) {
        cnt++;
        head = head->next;
    }
    head = prec_stack_head(&stack);
    debug("yo");

    prec_rules_t rule;
    EXEC(get_rule(&rule, cnt));

    tmp = NULL;
    prec_stack_item_t *left, *right;

    switch (rule) {
        case ID_R:
            tmp = prec_stack_head(&stack);
            POP_N(2);
            PUSH_SYMBOL(NONTERM, tmp->type);
            break;
        case PLUS_R:
        case MUL_R:
        case MINUS_R:
        case DIV_R:
            left = head->next->next;
            right = head;
            EXEC(compatibility(rule, left, right));
            determine_result_type(rule, left, right);
        case NILCOAL_R:
            // a ?? b => a != nil ? a! : b
            // The expression b must match the type that’s stored inside a.
        case EXCL_R:
        case EQ_R:
        case NEQ_R:
        case GE_R:
        case GT_R:
        case LE_R:
        case LT_R:
        default:
            break;

    }

    return NO_ERRORS;
}

int equal() {
    if (symb == ID) return SEMANTIC_EXPR_ERROR;
    POP_N(2);
    PUSH_SYMBOL(NONTERM, fill_none_datatype());
    head = prec_stack_head(&stack);

    return NO_ERRORS;
}

int analyze_symbol() {
    char precedence = prec_table[head->symb][symb];

    switch (precedence) {
        case '<': // shift
            EXEC(shift());
            break;
        case '>': // reduce
            EXEC(reduce());
            break;
        case '=':
            EXEC(equal());
            break;
        case 'e':
        default:
            return SYNTAX_ERROR;
    }
    return NO_ERRORS;
}

int parse_expression(int origin) {
    // here token type is either IF, WHILE or =

    // origin = 0 if from expression, end of expression is the start of next one
    // origin = 1 if from statement if or while, end of expression is { token
    // origin = 2 from return statement

    from = origin;

    // from if: current token is the very next one after keyword "if"
    // from while: same as for if

    // logic: end of every expression is the start of next one (or EOF)

    // find working table
    workingTable = scope == 0 ? &gTable : localTables.head->table;

    prec_stack_init(&stack);
    PUSH_SYMBOL(EMPTY, fill_none_datatype());

    GET_TOKEN_SKIP_EOL(); // get first token
    if (end_of_expression()) return SYNTAX_ERROR;
    EXEC(get_symbol(&symb));
    EXEC(get_data_type(&symbDt));

    if (is_symbol_operator(symb)) return SEMANTIC_OTHER_ERROR;

    if(symb == ID) PUSH_SYMBOL(symb, symbDt);

    head = prec_stack_head(&stack);

    while (!prec_stack_is_empty(&stack)) {
        GET_TOKEN_SKIP_EOL();

        if (end_of_expression()) {
            while (!prec_stack_is_empty(&stack)) {
                head = prec_stack_head(&stack);
                EXEC(reduce());
            }
            return NO_ERRORS;
        } else {
            EXEC(get_symbol(&symb));
            EXEC(get_data_type(&symbDt));
            analyze_symbol();
        }
    }

    prec_stack_free(&stack);
    return NO_ERRORS;
}
