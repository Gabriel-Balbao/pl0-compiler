#include <stdio.h>
#include <stdlib.h>

#include "grammar_sets.h"

SymbolInfo create_symbol_info(const char **first, int first_count, const char **follow, int follow_count){
    SymbolInfo symbol_info;

    // Allocate memory for vector of firsts and followers
    symbol_info.first.items = malloc(first_count * sizeof(char *));
    symbol_info.follow.items = malloc(follow_count * sizeof(char *));

    // Copy each first
    for(int i = 0; i < first_count; i++)
        symbol_info.first.items[i] = strdup(first[i]);

    // Copy each follower
    for(int i = 0; i < follow_count; i++)
        symbol_info.follow.items[i] = strdup(follow[i]);

    // Copy number of firsts and followers
    symbol_info.first.count = first_count;
    symbol_info.follow.count = follow_count;

    return symbol_info;
}


int insert_first_follow(HashTable *hash_table){
    // Insert firsts and followers in hash table

    hash_insert(hash_table, "<programa>", 
        create_symbol_info((const char*[]){"CONST", "VAR", "PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 8, NULL, 0)
    );

    hash_insert(hash_table, "<bloco>", 
        create_symbol_info((const char*[]){"CONST", "VAR", "PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 8, (const char*[]){ ".", ";", "END", "THEN", "DO" }, 5)
    );

    hash_insert(hash_table, "<declaracao>", 
        create_symbol_info((const char*[]){"CONST", "VAR", "PROCEDURE"}, 3, (const char*[]){"ident", "CALL", "BEGIN", "IF", "WHILE"}, 5)
    );

    hash_insert(hash_table, "<constante>", 
        create_symbol_info((const char*[]){"CONST"}, 1, (const char*[]){"VAR", "PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 6)
    );

    hash_insert(hash_table, "<mais_const>", 
        create_symbol_info((const char*[]){","}, 1, (const char*[]){";"}, 1)
    );

    hash_insert(hash_table, "<variavel>", 
        create_symbol_info((const char*[]){"VAR"}, 1, (const char*[]){"PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 5)
    );

    hash_insert(hash_table, "<mais_var>", 
        create_symbol_info((const char*[]){","}, 1, (const char*[]){";"}, 1)
    );

    hash_insert(hash_table, "<procedimento>", 
        create_symbol_info((const char*[]){"PROCEDURE"}, 1, (const char*[]){"ident", "CALL", "BEGIN", "IF", "WHILE"}, 5)
    );

    hash_insert(hash_table, "<comando>", 
        create_symbol_info((const char*[]){"ident", "CALL", "BEGIN", "IF", "WHILE"}, 5, (const char*[]){";", "END", "THEN", "DO"}, 4)
    );

    hash_insert(hash_table, "<mais_cmd>", 
        create_symbol_info((const char*[]){";"}, 1, (const char*[]){"END"}, 1)
    );

    hash_insert(hash_table, "<expressao>", 
        create_symbol_info((const char*[]){"+", "-", "ident", "numero", "("}, 5, (const char*[]){"THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 11)
    );

    hash_insert(hash_table, "<operador_unario>", 
        create_symbol_info((const char*[]){"+", "-"}, 2, (const char*[]){"ident", "numero", "("}, 3)
    );

    hash_insert(hash_table, "<termo>", 
        create_symbol_info((const char*[]){"ident", "numero", "("}, 3, (const char*[]){"+", "-", "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 13)
    );

    hash_insert(hash_table, "<mais_termos>", 
        create_symbol_info((const char*[]){"+", "-"}, 2, (const char*[]){ "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 11)
    );

    hash_insert(hash_table, "<fator>", 
        create_symbol_info((const char*[]){"ident", "numero", "("}, 3, (const char*[]){ "*", "/", "+", "-", "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 15)
    );

    hash_insert(hash_table, "<mais_fatores>", 
        create_symbol_info((const char*[]){ "*", "/"}, 2, (const char*[]){ "+", "-", "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 13)
    );

    hash_insert(hash_table, "<condicao>", 
        create_symbol_info((const char*[]){"ODD", "ident", "numero", "("}, 4, (const char*[]){ "THEN", "DO"}, 2)
    );

    hash_insert(hash_table, "<relacional>", 
        create_symbol_info((const char*[]){"=", "<>", "<", "<=", ">", ">="}, 6, (const char*[]){ "ident", "numero", "(" }, 3)
    );

    return 0;
}