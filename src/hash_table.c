#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

unsigned int hash(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

void hash_insert(const char *key, FirstFollowSet set) {
    unsigned int idx = hash(key);
    HashNode *node = (HashNode*)malloc(sizeof(HashNode));
    strcpy(node->key, key);
    node->set = set;
    node->next = hashTable[idx];
    hashTable[idx] = node;
}

FirstFollowSet* hash_get(const char *key) {
    unsigned int idx = hash(key);
    HashNode *node = hashTable[idx];
    while (node) {
        if (strcmp(node->key, key) == 0)
            return &node->set;
        node = node->next;
    }
    return NULL;
}

FirstFollowSet createSet(const char **first, int firstCount, const char **follow, int followCount) {
    FirstFollowSet set;
    set.first = malloc(MAX_SET_SIZE * sizeof(char*));
    set.follow = malloc(MAX_SET_SIZE * sizeof(char*));
    set.firstCount = firstCount;
    set.followCount = followCount;
    for (int i = 0; i < firstCount; i++) {
        set.first[i] = malloc((strlen(first[i]) + 1) * sizeof(char));
        strcpy(set.first[i], first[i]);
    }
    for (int i = 0; i < followCount; i++) {
        set.follow[i] = malloc((strlen(follow[i]) + 1) * sizeof(char));
        strcpy(set.follow[i], follow[i]);
    }
    return set;
}

int insert_first_follow() {
    // FIRST and FOLLOW sets for all non-terminals

    hash_insert("<programa>", createSet(
        (const char*[]){"CONST", "VAR", "PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 8,
        NULL, 0
    ));

    hash_insert("<bloco>", createSet(
        (const char*[]){"CONST", "VAR", "PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 8,
        (const char*[]){ ".", ";", "END", "THEN", "DO" }, 5
    ));

    hash_insert("<declaracao>", createSet(
        (const char*[]){"CONST", "VAR", "PROCEDURE"}, 3,
        (const char*[]){"ident", "CALL", "BEGIN", "IF", "WHILE"}, 5
    ));

    hash_insert("<constante>", createSet(
        (const char*[]){"CONST"}, 1,
        (const char*[]){"VAR", "PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 6
    ));

    hash_insert("<mais_const>", createSet(
        (const char*[]){","}, 1,
        (const char*[]){";"}, 1
    ));

    hash_insert("<variavel>", createSet(
        (const char*[]){"VAR"}, 1,
        (const char*[]){"PROCEDURE", "ident", "CALL", "BEGIN", "IF", "WHILE"}, 5
    ));

    hash_insert("<mais_var>", createSet(
        (const char*[]){","}, 1,
        (const char*[]){";"}, 1
    ));

    hash_insert("<procedimento>", createSet(
        (const char*[]){"PROCEDURE"}, 1,
        (const char*[]){"ident", "CALL", "BEGIN", "IF", "WHILE"}, 5
    ));

    hash_insert("<comando>", createSet(
        (const char*[]){"ident", "CALL", "BEGIN", "IF", "WHILE"}, 5,
        (const char*[]){";", "END", "THEN", "DO"}, 4
    ));

    hash_insert("<mais_cmd>", createSet(
        (const char*[]){";"}, 1,
        (const char*[]){"END"}, 1
    ));

    hash_insert("<expressao>", createSet(
        (const char*[]){"+", "-", "ident", "numero", "("}, 5,
        (const char*[]){"THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 11
    ));

    hash_insert("<operador_unario>", createSet(
        (const char*[]){"+", "-"}, 2,
        (const char*[]){"ident", "numero", "("}, 3
    ));

    hash_insert("<termo>", createSet(
        (const char*[]){"ident", "numero", "("}, 3,
        (const char*[]){"+", "-", "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 13
    ));

    hash_insert("<mais_termos>", createSet(
        (const char*[]){"+", "-"}, 2,
        (const char*[]){ "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 11
    ));

    hash_insert("<fator>", createSet(
        (const char*[]){"ident", "numero", "("}, 3,
        (const char*[]){ "*", "/", "+", "-", "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 15
    ));

    hash_insert("<mais_fatores>", createSet(
        (const char*[]){ "*", "/"}, 2,
        (const char*[]){ "+", "-", "THEN", "DO", "=", "<>", "<", "<=", ">", ">=", ")", ";", "END"}, 13
    ));

    hash_insert("<condicao>", createSet(
        (const char*[]){"ODD", "ident", "numero", "("}, 4,
        (const char*[]){ "THEN", "DO"}, 2
    ));

    hash_insert("<relacional>", createSet(
        (const char*[]){"=", "<>", "<", "<=", ">", ">="}, 6,
        (const char*[]){ "ident", "numero", "(" }, 3
    ));

    return 0;
}