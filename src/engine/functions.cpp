#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip> // per la stampa
#include "engine_header.hh"
#include "parser_header.hh"

void populate_dictionary(const std::vector<std::string>& vars, dictionary& dict) {
    int id = 0;

    for (const auto& name : vars) {
        dict.id[name] = id ++;
        dict.name.push_back(name);
    }
}

dictionary get_dictionary(std::vector<std::string>& symbol_table, var_ordering rule) {
    dictionary dict;

    switch (rule) {
        case var_ordering::ALPHABETICAL: {
            std::sort(symbol_table.begin(), symbol_table.end());
            
            populate_dictionary(symbol_table, dict);
            
            break;
        }

        case var_ordering::REV_ALPHABETICAL: {
            std::sort(symbol_table.begin(), symbol_table.end(), [](const std::string& a, const std::string& b) {
                return a > b; 
            });
            populate_dictionary(symbol_table, dict);
            
            break;
        }

        case var_ordering::BY_LENGTH: {
            std::sort(symbol_table.begin(), symbol_table.end(), [](const std::string& a, const std::string& b) {
                return a.length() < b.length();
            });
            populate_dictionary(symbol_table, dict);
            
            break;
        }

        // è lo stesso ordine di inserimento nel dizionario
        case var_ordering::CHRONOLOGICAL: {
            int new_id = 0;

            for (const auto& var : symbol_table) {
                dict.id[var] = new_id ++;
                dict.name.push_back(var);
            }

            break;
        }
    }

    return dict;
}

std::vector<variable> get_vars(const dictionary& dict, const std::vector<clause>& clause_list) {

    int n_var = dict.name.size();
    int n_clause = clause_list.size();

    std::vector<variable> vars(n_var + n_clause);

    // var non basic nella prima partizione
    for (int i = 0; i < n_var; i ++) {
        vars[i].type = N;
        vars[i].lower = NEG_INF;
        vars[i].upper = INF;
        vars[i].value = 0.0f;
    }

    // var basic nella seconda partizione
    // si può modificare l'ordinamento iterando il vettore in altri modi
    for (int r = 0; r < n_clause; r ++) {

        int id = n_var + r;
        const clause& c = clause_list[r];

        // il termine noto viene negato perchè si trova a sinistra dell'espressione
        float constant = -(c.L.constant);
        
        vars[id].type = B;
        vars[id].value = 0;

        switch (c.op) {
            case LTE:
                vars[id].lower = NEG_INF;
                vars[id].upper = constant;
                break;
                
            case GTE:
                vars[id].lower = constant;
                vars[id].upper = INF;
                break;
                
            case EQ:
                vars[id].lower = constant;
                vars[id].upper = constant;
                break;
        }
    }

    return vars;
}

std::vector<std::vector<float>> get_tableau(dictionary& dict, std::vector<clause>& clause_list) {

    int rows = clause_list.size();
    int cols = dict.id.size();

    std::vector<std::vector<float>> tableau(rows, std::vector<float>(cols, 0.0f));

    for (int r = 0; r < rows; r ++) {
        const clause& c = clause_list[r];
        
        for (const auto& var : c.L.terms) {
            int col_id = dict.id[var.id];
            tableau[r][col_id] = var.coeff;
        }
    }

    return tableau;
}

void print_tableau(const simplex_data& data, const dictionary& dict) {
    
    if (data.tableau.empty()) {
        std::cout << "Il tableau e' vuoto.\n";
        return;
    }

    int num_rows = data.basic.size();
    int num_cols = data.non_basic.size();
    const int COL_WIDTH = 10;

    std::cout << "\n=== SIMPLEX TABLEAU ===\n\n";

    std::cout << std::setw(COL_WIDTH) << " "; 
    for (int c = 0; c < num_cols; ++c) {
        std::cout << std::setw(COL_WIDTH) << std::right << dict.name[data.non_basic[c]];
    }
    std::cout << "\n";

    std::cout << std::string(COL_WIDTH * (num_cols + 1), '-') << "\n";

    for (int r = 0; r < num_rows; ++r) {
        
        std::cout << std::setw(COL_WIDTH) << std::left << r;        
        for (int c = 0; c < num_cols; ++c) {
            float value = data.tableau[r][c];
            std::cout << std::setw(COL_WIDTH) << std::right << std::fixed << std::setprecision(2) << value;
        }
        std::cout << "\n";
    }
    std::cout << std::string(COL_WIDTH * (num_cols + 1), '-') << "\n\n";
}
