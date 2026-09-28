%code requires { // dichiarazioni c++ -> .hh
	#include <iostream>
	#include <cstdio>
	#include <vector>
	#include <unordered_set>
	#include "parser_header.hh"

  	extern std::FILE * yyin;		// file di input
  	extern int yylineno;			// riga corrente
}

%{	// definizioni per l'implementazione -> .cc
	#include "parser.tab.hh"

	// dichiarazione del lexer
	int yylex(yy::parser::semantic_type* yylval);

	// hashtable per tenere traccia delle variabili già viste
	std::unordered_set<std::string> seen_variables;
%}

// parametri passati dal main al parser
%parse-param { std::vector<clause>& lista_clausole }
%parse-param { std::vector<std::string>& symbol_table }

// IMPOSTAZIONI YACC PER C++

%skeleton "lalr1.cc"
%language "c++"

// uso variant nel parser al posto della union
%define api.value.type variant

// DEFINIZIONE TOKEN E TIPI

%token LTE
%token GTE
%token EQ
%token PLUS
%token MINUS
%token TIMES
%token DIV
%token L_PAR
%token R_PAR

%token <float> 			NUMBER
%token <std::string>	VAR

%type <expr> 		expr
%type <clause> 		clause


/**
	Nota sulle regole di precedenza: è stato necessario inserire
	VAR tra le regole per poter gestire un conflitto con gli altri
	token, in particolare con la moltiplicazione implicita. Avendo
	una grammatica per le espressioni molto flessibile, è difficile
	gestire tutte le precedenze in modo più semplice, questo è stato
	il trade off migliore.
*/

%left     	PLUS MINUS
%left     	TIMES DIV IMPLICIT_MULT VAR
%right		UMINUS

%start 		lines

%%

lines:	line lines
	|
;

line:	clause '\n' {
			lista_clausole.push_back($1);
		}
	|	'\n'
;

// tutti i termini vengono infine spostati a sinistra della espressione
clause:		expr LTE expr		{ $$ = new_clause(sum($1, mult($3, -1.0f)), LTE); }
		|	expr GTE expr		{ $$ = new_clause(sum($1, mult($3, -1.0f)), GTE); }
		|	expr EQ expr		{ $$ = new_clause(sum($1, mult($3, -1.0f)), EQ); }

expr:   expr PLUS expr	{ $$ = sum($1, $3); }
    |   expr MINUS expr	{ $$ = sum($1, mult($3, -1.0f)); }
    |   expr TIMES expr	{
							// controllo semantico per impedire la moltiplicazione tra variabili
            				if (!$1.terms.empty() && !$3.terms.empty()) {
            				    yy::parser::error("Moltiplicazione non lineare!\n");
								YYERROR;
							}
							$$ = ($3.terms.empty()) ? mult($1, $3.constant) : mult($3, $1.constant);
        				}
    |   expr DIV expr	{ 
            				// controllo semantico per impedire la divisione per una variabile
            				if (!$3.terms.empty()) {
            				    yy::parser::error("Divisione per una variabile non permessa!\n");
								YYERROR;
            				}
							if ($3.constant == 0.0f) {
								yy::parser::error("Divisione per zero!");
								YYERROR;
							}
            				$$ = mult($1, 1.0f / $3.constant);
        				}

    |   MINUS expr   	{ $$ = mult($2, -1.0f); } %prec UMINUS	
    |   expr VAR 	 	{
							if (!$1.terms.empty()) {
            				    yy::parser::error("Moltiplicazione non lineare!\n");
            				    YYERROR;
            				}
							
            				expr var_expr = new_variable($2);
            				if(seen_variables.insert($2).second) {
            				    symbol_table.push_back($2);
            				}

            				$$ = mult(var_expr, $1.constant);
						} %prec IMPLICIT_MULT
    
    |   L_PAR expr R_PAR	{ $$ = $2; }
    |   VAR         	{
							$$ = new_variable($1);
							if(seen_variables.insert($1).second)
								symbol_table.push_back($1);
						}
    |   NUMBER      	{ $$ = new_number($1); }
;

%%

void yy::parser::error(const std::string& msg) {
    std::cerr << "Syntax error at line " << yylineno << ": " << msg << std::endl;
	exit(-1);
}
