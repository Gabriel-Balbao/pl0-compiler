#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "headers.h"

/**
 * @brief Populates transition matrix in the case of special input strings
 * @param symbol input string from the transition currently being analysed
 * @param new_transition struct containing transition information (except output)
 * @param transition_vector transition vector corresponding to the transition's beggining state
 */
void apply_specific_rules(char *symbol, Transition new_transition, Transition* transition_vector){
	// Populates positions corresponding to numbers, outputs corresponding number
	if(!strcmp(symbol, "digito")){
		for(int i = 48; i <= 57; i++){
			new_transition.output = malloc(sizeof(char) * 2);
			sprintf(new_transition.output, "%c", i);
			free(transition_vector[i].output);
			transition_vector[i] = new_transition;
		}
	}
	// Populates positions corresponding to letters, outputs corresponding letter
	else if(!strcmp(symbol, "letra")){
		for(int i = 0; i <= 255; i++){
			if(isalpha(i)){
				new_transition.output = malloc(sizeof(char) * 2);
				sprintf(new_transition.output, "%c", i);
				free(transition_vector[i].output);
				transition_vector[i] = new_transition;
			}
		}
	}
	// Populates positions not corresponding to numbers, outputs "numero"
	else if(!strcmp(symbol, "nao_digito")){
		new_transition.output = "numero";
		new_transition.lookahead = 1;
		for(int i = 0; i <= 255; i++){
			if(isdigit(i)) continue;
			free(transition_vector[i].output);
			transition_vector[i] = new_transition;
		}
	}
	// Populates positions not corresponding to alphanumeric characters, outputs "identificador"
	else if(!strcmp(symbol, "outro")){
		new_transition.output = "identificador";
		new_transition.lookahead = 1;
		for (int i = 0; i <= 255; i++){
			if (isalnum(i)) continue;
			free(transition_vector[i].output);
			transition_vector[i] = new_transition;
		}
	}
	// Populates positions not corresponding to '=', outputs "<ERRO_SIMBOLO_INCOMPLETO>"
	else if(!strcmp(symbol, "outro_:=")){
		new_transition.output = "<ERRO_SIMBOLO_INCOMPLETO>";
		new_transition.lookahead = 1;
		for(int i = 0; i <= 255; i++){
			if(i == '=') continue;
			free(transition_vector[i].output);
			transition_vector[i] = new_transition;
		}
	}
	// Populates positions not corresponding to '=', outputs "simb_maior_que"
	else if(!strcmp(symbol, "outro_=")){
		new_transition.output = "simb_maior_que";
		new_transition.lookahead = 1;
		for(int i = 0; i <= 255; i++){
			if(i == '=') continue;
			free(transition_vector[i].output);
			transition_vector[i] = new_transition;
		}
	}
	// Populates positions not corresponding to either '=' or '>', outputs "simb_menor_que"
	else if(!strcmp(symbol, "outro_>_=")){
		new_transition.output = "simb_menor_que";
		new_transition.lookahead = 1;
		for(int i = 0; i <= 255; i++){
			if(i == '=' || i == '>') continue;
			free(transition_vector[i].output);
			transition_vector[i] = new_transition;
		}
	}
	// Populates positions not corresponding to either '}' or '\n', outputs lambda
	else if(!strcmp(symbol, "outro_}")){
		for(int i = 0; i <= 255; i++){
			if(i == '}' || i == '\n') continue;
			new_transition.output = strdup("\0");
			free(transition_vector[i].output);
			transition_vector[i] = new_transition;
		}
	}
}

/**
 * @brief Turns plain text character into its corresponding char
 * @param symbol plain text character string
 * @return string with the correspondig char
 */
char* sanitize_token(unsigned char* symbol){
	if(!strcmp(symbol, "virgula simb_virgula"))
		return ", simb_virgula";
	else if(!strcmp(symbol, "virgula")){
		return ",";
	}else if(!strcmp(symbol, "tab"))
		return "\t";
	else if(!strcmp(symbol, "espaco"))
		return " ";
	else if(!strcmp(symbol, "quebra_linha"))
		return "\n";
	else if(!strcmp(symbol, "lambda"))
		return "\0";

	return symbol;
}

/**
 * @brief Generates the transition matrix from the csv file representing the automata
 * @param file_name name of the csv file with the transitions
 * @return transition matrix
 */
Transition** csv_parser(char* file_name){
	// Opens csv file with transitions from the Mealy automata
	FILE* csv_file = fopen(file_name, "r");
	if(csv_file == NULL){
		printf("Error reading file!\n");
		exit(0);
	}

	char line[128];
	char buffer[128];

	// Get number of states and transitions from the first line
	fgets(line, sizeof(line), csv_file);
	int number_states = atoi(strtok(line, ","));
	int number_transitions = atoi(strtok(NULL, ","));

	// Create and allocate memory for the transition matrix
	Transition** transition_matrix = malloc(number_states * sizeof(Transition*));
	
	// For each state in the automata, allocates space and populates with standard transitions
	for(int i = 0; i < number_states; i++) {		
		transition_matrix[i] = calloc(ASCII_EXTENDED_SIZE, sizeof(Transition));

		// Populate positions with a standard invalid transition
		for(int j = 0; j < ASCII_EXTENDED_SIZE; j++){
			Transition not_implemented;
			not_implemented.next_state.number = 0;
			not_implemented.next_state.is_final = 1;
			not_implemented.lookahead = 0;
			not_implemented.num_outputs = 2;
			sprintf(buffer, "%c <ERRO_CARACTERE_INVALIDO>", (char) j);
			not_implemented.output = strdup(buffer);
			transition_matrix[i][j] = not_implemented;
		}
	}

	// Discards csv header
	fgets(line, sizeof(line), csv_file);

	// Deals with each transition represented in the csv file
	while(fgets(line, sizeof(line), csv_file)){
		// Gets current state
		int current_state = atoi(strtok(line, ","));
		
		// Gets input symbol (string)
		char *symbol = strtok(NULL, ",");
		symbol = sanitize_token(symbol);

		// Gets number of outputs (1 or 2)
		int num_outputs = atoi(strtok(NULL, ","));

		// Gets the output string
		char *output = strtok(NULL, ",");
		output = sanitize_token(output);

		// Gets next state
		int next_state = atoi(strtok(NULL, ","));

		// Gets if the next state is final
		int next_state_is_final = atoi(strtok(NULL, ","));

		//Gets if next state is an error state
		int next_state_is_error = atoi(strtok(NULL, ","));


		// Generates a new transition with the information receives
		Transition new_transition;	
		new_transition.num_outputs = num_outputs;
		new_transition.next_state.number = next_state;
		new_transition.next_state.is_final = next_state_is_final;
		new_transition.next_state.is_error = next_state_is_error;

		// If the input string is a single char, simply sets the output and stores the transition in the table
		if(strlen(symbol) == 1){
			new_transition.output = strdup(output);
			free(transition_matrix[current_state][symbol[0]].output);
			transition_matrix[current_state][symbol[0]] = new_transition;
		}
		// Otherwise deals with special input strings (Ex: "letra", "digito", etc)
		else{
			apply_specific_rules(symbol, new_transition, transition_matrix[current_state]);	
		}
	}

	// Closes csv file
	fclose(csv_file);

	return transition_matrix;
}
