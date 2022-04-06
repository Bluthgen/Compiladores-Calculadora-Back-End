%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asa.h"
extern int linha;
extern int coluna;
extern int erro_sintatico;
extern void initLOC();
void yyerror ();
extern FILE* yyin;
extern int yylex ();

%}

%union {
	int ival;
	double fval;
  	char *sval;
	struct node* no;
}

%token <ival> INTEIRO
%token <fval> REAL
%token <sval> ID
%token NOVA
%right ATRIB
%token PESQ PDIR
%right PRINT
%left SOMA SUB
%left MULT DIV
%left NEGATIVO
%right POW

%type <no> Expressao
%type <no> Numero
%type <no> Input
%type <no> Linha
%type <no> Comando
%type <no> Nome




%start Input
%%

Input:   	Linha	{ initLOC(); int linha= 1; int coluna= 1; struct node* esq= $1; raizASA= criaRoot(esq, linha, coluna); }
;

Linha:	Comando NOVA { $$ = $1; }
		|Comando NOVA Linha { struct node* esq= $1; int linha= yylloc.first_line; int coluna= yylloc.first_column; struct node* dir= $3; $$ = criaNoLigacao(esq, dir, linha, coluna); }		
;

Comando:  	Nome ATRIB Expressao { struct node* esq= $1; int linha= yylloc.first_line; int coluna= yylloc.first_column; struct node* dir= $3; $$ = criaNoAtrib(esq, dir, linha, coluna); }
			|PRINT Expressao { int linha= yylloc.first_line; int coluna= yylloc.first_column; struct node* dir= $2; $$ = criaNoPrint(dir, linha, coluna); }
;

Expressao: Numero { $$ = $1; }
			|Expressao SOMA Expressao  { struct node* esq= $1; int linha= @2.first_line; int coluna= @2.first_column; struct node* dir= $3; $$ = criaNoOpMat(t_soma, esq, dir, linha, coluna); }
			|Expressao SUB Expressao  { struct node* esq= $1; int linha= @2.first_line; int coluna= @2.first_column; struct node* dir= $3; $$ = criaNoOpMat(t_sub, esq, dir, linha, coluna); }
			|Expressao MULT Expressao  { struct node* esq= $1; int linha= @2.first_line; int coluna= @2.first_column; struct node* dir= $3; $$ = criaNoOpMat(t_mult, esq, dir, linha, coluna); }
			|Expressao DIV Expressao  { struct node* esq= $1; int linha= @2.first_line; int coluna= @2.first_column; struct node* dir= $3; $$ = criaNoOpMat(t_div, esq, dir, linha, coluna); }
			|SUB Expressao %prec NEGATIVO  { int linha= yylloc.first_line; int coluna= @2.first_column; struct node* dir= $2; $$ = criaNoNeg(dir, linha, coluna); }
			|Expressao POW Expressao  { struct node* esq= $1; int linha= @2.first_line; int coluna= @2.first_column; struct node* dir= $3; $$ = criaNoOpMat(t_pow, esq, dir, linha, coluna); }
			|PESQ Expressao PDIR  { $$ = $2; }
			|Nome { $$ = $1; }
;

Nome: ID { int linha= yylloc.first_line; int coluna= yylloc.first_column; $$ = criaNoId(string, linha, coluna); }
;

Numero:	INTEIRO { int linha= yylloc.first_line; int coluna= yylloc.first_column; $$ = criaNoInt(yylval.ival, linha, coluna); }
		|REAL { int linha= yylloc.first_line; int coluna= yylloc.first_column; $$ = criaNoReal(yylval.fval, linha, coluna); }
;

%%

void yyerror (){
  fprintf(stderr, "Erro de sintaxe na linha %d e coluna %d\n", yylloc.first_line, yylloc.first_column);
  erro_sintatico= 1;
}
