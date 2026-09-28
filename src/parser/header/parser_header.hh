#include <iostream>
#include <string>
#include <vector>

#ifndef PARSER_HEADER_HH
#define PARSER_HEADER_HH

typedef struct clause clause;
typedef struct expr expr;
typedef struct term term;

enum clause_operator {
    GTE,    // >=
    LTE,    // <=
    EQ      // =
};

// termini del linguaggio composti da una variabile e un coefficiente
struct term {
    std::string id;
    float coeff;
};

// l'espressione è un insieme di termini del linguaggio e un termine noto
struct expr {
    std::vector<term> terms;
    float constant;
};

// la clasuola è composta da una espressione e un operatore
// assumiamo che l'espressione sia già stata valutata a sinistra
struct clause {
    expr L;
    enum clause_operator op;
};

// costruisce una nuova espressione che ha solo un termine noto
struct expr new_number(float value);

// costruisce una nuova variabile con coefficiente pari a 1
struct expr new_variable(std::string id);

// moltiplicazione di una espressione per una costante
struct expr mult(expr expression, float coeff);

// somma tra due espressioni
struct expr sum(const expr& expression_a, const expr& expression_b);

// costruzione di una nuova clausola
struct clause new_clause(expr expression_a, enum clause_operator op);

#endif
