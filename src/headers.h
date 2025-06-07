#include <stdio.h>
#include <stdbool.h> // Added for bool type

#define NUM_RESERVED_WORDS 11
#define MAX_RESERVED_WORD_LENGHT 10
#define ASCII_EXTENDED_SIZE 256
#define MAX_TOKEN_LENGHT 256
#define MAX_SET_SIZE 20
#define MAX_TOKENS 1000

#define MAX_SYMBOLS 20
#define MAX_NAME_LEN 50
#define HASH_SIZE 100

// Mealy's machine state
typedef struct {
        int number;
        int is_error;
        int is_final;
} State;

// Mealy's machine transition;
typedef struct {
        int num_outputs;
        char* output;
        int lookahead;
        State next_state;
} Transition;

typedef struct {
	char token[MAX_TOKEN_LENGHT];
	char class[MAX_TOKEN_LENGHT];
	int end;
    int is_error;
    int line;
} LexicalOutput;

typedef struct {
    char first[MAX_SET_SIZE][20];
    int firstCount;

    char follow[MAX_SET_SIZE][20];
    int followCount;
} Data;

// Defines for First/Follow tables

#define MAX_NON_TERMINALS 50
#define MAX_TERMINALS 50


Transition** csv_parser(char* input_file_name);

//Hash table 

typedef struct {
    char **first;
    int firstCount;
    char **follow;
    int followCount;
} FirstFollowSet;

typedef struct HashNode {
    char key[MAX_NAME_LEN];
    FirstFollowSet set;
    struct HashNode* next;
} HashNode;

extern HashNode** hashTable;
//extern Transition** transition_matrix;

int insert_first_follow();

FirstFollowSet* hash_get(const char *key);

// Lexical Analysis
LexicalOutput lexical_analyser(FILE *input_file, Transition** transition_matrix);
void return_token(FILE *input_file, LexicalOutput returned_token); // New function declaration
void free_memory(Transition** transition_matrix);

void init_token_capture();

// Syntatic Analysis
void programa(FILE *input_file, Transition** transition_matrix);
void bloco(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void declaracao(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void constante(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void mais_const(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void variavel(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void mais_var(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void procedimento(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void comando(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void mais_cmd(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void expressao(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void operador_unario(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void termo(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void mais_termos(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void fator(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void mais_fatores(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void condicao(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
void relacional(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers);
