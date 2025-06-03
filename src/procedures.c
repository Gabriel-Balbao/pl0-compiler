#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

LexicalOutput get_token(FILE *input_file, Transition** transition_matrix){
	LexicalOutput current_token;
	while(1){
		current_token = lexical_analyser(input_file, transition_matrix);
		if(strcmp(current_token.token, "")) return current_token;
	}
}

FirstFollowSet add_follow(FirstFollowSet data, char **symbol, int num_symbols) {
    // Check if the symbol is already in the follow set
    for(int j = 0; j < num_symbols; j++) {
        for(int i = 0; i < data.followCount; i++) {
            if(strcmp(data.follow[i], symbol[j]) == 0) {
                break; // Symbol already exists in the follow set
            }
        }
        // Adds symbol to the follow set
        data.follow[data.followCount++] = strdup(symbol[j]);
    }

    return data;
}

/**
 * @brief Checks if a given symbol (string) is in the FOLLOW set.
 * @param data The FirstFollowSet structure containing the FOLLOW set.
 * @param symbol The symbol to check for.
 * @return 1 if the symbol is in the FOLLOW set, 0 otherwise.
 */
int is_in_follow_set(FirstFollowSet* data, const char *symbol) {
    for (int i = 0; i < data->followCount; i++) {
        if (strcmp(data->follow[i], symbol) == 0) {
            return 1; // Symbol found in follow set
        }
    }
    return 0; // Symbol not found in follow set
}

/**
 * @brief Checks if a given symbol (string) is in the FIRST set.
 * @param data The FirstFollowSet structure containing the FIRST set.
 * @param symbol The symbol to check for.
 * @return 1 if the symbol is in the FIRST set, 0 otherwise.
 */
int is_in_first_set(FirstFollowSet* data, const char *symbol) {
    for (int i = 0; i < data->firstCount; i++) {
        if (strcmp(data->first[i], symbol) == 0) {
            return 1; // Symbol found in follow set
        }
    }
    return 0; // Symbol not found in follow set
}

LexicalOutput current_token;
void readToken(FILE *input_file, Transition** transition_matrix) {
    current_token = lexical_analyser(input_file, transition_matrix);
    if(current_token.end) {
        printf("Unexpected end of file reached\n");
        exit(0);
    }
    printf("%s\n", current_token.token);
}

void print_error(char** expected, int num_expected) {
    printf("Syntatic error: expected {");
    for(int i = 0; i < num_expected - 1; i++) {
        printf("\"%s\", ", expected[i]);
    }
    printf("\"%s\"", expected[num_expected - 1]);
    printf("}, found \"%s\"\n", current_token.token);
}

void error(FILE *input_file, Transition** transition_matrix, char** expected, int num_expected, FirstFollowSet followers) {
    // Prints error
    print_error(expected, num_expected);

    // Enters panic mode
    while(!is_in_follow_set(&followers, current_token.class)) {
        readToken(input_file, transition_matrix);
    }
}

char** convert(char string[]) {
    char** vector = malloc(sizeof(char*));
    *vector = strdup(string);
    return vector;
}

void programa(FILE *input_file, Transition** transition_matrix) {
    printf("Entrou programa\n");
    // Sets initial follower list
    FirstFollowSet followers = *hash_get("<programa>");

    // Reads first symbol
    readToken(input_file, transition_matrix);
    
    // Recursion, calling bloco
    bloco(input_file, transition_matrix, add_follow(followers, convert("simb_ponto"), 1));

    // Checks if token is "simb_ponto"
    if(strcmp(current_token.class, "simb_ponto")) {
        error(input_file, transition_matrix, convert("simb_ponto"), 1, followers);
        // There isn't much to check in respect to this error
    }
    printf("Saiu programa\n");
}

void bloco(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou bloco\n");
    FirstFollowSet followers;

    // Recursion, calling declaracao
    followers = add_follow(parent_followers, hash_get("<comando>")->first, hash_get("<comando>")->firstCount);
    declaracao(input_file, transition_matrix, followers);

    // Recursion, calling comando
    followers = parent_followers;
    comando(input_file, transition_matrix, followers);
    printf("Saiu bloco\n");
}

void declaracao(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou declaracao\n");
    FirstFollowSet followers;

    // Recursion, calling constante
    followers = add_follow(parent_followers, hash_get("<variavel>")->first, hash_get("<variavel>")->firstCount);
    constante(input_file, transition_matrix, followers);

    // Recursion, calling variavel
    followers = add_follow(parent_followers, hash_get("<procedimento>")->first, hash_get("<procedimento>")->firstCount);
    variavel(input_file, transition_matrix, followers);

    // Recursion, calling procedimento
    followers = parent_followers;
    procedimento(input_file, transition_matrix, followers);
    printf("Saiu declaracao\n");
}

void constante(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou constante\n");
    FirstFollowSet followers;

    // Checks if token is "CONST"
    if(strcmp(current_token.class, "CONST")) {
        return; // Consider lambda

        /*followers = add_follow(parent_followers, "identificador"), 1);
        error(input_file, transition_matrix, "CONST"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error("identificador"), 1);
            return;
        }*/
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "identificador"
    if(strcmp(current_token.class, "identificador")) {
        followers = add_follow(parent_followers, convert("simb_igual"), 1);
        error(input_file, transition_matrix, convert("identificador"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(convert("simb_igual"), 1);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "simb_igual"
    if(strcmp(current_token.class, "simb_igual")) {
        followers = add_follow(parent_followers, convert("numero"), 1);
        error(input_file, transition_matrix, convert("simb_igual"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(convert("numero"), 1);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "numero"
    if(strcmp(current_token.class, "numero")) {
        followers = add_follow(parent_followers, hash_get("<mais_const>")->first, hash_get("<mais_const>")->firstCount);
        error(input_file, transition_matrix, convert("numero"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(hash_get("<mais_const>")->first, hash_get("<mais_const>")->firstCount);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Recursion, calling mais_const
    followers = add_follow(parent_followers, convert("simb_ponto_virgula"), 1);
    mais_const(input_file, transition_matrix, followers);

    // Checks if token is "simb_ponto_virgula"
    if(strcmp(current_token.class, "simb_ponto_virgula")) {
        followers = parent_followers;
        error(input_file, transition_matrix, convert("simb_ponto_virgula"), 1, followers);
    }
    printf("Saiu constante\n");
}

void mais_const(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("entrou mais_const\n");
    FirstFollowSet followers;

    // Checks if token is "simb_virgula"
    if(strcmp(current_token.class, "simb_virgula")) {
        return; // Consider lambda

        /*followers = add_follow(parent_followers, "identificador"), 1);
        error(input_file, transition_matrix, "simb_virgula"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error("identificador"), 1);
            return;
        }*/
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "identificador"
    if(strcmp(current_token.class, "identificador")) {
        followers = add_follow(parent_followers, convert("simb_igual"), 1);
        error(input_file, transition_matrix, convert("identificador"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(convert("simb_igual"), 1);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "simb_igual"
    if(strcmp(current_token.class, "simb_igual")) {
        followers = add_follow(parent_followers, convert("numero"), 1);
        error(input_file, transition_matrix,convert( "simb_igual"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(convert("numero"), 1);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "numero"
    if(strcmp(current_token.class, "numero")) {
        followers = add_follow(parent_followers, hash_get("<mais_const>")->first, hash_get("<mais_const>")->firstCount);
        error(input_file, transition_matrix, convert("numero"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(hash_get("<mais_const>")->first, hash_get("<mais_const>")->firstCount);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Recursion, calling mais_const
    followers = parent_followers;
    mais_const(input_file, transition_matrix, followers);
    printf("Saiu mais_const\n");
}

void variavel(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou variavel\n");
    FirstFollowSet followers;

    // Checks if token is "VAR"
    if(strcmp(current_token.class, "VAR")) {
        return; // Consider lambda

        /*followers = add_follow(parent_followers, "identificador"), 1);
        error(input_file, transition_matrix, "VAR"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error("identificador"), 1);
            return;
        }*/
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "identificador"
    if(strcmp(current_token.class, "identificador")) {
        followers = add_follow(parent_followers, hash_get("<mais_var>")->first, hash_get("<mais_var>")->firstCount);
        error(input_file, transition_matrix, convert("identificador"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(hash_get("<mais_var>")->first, hash_get("<mais_var>")->firstCount);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Recursion, calling mais_var
    followers = parent_followers;
    mais_var(input_file, transition_matrix, followers);

    // Checks if token is "simb_ponto_virgula"
    if(strcmp(current_token.class, "simb_ponto_virgula")) {
        followers = parent_followers;
        error(input_file, transition_matrix, convert("simb_ponto_virgula"), 1, followers);
    }

    readToken(input_file, transition_matrix);
    printf("Saiu variavel\n");
}

void mais_var(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou mais_var\n");
    FirstFollowSet followers;

    // Checks if token is "simb_virgula"
    if(strcmp(current_token.class, "simb_virgula")) {
        return; // Consider that lambda was chosen

        /*followers = add_follow(parent_followers, "identificador"), 1);
        error(input_file, transition_matrix, "simb_virgula"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error("identificador"), 1);
            return;
        }*/
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "identificador"
    if(strcmp(current_token.class, "identificador")) {
        followers = add_follow(parent_followers, hash_get("<mais_var>")->first, hash_get("<mais_var>")->firstCount);
        error(input_file, transition_matrix, convert("identificador"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(hash_get("<mais_var>")->first, hash_get("<mais_var>")->firstCount);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Recursion, calling mais_var
    followers = parent_followers;
    mais_var(input_file, transition_matrix, followers);
    printf("Saiu mais_var\n");
}

void procedimento(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou procedimento\n");
    FirstFollowSet followers;

    // Checks if token is "PROCEDURE"
    if(strcmp(current_token.class, "PROCEDURE")) {
        return; // Consider lambda
        
        /*followers = add_follow(parent_followers, "identificador"), 1);
        error(input_file, transition_matrix, "PROCEDURE"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error("identificador"), 1);
            return;
        }*/
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "identificador"
    if(strcmp(current_token.class, "identificador")) {
        followers = add_follow(parent_followers, convert("simb_ponto_virgula"), 1);
        error(input_file, transition_matrix, convert("identificador"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(convert("simb_ponto_virgula"), 1);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Checks if token is "simb_ponto_virgula"
    if(strcmp(current_token.class, "simb_ponto_virgula")) {
        followers = add_follow(parent_followers, hash_get("<bloco>")->first, hash_get("<bloco>")->firstCount);
        error(input_file, transition_matrix, convert("simb_ponto_virgula"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(hash_get("<bloco>")->first, hash_get("<bloco>")->firstCount);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Recursion, calling bloco
    followers = add_follow(parent_followers, convert("simb_ponto_virgula"), 1);
    bloco(input_file, transition_matrix, followers);

    // Checks if token is "simb_ponto_virgula"
    if(strcmp(current_token.class, "simb_ponto_virgula")) {
        followers = add_follow(parent_followers, hash_get("<procedimento>")->first, hash_get("<procedimento>")->firstCount);
        error(input_file, transition_matrix, convert("simb_ponto_virgula"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(hash_get("<procedimento>")->first, hash_get("<procedimento>")->firstCount);
            return;
        }
    }

    readToken(input_file, transition_matrix);

    // Recursion, calling procedimento
    followers = parent_followers;
    procedimento(input_file, transition_matrix, followers);
    printf("Saiu procedimento\n");
}

void comando(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou comando\n");
    FirstFollowSet followers;

    // Checks if token is "identificador"
    if(!strcmp(current_token.class, "identificador")) {
        readToken(input_file, transition_matrix);

        // Checks if token is "simb_atribuicao"
        if(strcmp(current_token.class, "simb_atribuicao")) {
            followers = add_follow(parent_followers, hash_get("<expressao>")->first, hash_get("<expressao>")->firstCount);
            error(input_file, transition_matrix, convert("simb_atribuicao"), 1, followers);
            // Checks if follower found belongs to parent
            if(is_in_follow_set(&parent_followers, current_token.class)) {
                print_error(hash_get("<expressao>")->first, hash_get("<expressao>")->firstCount);
                return;
            }
        }

        readToken(input_file, transition_matrix);

        // Recursion, calling expressao
        followers = parent_followers;
        expressao(input_file, transition_matrix, followers);
    }
    // Checks if token is "CALL"
    else if(!strcmp(current_token.class, "CALL")) {
        readToken(input_file, transition_matrix);

        // Checks if token is "identificador"
        if(strcmp(current_token.class, "identificador")) {
            followers = parent_followers;
            error(input_file, transition_matrix, convert("identificador"), 1, followers);
        }
    }
    // Checks if token is "BEGIN"
    else if(!strcmp(current_token.class, "BEGIN")) {
        readToken(input_file, transition_matrix);

        // Recursion, calling comando
        followers = add_follow(parent_followers, hash_get("<mais_cmd>")->first, hash_get("<mais_cmd>")->firstCount);
        comando(input_file, transition_matrix, followers);

        // Recursion, calling mais_cmd
        followers = add_follow(parent_followers, convert("END"), 1);
        mais_cmd(input_file, transition_matrix, followers);

        // Checks if token is "END"
        if(strcmp(current_token.class, "END")) {
            followers = parent_followers;
            error(input_file, transition_matrix, convert("END"), 1, followers);
        }

        readToken(input_file, transition_matrix);
    }
    // Checks if token is "IF"
    else if(!strcmp(current_token.class, "IF")) {
        readToken(input_file, transition_matrix);

        // Recursion, calling condicao
        followers = add_follow(parent_followers, convert("THEN"), 1);
        condicao(input_file, transition_matrix, followers);

        // Checks if token is "THEN"
        if(strcmp(current_token.class, "THEN")) {
            followers = add_follow(parent_followers, hash_get("<comando>")->first, hash_get("<comando>")->firstCount);
            error(input_file, transition_matrix, convert("THEN"), 1, followers);
            // Checks if follower found belongs to parent
            if(is_in_follow_set(&parent_followers, current_token.class)) {
                print_error(hash_get("<comando>")->first, hash_get("<comando>")->firstCount);
                return;
            }
        }
        
        readToken(input_file, transition_matrix);

        // Recursion, calling comando
        followers = parent_followers;
        comando(input_file, transition_matrix, followers);
    }
    // Checks if token is "WHILE"
    else if(!strcmp(current_token.class, "WHILE")) {
        readToken(input_file, transition_matrix);

        // Recursion, calling condicao
        followers = add_follow(parent_followers, convert("DO"), 1);
        condicao(input_file, transition_matrix, followers);

        // Checks if token is "DO"
        if(strcmp(current_token.class, "DO")) {
            followers = add_follow(parent_followers, hash_get("<comando>")->first, hash_get("<comando>")->firstCount);
            error(input_file, transition_matrix, convert("DO"), 1, followers);
            // Checks if follower found belongs to parent
            if(is_in_follow_set(&parent_followers, current_token.class)) {
                print_error(hash_get("<comando>")->first, hash_get("<comando>")->firstCount);
                return;
            }
        }
        
        readToken(input_file, transition_matrix);

        // Recursion, calling comando
        followers = parent_followers;
        comando(input_file, transition_matrix, followers);
    }
    // Considers it lambda
    else {
        printf("Saiu comando\n");
        return;
    }
    printf("Saiu comando\n");
}

void mais_cmd(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou mais_cmd\n");
    FirstFollowSet followers;

    // Checks if token is "simb_ponto_virgula"
    if(strcmp(current_token.class, "simb_ponto_virgula")) {
        return; // Consider lambda
        
        /*followers = add_follow(parent_followers, hash_get("<comando>")->first, hash_get("<comando>")->firstCount);
        error(input_file, transition_matrix, "simb_ponto_virgula"), 1, followers);
        // Checks if follower found belongs to parent
        if(is_in_follow_set(&parent_followers, current_token.class)) {
            print_error(hash_get("<comando>")->first, hash_get("<comando>")->firstCount);
            return;
        }*/
    }

    readToken(input_file, transition_matrix);

    // Recursion, calling comando
    followers = add_follow(parent_followers, hash_get("<mais_cmd>")->first, hash_get("<mais_cmd>")->firstCount);
    comando(input_file, transition_matrix, followers);

    // Recursion, calling comando
    followers = parent_followers;
    mais_cmd(input_file, transition_matrix, followers);
    printf("Saiu mais_cmd\n");
}

void expressao(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou expressao\n");
    FirstFollowSet followers;

    // Recursion, calling operador_unario
    followers = add_follow(parent_followers, hash_get("<termo>")->first, hash_get("<termo>")->firstCount);
    operador_unario(input_file, transition_matrix, followers);

    // Recursion, calling termo
    followers = add_follow(parent_followers, hash_get("<mais_termos>")->first, hash_get("<mais_termos>")->firstCount);
    termo(input_file, transition_matrix, followers);

    // Recursion, calling mais_termos
    followers = parent_followers;
    mais_termos(input_file, transition_matrix, followers);
    printf("Saiu expressao\n");
}

void operador_unario(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou operador_unario\n");
    if(strcmp(current_token.class, "simb_mais") == 0)
        readToken(input_file, transition_matrix);
    else if(strcmp(current_token.class, "simb_menos") == 0)
        readToken(input_file, transition_matrix);
    else {
        printf("Saiu operador_unario\n");
        return; // lambda
    }
    printf("Saiu operador_unario\n");
}

void termo(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou termo\n");
    FirstFollowSet followers;

    // Recursion, calling fator
    followers = add_follow(parent_followers, hash_get("<mais_fatores>")->first, hash_get("<mais_fatores>")->firstCount);
    fator(input_file, transition_matrix, followers);

    // Recursion, calling mais_fatores
    followers = parent_followers;
    mais_fatores(input_file, transition_matrix, followers);
    printf("Saiu termo\n");
}

void mais_termos(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou mais_termos\n");
    FirstFollowSet followers;

    if(strcmp(current_token.class, "simb_mais") == 0) {
        readToken(input_file, transition_matrix);

        // Recursion, calling termo
        followers = add_follow(parent_followers, hash_get("<mais_termos>")->first, hash_get("<mais_termos>")->firstCount);
        termo(input_file, transition_matrix, followers);

        // Recursion, calling mais_termos
        followers = parent_followers;
        mais_termos(input_file, transition_matrix, followers);
    }
    else if(strcmp(current_token.class, "simb_menos") == 0) {
        readToken(input_file, transition_matrix);

        // Recursion, calling termo
        followers = add_follow(parent_followers, hash_get("<mais_termos>")->first, hash_get("<mais_termos>")->firstCount);
        termo(input_file, transition_matrix, followers);

        // Recursion, calling mais_termos
        followers = parent_followers;
        mais_termos(input_file, transition_matrix, followers);
    }
    else
        return; // lambda
}

void fator(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    printf("Entrou fator\n");
    FirstFollowSet followers;

    if(strcmp(current_token.class, "identificador") == 0) {
        readToken(input_file, transition_matrix);
    }
    else if(strcmp(current_token.class, "numero") == 0) {
        readToken(input_file, transition_matrix);
    }
    else if(strcmp(current_token.class, "(") == 0) {
        readToken(input_file, transition_matrix);

        // Recursion, calling expressao
        followers = add_follow(parent_followers, convert(")"), 1);
        expressao(input_file, transition_matrix, followers);

        // Checks if token is ")"
        if(strcmp(current_token.class, ")")) {
            followers = parent_followers;
            error(input_file, transition_matrix, convert(")"), 1, followers);
        }

        readToken(input_file, transition_matrix);
    }
    // Error
    else {
        followers = parent_followers;
        error(input_file, transition_matrix, hash_get("<fator>")->first, hash_get("<fator>")->firstCount, followers);
    }
}

void mais_fatores(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    FirstFollowSet followers;

    if(strcmp(current_token.class, "simb_mult") == 0) {
        readToken(input_file, transition_matrix);

        // Recursion, calling fator
        followers = add_follow(parent_followers, hash_get("<mais_fatores>")->first, hash_get("<mais_fatores>")->firstCount);
        fator(input_file, transition_matrix, followers);

        // Recursion, calling mais_fatores
        followers = parent_followers;
        mais_fatores(input_file, transition_matrix, followers);
    }
    else if(strcmp(current_token.class, "simb_div") == 0) {
        readToken(input_file, transition_matrix);

        // Recursion, calling fator
        followers = add_follow(parent_followers, hash_get("<mais_fatores>")->first, hash_get("<mais_fatores>")->firstCount);
        fator(input_file, transition_matrix, followers);

        // Recursion, calling mais_fatores
        followers = parent_followers;
        mais_fatores(input_file, transition_matrix, followers);
    }
    else
        return; // lambda
}

void condicao(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    FirstFollowSet followers;

    // Checks if token is "ODD"
    if(strcmp(current_token.class, "ODD") == 0) {
        readToken(input_file, transition_matrix);

        // Recursion, calling expressao
        followers = parent_followers;
        expressao(input_file, transition_matrix, followers);
    }
    else {
        // Recursion, calling expressao
        followers = add_follow(parent_followers, hash_get("<relacional>")->first, hash_get("<relacional>")->firstCount);
        expressao(input_file, transition_matrix, followers);

        // Recursion, calling relacional
        followers = add_follow(parent_followers, hash_get("<expressao>")->first, hash_get("<expressao>")->firstCount);
        relacional(input_file, transition_matrix, followers);

        // Recursion, calling expressao
        followers = parent_followers;
        expressao(input_file, transition_matrix, followers);
    }
}

void relacional(FILE *input_file, Transition** transition_matrix, FirstFollowSet parent_followers) {
    FirstFollowSet followers;

    if(strcmp(current_token.class, "simb_igual") == 0) {
        readToken(input_file, transition_matrix);
    }
    else if(strcmp(current_token.class, "simb_diferente") == 0) {
        readToken(input_file, transition_matrix);
    }
    else if(strcmp(current_token.class, "simb_menor_que") == 0) {
        readToken(input_file, transition_matrix);
    }
    else if(strcmp(current_token.class, "simb_menor_igual") == 0) {
        readToken(input_file, transition_matrix);
    }
    else if(strcmp(current_token.class, "simb_maior_que") == 0) {
        readToken(input_file, transition_matrix);
    }
    else if(strcmp(current_token.class, "simb_maior_igual") == 0) {
        readToken(input_file, transition_matrix);
    }
    // Error
    else {
        followers = parent_followers;
        error(input_file, transition_matrix, hash_get("<relacional>")->first, hash_get("<relacional>")->firstCount, followers);
    }
}