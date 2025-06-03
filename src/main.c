#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "headers.h"


// Sets the list of reserved words
static const char reserved_words[NUM_RESERVED_WORDS][MAX_RESERVED_WORD_LENGHT] = {
    "CONST", "VAR", "PROCEDURE", "BEGIN", "END",
    "CALL", "WHILE", "DO", "ODD", "IF", "THEN"
};

// hashTable is a global variable
HashNode** hashTable = NULL;

/**
 * @brief Checks if the input string is a reserved word
 * @param word input string
 * @return 1 if reserved word, 0 otherwise
 */
int check_if_reserved_word(char *word){
	for(int i = 0; i < NUM_RESERVED_WORDS; i++)
		if(!strcmp(word, reserved_words[i])) return 1;
	
	return 0;
}

/**
 * @brief Lexical analyser function
 * @param input_file input file with the code to be compiled
 * @param transition_matrix transition matrix of the Mealy automata used
 * @return struct with the token read, its class and an int indicating if EOF was reached
 */
LexicalOutput lexical_analyser(FILE *input_file, Transition** transition_matrix){
	// Starts analysis in the 0 state
	State current_state;
	current_state.number = 0;
	current_state.is_final = 0;

	// Instantiates a standard output
	LexicalOutput lexical_output;
	lexical_output.end = 0;
	lexical_output.token[0] = '\0'; // Initialize token string
	lexical_output.class[0] = '\0'; // Initialize class string

	// Instantiates input and output variables
	char char_consumed;
	char capture_output[MAX_TOKEN_LENGHT];
	capture_output[0] = '\0'; // Initialize capture_output string

	// Loops while the char read is different from EOF
	while((char_consumed = fgetc(input_file)) != EOF){
		Transition current_transition = transition_matrix[current_state.number][(unsigned char) char_consumed];

		if(current_transition.next_state.is_final){
			switch(current_transition.num_outputs){
				case 1: // Outputs only its class (e.g., "identificador", "numero")
					// capture_output should contain the accumulated token string
					if(check_if_reserved_word(capture_output)) {
						strcpy(lexical_output.class, capture_output);
					} else {
						// Ensure current_transition.output is not null before copying
						if (current_transition.output) {
							strcpy(lexical_output.class, current_transition.output);
						} else {
							lexical_output.class[0] = '\0'; // Safety: if output is NULL
						}
					}
					strcpy(lexical_output.token, capture_output);
					break;
				case 2: // Outputs token and class (e.g., "+ simb_mais")
					{
						// Ensure current_transition.output is not null before strdup
						if (current_transition.output) {
							char *original_fsm_output_dup = strdup(current_transition.output);
							if (!original_fsm_output_dup) { perror("strdup failed in lexer case 2"); exit(EXIT_FAILURE); }
							char *temp_ptr_for_strsep = original_fsm_output_dup;
							char* token_part = strsep(&temp_ptr_for_strsep, " ");
							char* class_part = temp_ptr_for_strsep; // Remainder is the class

							if (token_part) {
								strcat(capture_output, token_part);
							}
							if (class_part) {
								strcpy(lexical_output.class, class_part);
							} else {
								lexical_output.class[0] = '\0'; // Ensure class is empty if not found
							}
							strcpy(lexical_output.token, capture_output);
							free(original_fsm_output_dup); // Free the string allocated by strdup
						} else {
							// Handle NULL current_transition.output for case 2 if necessary
							lexical_output.token[0] = '\0';
							lexical_output.class[0] = '\0';
						}
					}
					break;
				default:
					exit(-1);
			}

			if(current_transition.lookahead) {
				fseek(input_file, -1, SEEK_CUR);
			}

            //caso seja um caracter de espaço ou quebra de linha, ignora
            if (lexical_output.token[0] == '\0' && lexical_output.class[0] == '\0') {
                current_state.number = 0; // Reset to initial state
                current_state.is_final = 0;
                capture_output[0] = '\0';   // Clear accumulated capture
                lexical_output.token[0] = '\0'; // Ensure lexical_output is clean for this skipped iteration
                lexical_output.class[0] = '\0';
                // The character that led to this empty token is consumed.
                // The 'continue' will fetch the next character.
                continue; 
            }

			return lexical_output;
		}
		else{
			// If the state is not final, append its output to the current output token string
			if (current_transition.output != NULL && strcmp(current_transition.output, "lambda") != 0 && strcmp(current_transition.output, "\0") != 0) {
                 strcat(capture_output, current_transition.output);
            }
		}
		current_state = current_transition.next_state; // Corrected assignment
	}

	lexical_output.end = 1;
	// If EOF is reached and capture_output has content, it might be the last token.
	// This logic might need to be adjusted based on FSM design for EOF.
	if (capture_output[0] != '\0' && lexical_output.token[0] == '\0') {
	    // This is a guess; FSM should ideally define how EOF finalizes tokens.
	    // Assuming the class should be 'identificador' or 'numero' if not reserved.
	    // This part is speculative and depends heavily on your FSM's design for EOF.
	    if(check_if_reserved_word(capture_output)) {
			strcpy(lexical_output.class, capture_output);
		} else {
		    // Heuristic: if it looks like a number, class is numero, else identificador
		    // This is a simplification. Your FSM should define this.
		    bool is_num = true;
		    for (int i = 0; capture_output[i] != '\0'; i++) {
		        if (!isdigit(capture_output[i])) {
		            is_num = false; break;
		        }
		    }
		    if (is_num && capture_output[0] != '\0') strcpy(lexical_output.class, "numero");
		    else strcpy(lexical_output.class, "identificador"); // Default/fallback
		}
		strcpy(lexical_output.token, capture_output);
	}
	return lexical_output;
}

/**
 * @brief Returns a token to the input file stream by seeking back.
 * @param input_file The file stream to return the token to.
 * @param returned_token The lexical output (token) to be "unread".
 */
void return_token(FILE *input_file, LexicalOutput returned_token){
    if (returned_token.token[0] != '\0') { // Only return if it's a valid token
        long offset = strlen(returned_token.token);
        long current_pos = ftell(input_file);

        if (offset > current_pos) {
            fprintf(stderr, "Warning: Attempted to return token '%s' (length %ld) when file pointer is at %ld. Token not returned to prevent seeking before file start.\n", returned_token.token, offset, current_pos);
            return; // Prevent the erroneous seek
        }

        if (fseek(input_file, -offset, SEEK_CUR) != 0) {
            perror("Error seeking back in file");
            // Handle error appropriately, maybe exit or set an error flag
        }
    }
}

/**
 * @brief Frees the memory used by the transition matrix
 * @param transition_matrix transition matrix pointer
 */
void free_memory(Transition** transition_matrix) {
	for(int i = 0; i < 27; i++) {
		for(int j = 0; j < ASCII_EXTENDED_SIZE; j++) {
			// Checks if the output vriable can be freed
			if(strcmp(transition_matrix[i][j].output, "numero") != 0 
				&& strcmp(transition_matrix[i][j].output, "identificador") != 0 
				&& strcmp(transition_matrix[i][j].output, "<ERRO_SIMBOLO_INCOMPLETO>") != 0 
				&& strcmp(transition_matrix[i][j].output, "simb_maior_que") != 0 
				&& strcmp(transition_matrix[i][j].output, "simb_menor_que") != 0)
					// Frees the variable
					free(transition_matrix[i][j].output);
		}
		// Frees each line
		free(transition_matrix[i]);
	}
	// Frees the matrix
	free(transition_matrix);
}

/**
 * @brief Syntatic analyser function
 * @param input_file_name name of the input file with the code to be compiled
 * @param automata_path name of the file with the Mealy automata transitions
 */
void syntatic_analyser(char* input_file_name, char* automata_path){
	// Open input file
	FILE *input_file;
	input_file = fopen(input_file_name, "r");
	if(input_file == NULL){
		printf("Error: Unable to open input file!\n");
		exit(1);
	}

	// Open output file
	FILE *output_file;
	output_file = fopen("output.txt", "w");
	if(output_file == NULL){
		printf("Error: Unable to open output file!\n");
		exit(1);
	}

	// Load transition table representing the automata
	Transition **transition_matrix = csv_parser(automata_path);
	
	// Starts analysis from programa
	programa(input_file, transition_matrix);

	// Closes input file
	fclose(input_file);
	// Closes output file
	fclose(output_file);
}

void main(int argc, char** argv){
	// Checks if the number of inputs is valid
	if(argc != 2){
		printf("ERROR! This analyser takes one and only one argument, which is the file name!\n");
		exit(1);
	}

	hashTable = calloc(sizeof(HashNode*), HASH_SIZE);
	// Insert FIRST and FOLLOW sets into the hash table
    insert_first_follow();

	// Defines the input as the name of the input file
	char *input_file_name = argv[1];
	// Defines the automata transition table's file path
	char automata_path[] = "./data/transition_matrix.csv";

	printf("\n========== STARTING ANALYSIS! ========= \n\n");

	Transition **transition_matrix = csv_parser(automata_path);

	FILE *input_file;
	input_file = fopen(input_file_name, "r");
	if(input_file == NULL){
		printf("Error: Unable to open input file!\n");
		exit(1);
	}

	// Starts syntatic analysis
	syntatic_analyser(input_file_name, automata_path);
}
