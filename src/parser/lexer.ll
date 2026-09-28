%{
    #include <string>
    #include <iostream>
    #include "parser.tab.hh"

    #define YY_DECL int yylex(yy::parser::semantic_type * yylval)

%}

%option noinput
%option nounput
%option yylineno
%option noyywrap

NUMBER 			(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)
VAR             [a-zA-Z][a-zA-Z]*[0-9]*

%%

{NUMBER}   		{
                    yylval->build<float>(std::stof(yytext));
                    return yy::parser::token::NUMBER;
                }

{VAR}           {
                    yylval->build<std::string>(yytext);
                    return yy::parser::token::VAR;
                }

"<="            { return yy::parser::token::LTE; }
">="            { return yy::parser::token::GTE; }
"="             { return yy::parser::token::EQ; }
"=="            { return yy::parser::token::EQ; }

"+"             { return yy::parser::token::PLUS; }
"-"             { return yy::parser::token::MINUS; }
"*"             { return yy::parser::token::TIMES; }
"/"             { return yy::parser::token::DIV; }
"("             { return yy::parser::token::L_PAR; }
")"             { return yy::parser::token::R_PAR; }

[ \t\r]			;

\n     			return '\n';
.      			return yytext[0];

%%
