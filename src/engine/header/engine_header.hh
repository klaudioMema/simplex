#include <iostream>
#include <limits>
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include "parser_header.hh"

#ifndef ENGINE_HEADER_H
#define ENGINE_HEADER_H

// costanti dei limiti massimi
const float INF = std::numeric_limits<float>::infinity();
const float NEG_INF = -std::numeric_limits<float>::infinity();

// DICHIARAZIONE STRUTTURE DATI

typedef struct variable variable;
typedef struct dictionary dictionary;
typedef struct compare_by_length compare_by_length;
typedef struct simplex_data simplex_data;

// tipi di variabile: di base(B) o non di base(N)
enum var_type { B, N };

// raccoglie in una struttura i dati di lavoro del simplesso
struct simplex_data {
    std::vector<variable> vars;
    std::vector<int> basic;
    std::vector<int> non_basic;
    std::vector<std::vector<float>> tableau;
};

// dati legati alle variabili: lower e upper bounds, assegnamento e tipo
struct variable {
    float upper;
    float lower;
    float value;
    var_type type;
};

// struttura dati per mappare gli id formato stringa delle variabili a id formato intero
struct dictionary {
    std::unordered_map<std::string, int> id;
    std::vector<std::string> name;
};

// tipi possibili di ordinamento delle variabili di ingresso
enum class var_ordering {
    CHRONOLOGICAL,
    ALPHABETICAL,
    REV_ALPHABETICAL,
    BY_LENGTH
};

// DICHIARAZIONE FUNZIONI

// popola il dizionario e assegna gli id interi alle variabili seguendo la regola d'ordine indicata
dictionary get_dictionary(std::vector<std::string>& symbol_table, var_ordering rule);

// alloca le informazioni di ciascuna variabile, nella prima partizione le non basic, nella seconda le basic
std::vector<variable> get_vars(const dictionary& dict, const std::vector<clause>& clause_list);

// popola il tableau recuperando i coefficienti dalle clausole
std::vector<std::vector<float>> get_tableau(dictionary& dict, std::vector<clause>& clause_list);

// funzione ausiliaria di stampa del tableau
void print_tableau(const simplex_data& data, const dictionary& dict);

#endif
