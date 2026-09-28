#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "engine_header.hh"
#include "parser.tab.hh"
#include "simplex_header.hh"

// funzione ausiliaria di stampa delle soluzioni
void print_solution(const std::vector<variable>& vars, const dictionary& dict) {
    int num_user_vars = dict.name.size();
    
    std::cout << "\n=== ASSEGNAMENTO VARIABILI ===\n";
    
    for (int i = 0; i < num_user_vars; i ++) {
        std::string name = dict.name[i];
        float value = vars[i].value;
        
        std::cout << name << " = " << value << "\n";
    }
    
    std::cout << "==============================\n\n";
}

int main(int argc, char ** argv){

    // PARSING

    // cronometro
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<clause> lista_clausole;
    std::vector<std::string> symbol_table;

    yy::parser yacc_parser(lista_clausole, symbol_table);

	printf("simplex SAT solver\n----------------------\n\n");
	if(argc > 1) {
		yyin = std::fopen(argv[1], "r");

		if(yyin == NULL) {
			printf("Unable to read file from input\n");
			return -1;
		} else {
			printf("reading file from input: %s\n\n", argv[1]);
		}
	} else {
		printf("Provide an input file!\n");
        exit(-1);
	}

	yacc_parser.parse();

	if (yyin && yyin != stdin) {
        std::fclose(yyin);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "parsing completato in: " << duration.count() << " secondi\n" << std::endl;

    // POPOLA IL DIZIONARIO

    simplex_data data;

    dictionary dict = get_dictionary(symbol_table, var_ordering::CHRONOLOGICAL);

    int n_var = symbol_table.size();
    int n_clause = lista_clausole.size();

    // CARICAMENTO DATI DELLE VARIABILI

    data.vars = get_vars(dict, lista_clausole);
    data.tableau = get_tableau(dict, lista_clausole);
    data.basic.resize(n_clause);
    data.non_basic.resize(n_var);

    // popola il vettore delle non basic variable(prima partizione del vettore)
    for (int i = 0; i < n_var; i ++) {
        data.non_basic[i] = i;
    }
    // popola il vettore delle basic variable(seconda partizione del vettore)
    for (int i = 0; i < n_clause; i ++) {
        data.basic[i] = n_var + i;
    }

    start = std::chrono::high_resolution_clock::now();

    if(simplex(data)) {
        std::cout << "\n\nSAT" << std::endl;
        print_solution(data.vars, dict);
    } else {
        std::cout << "\n\nUNSAT" << std::endl;
    }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "algoritmo del simplesso completato in: " << duration.count() << " secondi" << std::endl;

	return 0;
}
