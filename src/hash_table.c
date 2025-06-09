#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

unsigned int hash_function(const char *str){
    // We're using djb2 (Daniel J. Bernstein) hash function to calculate the hash key
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // This line performs hash * 32 + hash, but using binary operations
    
    return hash % TABLE_SIZE;    
}

HashTable *hash_create(){
    HashTable *hash_table = malloc(sizeof(HashTable));
    if(hash_table == NULL) return NULL;
    
    hash_table->buckets = calloc(TABLE_SIZE, sizeof(HashEntry *)); // Each bucket is allocated with 0
    if(hash_table->buckets = NULL){
        free(hash_table);
        return NULL;
    }

    return hash_table;
}

int hash_insert(HashTable *hash_table, const char *symbol, SymbolInfo symbol_info){
    if(symbol == NULL) return 0;

    unsigned int index = hash_function(symbol); // Get index
    HashEntry *bucket = hash_table->buckets[index];

    // Check if symbol already exists 
    HashEntry *current = bucket;
    while(current != NULL){
        if(strcmp(current->symbol, symbol) == 0) return 0;
        
        current = current->next;
    }

    HashEntry *new_entry = malloc(sizeof(HashEntry));
    if(new_entry == NULL) return 0;

    // Copy information to new entry
    new_entry->symbol = strdup(symbol);
    new_entry->info = symbol_info;
    new_entry->next = bucket;

    // Insert new entry at the beggining of the linked list
    hash_table->buckets[index] = new_entry;

    return 1;
}


SymbolInfo *hash_get(HashTable *hash_table, const char *symbol){
    if((symbol == NULL) || (hash_table == NULL)) return NULL;

    unsigned int index = hash_function(symbol); // Get index 
    
    HashEntry *current = hash_table->buckets[index];
    while(current != NULL){
        if(strcmp(current->symbol, symbol) == 0) // If symbol found, return symbol information
            return &current->info;
            
        current = current->next;
    }

    return NULL; // Not found
}


void hash_destroy(HashTable *hash_table){
    if(hash_table == NULL) return; // Hash is already empty

    // For each bucket in hash table
    for(int i = 0; i < TABLE_SIZE; i++){
        HashEntry *current = hash_table->buckets[i];

        // For every node in linked list
        while(current != NULL){
            for(int j = 0; j < current->info.first.count; j++) // Free each first
                free(current->info.first.items[j]);

            for(int j = 0; j < current->info.follow.count; j++) // Free each follow
                free(current->info.follow.items[j]);

            free(current->info.first.items); // Free pointer to the vector of firsts
            free(current->info.follow.items); // Free pointer to the vector of followers

            free(current->symbol); // Free the symbol of the hash entry

            HashEntry *aux = current;
            current = current->next;
            free(aux); // Free the hash entry itself
        }
    }

    free(hash_table->buckets); // Free vector to buckets
    free(hash_table); // Free the hash table itself
}
