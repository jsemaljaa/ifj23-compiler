/*
 * Project: IFJ23 compiler
 *
 * expr.c
 *
 * @brief Precedence analysis implementation
 *
 * @author Vinogradova Alina <xvinog00@vutbr.cz>
 */

#include "expr.h"

extern htable gTable;
extern ht_stack_t localTables;
extern int scope;
extern int code;

extern token_t token;

// Precedence stack
prec_stack_t stack;

// Variables to represent current token as precedence stack symbols
prec_symbs_t symb;
datatype_t symbDt;

// Head of a precedence stack
prec_stack_item_t *head;

// Variable to help detect the end of an expression
int from;

// Flag used to determine, whether we should get next token in an expression
// or no, because we still have to process current
bool flag;

void debug_print_stack() {
    debug("stack");
    prec_stack_item_t *it = prec_stack_head(&stack);
    while (it != NULL) {
        debug("%s", symbol_to_text(it->symb));
        it = it->next;
    }
}

datatype_t fill_none_datatype() {
    datatype_t dt;

    dt.type = NONE_DT;
    dt.nullable = false;

    return dt;
}

int get_symbol(prec_symbs_t *symbol) {
    if(!is_token_allowed()) return SYNTAX_ERROR;
    switch (token.type) {
        case TYPE_INT:
        case TYPE_DOUBLE:
        case TYPE_STRING:
        case TYPE_ID:
            *symbol = ID;
            break;
        case TYPE_LPAR:
            *symbol = LPAR;
            break;
        case TYPE_RPAR:
            *symbol = RPAR;
            break;
        case TYPE_PLUS:
            *symbol = PLUS;
            break;
        case TYPE_MUL:
            *symbol = MUL;
            break;
        case TYPE_MINUS:
            *symbol = MINUS;
            break;
        case TYPE_DIV:
            *symbol = DIV;
            break;
        case TYPE_NILCOAL:
            *symbol = NILCOAL;
            break;
        case TYPE_EXCL:
            *symbol = EXCL;
            break;
        case TYPE_EQ:
            *symbol = EQ;
            break;
        case TYPE_NEQ:
            *symbol = NEQ;
            break;
        case TYPE_GT:
            *symbol = GT;
            break;
        case TYPE_GE:
            *symbol = GE;
            break;
        case TYPE_LT:
            *symbol = LT;
            break;
        case TYPE_LE:
            *symbol = LE;
            break;
        default:
            return SYNTAX_ERROR;
    }


    return NO_ERRORS;
}

int get_data_type(datatype_t *datatype) {
    if(!is_token_allowed()) return SYNTAX_ERROR;
    ht_item_t *symbol;
    datatype->nullable = false;
    switch (token.type) {
        case TYPE_ID:
            symbol = find_var_in_symtables(&token.attribute.id);
            if (symbol == NULL) {
                return SEMANTIC_UNDEF_VAR_ERROR;
            } else if (symbol->type == func) {
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
            if (it != NULL) {
                isFunc = it->type == func ? true : false;
            }
        }
        bool isKeyword = false;
        if (token.type == TYPE_KW) {
            isKeyword = true;
            if (token.attribute.keyword == K_NIL) {
                isKeyword = false;
            }
        }
        end = isKeyword || isFunc;
    } else if (from == 1 || from == 2) { // from if or while statement
        end = token.type == TYPE_LBRACKET ? true : false;
    } else if (from == 3) { // from return statement
        end = token.type == TYPE_RBRACKET ? true : false;
    } else {
        end = false;
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
        datatype_t tmp = head->type;
        POP_N(1);
        PUSH_STOP();
        PUSH_SYMBOL(NONTERM, tmp);
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

    if (symbsCnt == 1) {
        // TODO: not sure if other cases besides symb == ID are needed
        if (first->symb == ID || first->type.type == INTEGER_DT || first->type.type == DOUBLE_DT || first->type.type == STRING_DT) {
            *rule = ID_R;
        } else {
            return SYNTAX_ERROR;
        }
    } else if (symbsCnt == 2) {
        if (first->symb == NONTERM && second->symb == EXCL) {
            *rule = EXCL_R;
        } else {
            return SYNTAX_ERROR;
        }
    } else if (symbsCnt == 3) {
        prec_stack_item_t *third = head->next->next;
        if ((third->symb == NONTERM || first->symb == NONTERM) && is_symbol_operator(second->symb)) {
            switch (second->symb) {
                case PLUS: 			    // E -> E + E
                    *rule = PLUS_R;
                    break;
                case MUL: 			    // E -> E * E
                    *rule = MUL_R;
                    break;
                case MINUS: 		    // E -> E - E
                    *rule = MINUS_R;
                    break;
                case DIV: 			    // E -> E / E
                    *rule = DIV_R;
                    break;
                case NILCOAL:           // E -> E ?? E
                    *rule = NILCOAL_R;
                    break;
                case EQ: 		        // E -> E == E
                    *rule = EQ_R;
                    break;
                case NEQ: 		        // E -> E != E
                    *rule = NEQ_R;
                    break;
                case GT: 		        // E -> E > E
                    *rule = GT_R;
                    break;
                case GE: 			    // E -> E >= E
                    *rule = GE_R;
                    break;
                case LT: 			    // E -> E < E
                    *rule = LT_R;
                    break;
                case LE: 			    // E -> E <= E
                    *rule = LE_R;
                    break;
                default:
                    return SYNTAX_ERROR;
            }
        } else return SYNTAX_ERROR;
    } else {
        return SYNTAX_ERROR;
    }
    return NO_ERRORS;
}

int compatibility(prec_rules_t rule, prec_stack_item_t *first, prec_stack_item_t *second) {

    // Pro operátor == platí: Pokud je první operand jiného typu než druhý operand, dochází k chybě 7.

    if (rule == PLUS_R) {
        if (compare_datatypes(first->type, second->type)) {
            return NO_ERRORS;
        }
    }

    switch (rule) {
        case PLUS_R:
        case MUL_R:
        case MINUS_R:
        case DIV_R:
            // *, - and / can't operate with datatype string
            if (first->type.type == STRING_DT || second->type.type == STRING_DT) return SEMANTIC_TYPE_COMP_ERROR;
            break;
        case EQ_R:
        case NEQ_R:
        case GT_R:
        case GE_R:
        case LT_R:
        case LE_R:
            if (!compare_datatypes(first->type, second->type)) return SEMANTIC_TYPE_COMP_ERROR;
        default:
            break;
    }

    return NO_ERRORS;
}

datatype_t determine_result_type(prec_rules_t rule, prec_stack_item_t *first, prec_stack_item_t *second) {

    datatype_t final;

    if (rule == PLUS_R || rule == MUL_R || rule == MINUS_R || rule == DIV_R) {
        // single type for whole expression
        // we're here only in case if one or both operands are either Int or Double type
        if (compare_datatypes(first->type, second->type)) {
            final = first->type;
        }
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
    while (tmp->symb != STOP) {
        cnt++;
        tmp = tmp->next;
    }

    prec_rules_t rule;
    EXEC(get_rule(&rule, cnt));

    datatype_t tmpDT;
    tmp = NULL;
    prec_stack_item_t *left, *right;

    switch (rule) {
        case ID_R:
            tmp = prec_stack_head(&stack);
            tmpDT = tmp->type;
            POP_N(2);
            PUSH_SYMBOL(NONTERM, tmpDT);
            break;
        case PLUS_R:
        case MUL_R:
        case MINUS_R:
        case DIV_R:
        case EQ_R:
        case NEQ_R:
        case GE_R:
        case GT_R:
        case LE_R:
        case LT_R:
            left = head->next->next;
            right = head;
            EXEC(compatibility(rule, left, right));
            tmpDT = determine_result_type(rule, left, right);
            POP_N(4);
            PUSH_SYMBOL(NONTERM, tmpDT);
            break;
        case NILCOAL_R:
            // a ?? b => a != nil ? a! : b
            // The expression b must match the type that’s stored inside a.
        case EXCL_R:
        default:
            break;
    }

    head = prec_stack_first_terminal(&stack);
    return NO_ERRORS;
}

int equal() {
    if (symb == ID) return SEMANTIC_EXPR_ERROR;
    POP_N(2);
    PUSH_SYMBOL(NONTERM, fill_none_datatype());
    head = prec_stack_first_terminal(&stack);

    return NO_ERRORS;
}

int analyze_symbol() {
    EXEC(get_symbol(&symb));
    EXEC(get_data_type(&symbDt));

    char precedence = prec_table[head->symb][symb];

    switch (precedence) {
        case '<': // shift
            EXEC(shift());
            flag = false;
            break;
        case '>': // reduce
            EXEC(reduce());
            flag = true;
            break;
        case '=':
            EXEC(equal());
            flag = false;
            break;
        case 'e':
        default:
            return SYNTAX_ERROR;
    }
    return NO_ERRORS;
}

int finish_expr(datatype_t *resultDT) {
    // input is empty
    while (!prec_stack_is_empty(&stack)) {
        head = prec_stack_head(&stack);
//        debug_print_stack();
        EXEC(reduce());
    }

    // stack is empty implemented so at the head of the stack will always be resulting NONTERM
    // which also contains resulting datatype

    head = prec_stack_head(&stack);
    *resultDT = head->type;

    prec_stack_free(&stack);
    return NO_ERRORS;
}

int parse_expression(int origin, datatype_t *resultDT) {
    // here token type is either IF, WHILE or =

    // logic: end of every (besides if and while) expression is the start of next one (or EOF)
    // origin = 0 from expression, end of expression is the start of next one
    // origin = 1 from statement while, end of expression is { token
    // origin = 2 from statement if, end of expression is { token
    // origin = 3 from return statement

    from = origin;

    // from expression or return (origin = 0): end of expression is the start of next one
    // from while (origin = 1): current token is the very next one after keyword "while"
    // from if (origin = 2): current token is the token after keyword
    //                       first token of an expression if we're here


    prec_stack_init(&stack);
    PUSH_SYMBOL(EMPTY, fill_none_datatype());

    if (from != 2) GET_TOKEN_SKIP_EOL(); // get first token
    if (end_of_expression()) return SYNTAX_ERROR;


    EXEC(get_symbol(&symb));
    EXEC(get_data_type(&symbDt));

    if (is_symbol_operator(symb) || symb == RPAR) return SEMANTIC_OTHER_ERROR;

    if(symb == ID || symb == LPAR) {
        PUSH_STOP();
    }
    PUSH_SYMBOL(symb, symbDt);

    head = prec_stack_head(&stack);

    while (true) {
        if (!flag) {
            GET_TOKEN_SKIP_EOL();
        }

        if (end_of_expression()) {
            EXEC(finish_expr(resultDT));
            return NO_ERRORS;
        } else {
            EXEC(analyze_symbol());
        }
    }
}