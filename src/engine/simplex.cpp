#include <vector>
#include <algorithm>
#include "engine_header.hh"
#include "parser_header.hh"
#include "simplex_header.hh"
#include <cmath>

bool simplex(simplex_data& data) {

    int pivot_var;
    int suitable_var;
    int pivot_count = 0;

    while (true) {
        pivot_var = bland_rule(data.vars);

        // se non c'è nessuna violazione è SAT
        if (pivot_var == -1) {
            std::cout << "pivot_count: " << pivot_count << std::endl;
            return true;
        }

        suitable_var = select_entering_column(data, pivot_var);

        // se non c'è nessuna suitable var è UNSAT
        if (suitable_var == -1) {
            std::cout << "pivot_count: " << pivot_count << std::endl;
            return false;
        }

        update_values(data, pivot_var, suitable_var);

        pivot(data, pivot_var, suitable_var);

        pivot_count ++;
        if(pivot_count % 100 == 0) {
            std::cout << "\n\r" << pivot_count << " pivot" << std::flush;
        }

    }
}

// ricerca dell'elemento nel vettore
int get_index (const std::vector<int>& vec, int x) {
    int size = vec.size();
    
    for (int i = 0; i < size; i ++) {
        if (vec[i] == x) return i;
    }
    
    return -1;
}

int bland_rule(const std::vector<variable>& vars) {
    
    for(size_t i = 0; i < vars.size(); i ++)
        if ((vars[i].type == B) && (vars[i].value < vars[i].lower || vars[i].value > vars[i].upper))
            return i;

    return -1;
}

int select_entering_column(const simplex_data& data, int xi) {
    
    int vars_size = data.vars.size();
    int leaving_row = get_index(data.basic, xi);
    int entering_col = -1;
    
    bool needs_to_increase = data.vars[xi].value < data.vars[xi].lower;
    bool needs_to_decrease = data.vars[xi].value > data.vars[xi].upper;

    bool can_increase;
    bool can_decrease;

    for (int xj = 0; xj < vars_size; xj ++) {
        if (data.vars[xj].type != N) continue;

        entering_col = get_index(data.non_basic, xj);
        
        float coeff = data.tableau[leaving_row][entering_col];
        
        if (coeff == 0) continue;
        
        can_increase = data.vars[xj].value < data.vars[xj].upper;
        can_decrease = data.vars[xj].value > data.vars[xj].lower;

        if  (   (needs_to_increase && ((coeff > 0.0f && can_increase) || (coeff < 0.0f && can_decrease))) ||
                (needs_to_decrease && ((coeff > 0.0f && can_decrease) || (coeff < 0.0f && can_increase)))
            )
            return xj;
    }

    return -1;
}

void pivot(simplex_data& data, int xi, int xj) {

    int rows = data.basic.size();
    int cols = data.non_basic.size();
    int leaving_row = get_index(data.basic, xi);
    int entering_col = get_index(data.non_basic, xj);

    // scambio degli id e dei tipi
    data.basic[leaving_row] = xj;
    data.non_basic[entering_col] = xi;

    data.vars[xi].type = N;
    data.vars[xj].type = B;
    
    float pivot_value = data.tableau[leaving_row][entering_col];

    for (int r = 0; r < rows; r ++) {
        if (r == leaving_row) continue;
        
        float col_factor = data.tableau[r][entering_col];
        if (col_factor != 0.0f) {
            for (int c = 0; c < cols; c ++) {
                if (c == entering_col) continue;
                
                data.tableau[r][c] -= (col_factor * data.tableau[leaving_row][c]) / pivot_value;
            }
        }
    }

    for (int r = 0; r < rows; r++) {
        if (r != leaving_row) {
            data.tableau[r][entering_col] /= pivot_value;
        }
    }

    for (int c = 0; c < cols; c++) {
        if (c != entering_col) {
            data.tableau[leaving_row][c] = -data.tableau[leaving_row][c] / pivot_value;
        }
    }

    data.tableau[leaving_row][entering_col] = 1.0f / pivot_value;
}

void update_values(simplex_data& data, int xi, int xj) {

    int leaving_row = get_index(data.basic, xi);
    int entering_col = get_index(data.non_basic, xj);

    float pivot_coeff = data.tableau[leaving_row][entering_col];

    float delta = ( data.vars[xi].value < data.vars[xi].lower) ?
                    (data.vars[xi].lower - data.vars[xi].value) / pivot_coeff :
                    (data.vars[xi].upper - data.vars[xi].value) / pivot_coeff ;

    if (delta == 0.0f) return;

    data.vars[xj].value += delta;

    for (size_t i = 0; i < data.basic.size(); i ++) {
        int basic_var = data.basic[i];

        data.vars[basic_var].value = 0;
        for (size_t j = 0; j < data.non_basic.size(); j ++) {
            float coeff = data.tableau[i][j];

            data.vars[basic_var].value += coeff * data.vars[data.non_basic[j]].value;
        }
        
    }
}
