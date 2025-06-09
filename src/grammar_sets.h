#ifndef GRAMMARSETS_H
#define GRAMMARSETS_H

#include "hash_table.h"

// // Sets the list of reserved words
// static const char reserved_words[NUM_RESERVED_WORDS][MAX_RESERVED_WORD_LENGHT] = {
//     "CONST", "VAR", "PROCEDURE", "BEGIN", "END",
//     "CALL", "WHILE", "DO", "ODD", "IF", "THEN"
// };

SymbolInfo create_symbol_info(const char **first, int first_count, const char **follow, int follow_count);

int insert_first_follow(HashTable *hash_table);

#endif