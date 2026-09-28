#include <vector>
#include <algorithm>
#include "engine_header.hh"
#include "parser_header.hh"

#ifndef SIMPLEX_HEADER_HH
#define SIMPLEX_HEADER_HH

// funzione principale del simplesso
bool simplex(simplex_data& data);

// ritorna la prima variabile nell'insieme basic che non soddisfa i bound
int bland_rule(const std::vector<variable>& vars);

// ritorna la variabile nell'insieme non basic adatta al pivot
int select_entering_column(const simplex_data& data, int leaving_row);

// esegue il pivot tra due variabili
void pivot(simplex_data& data, int xi, int xj);

// calcola il delta e aggiorna gli assegnamenti delle variabili
void update_values(simplex_data& data, int xi, int xj);

#endif
