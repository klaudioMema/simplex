#include "parser_header.hh"
#include <vector>
#include <unordered_map>

struct expr new_number(float value) {
    struct expr expression;

    expression.constant = value;
    
    return expression;
}

struct expr new_variable(std::string id) {
    struct expr expression;

    expression.constant = 0;
    expression.terms.push_back({id, 1});

    return expression;
}

struct expr mult(expr expression, float constant) {
    expression.constant *= constant;

    for (auto& [id, coeff] : expression.terms) {
        coeff *= constant;
    }

    return expression;
}

struct expr sum(const expr& e1, const expr& e2) {
    expr result;

    result.constant = e1.constant + e2.constant;
    
    std::unordered_map<std::string, float> merged;
    
    for (const auto& term : e1.terms) merged[term.id] += term.coeff;
    for (const auto& term : e2.terms) merged[term.id] += term.coeff;
    
    for (const auto& pair : merged) {
        if (pair.second != 0.0f) {
            result.terms.push_back({pair.first, pair.second});
        }
    }

    return result;
}

struct clause new_clause(expr expression_a, enum clause_operator op) {
    struct clause cl;

    cl.L = expression_a;
    cl.op = op;

    return cl;
}
