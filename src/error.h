/*
 * error.h
 *
 * @brief Declaration of error codes for compiler
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef PROJ_ERROR_H
#define PROJ_ERROR_H


#define NO_ERRORS 0

/*
 * chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
 */
#define LEXICAL_ERROR 1

/*
 * chyba v programu v rámci syntaktické analýzy
 * (chybná syntaxe programu, chybějící hlavička, atp.)
 */
#define SYNTAX_ERROR 2

/*
 * sémantická chyba v programu – nedefinovaná funkce, redefinice proměnné
 */
#define SEMANTIC_DEF_ERROR 3

/*
 * sémantická chyba v programu – špatný počet/typ parametrů u volání funkce
 * či špatný typ návratové hodnoty z funkce
 */
#define SEMANTIC_CALL_RET_ERROR 4

/*
 * sémantická chyba v programu – použití nedefinované nebo neinicializované proměnné
 */
#define SEMANTIC_UNDEF_VAR_ERROR 5

/*
 * 6 - sémantická chyba v programu – chybějící/přebývající výraz v příkazu
 * návratu z funkce
 */
#define SEMANTIC_EXPR_ERROR 6

/*
 * sémantická chyba typové kompatibility v aritmetických,
 * řetězcových a relačních výrazech
 */
#define SEMANTIC_TYPE_COMP_ERROR 7

/*
 * sémantická chyba odvození typu – typ proměnné nebo parametru není uveden a
 * nelze odvodit od použitého výrazu
 */
#define SEMANTIC_TYPE_ERROR 8

/*
 * ostatní sémantické chyby
 */
#define SEMANTIC_OTHER_ERROR 9

/*
 * interní chyba překladače tj. neovlivněná vstupním programem
 * (např. chyba alokace paměti atd.)
 */
#define OTHER_ERROR 99

#endif //PROJ_ERROR_H
