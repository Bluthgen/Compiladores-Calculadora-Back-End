#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "asa.h"

extern FILE* yyin;
extern int yylex();
extern int yyparse();


struct node* criaRoot(struct node* filho, int linha, int coluna){
    struct node* root= (struct node *) malloc(sizeof(struct node));
    root->tipo= tipo_expressao;
    root->esq= filho;
    root->dir= NULL;
    root->valor= 0;
    root->nome= strdup("root");
    root->linha= linha;
    root->coluna= coluna;
    return root;
}

struct node* criaNoInt(int valor, int linha, int coluna){
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_int;
    novo->valor= (double)valor;
    novo->esq= NULL;
    novo->dir= NULL;
    novo->nome= strdup("int");
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct node* criaNoReal(double valor, int linha, int coluna){
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_real;
    novo->valor= valor;
    novo->esq= NULL;
    novo->dir= NULL;
    novo->nome= strdup("real");
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct node* criaNoOpMat(enum tipoOp tipo, struct node* filhoE, struct node* filhoD, int linha, int coluna){
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_opMat;
    novo->op= tipo;
    novo->esq= filhoE;
    novo->dir= filhoD;
    novo->valor= 0;
    novo->nome= strdup("mat");
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct node* criaNoId(char* nome, int linha, int coluna){
    //printf("ID: %s %s\n", nome, string);
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_id;
    novo->nome= strdup(string);
    novo->esq= NULL;
    novo->dir= NULL;
    novo->valor= 0;
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct node* criaNoNeg(struct node* filho, int linha, int coluna){
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_neg;
    novo->dir= filho;
    novo->esq= NULL;
    novo->valor= 0;
    novo->nome= strdup("negativo");
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct node* criaNoAtrib(struct node* nome, struct node* valor, int linha, int coluna){
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_atrib;
    novo->esq= nome;
    novo->dir= valor;
    novo->valor= 0;
    novo->nome= strdup("atrib");
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct node* criaNoPrint(struct node* expressao, int linha, int coluna){
    //printf("Print!\n");
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_print;
    novo->dir= expressao;
    //printArvore(expressao);
    novo->esq= NULL;
    novo->valor= 0;
    novo->nome= strdup("print");
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct node* criaNoLigacao(struct node* expEsq, struct node* expDir, int linha, int coluna){
    struct node* novo= (struct node *) malloc(sizeof(struct node));
    novo->tipo= tipo_ligacao;
    novo->esq= expEsq;
    novo->dir= expDir;
    novo->valor= 0;
    novo->nome= strdup("ligacao");
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

struct tokenOp* criaOp(int tipo, int linha, int coluna){
    struct tokenOp* novo= (struct tokenOp *) malloc(sizeof(struct tokenOp));
    novo->tipo= tipo;
    novo->linha= linha;
    novo->coluna= coluna;
    return novo;
}

void printArvore(struct node* root, int nivel, FILE* fout){
    struct node* atual= root;
    int novoNivel= atual->tipo == tipo_ligacao ? 0 : nivel+1;
    if(atual->tipo == tipo_expressao){
        printArvore(atual->esq, novoNivel, fout);
        return;
    }
    
    
    int i;
    
    for(i= 0; i<nivel; i++){
        fprintf(fout, " ");
    }
    if(atual->tipo == tipo_int){
        fprintf(fout, "(%.0f) [Linha %d, Coluna %d]\n", atual->valor, atual->linha, atual->coluna);
    }else if(atual->tipo == tipo_real){
        fprintf(fout, "(%.14f) [Linha %d, Coluna %d]\n", atual->valor, atual->linha, atual->coluna);
    }else if(atual->tipo == tipo_id){
        fprintf(fout, "(%s)[Linha %d, Coluna %d]\n", atual->nome, atual->linha, atual->coluna);
    }else if (atual->tipo == tipo_opMat){
        switch(atual->op){
            case t_soma: fprintf(fout, "%s\n", "SOMA"); break;
            case t_sub:fprintf(fout, "%s\n", "SUB"); break;
            case t_mult:fprintf(fout, "%s\n", "MULT"); break;
            case t_div:fprintf(fout, "%s\n", "DIV"); break;
            case t_pow:fprintf(fout, "%s\n", "POW"); break;
            case t_atrib: break;
        }
    }else if(atual->tipo != tipo_ligacao){
       fprintf(fout, "\n%s\n", atual->nome);
    }
    if(atual->esq != NULL){
        printArvore(atual->esq, novoNivel, fout);
    }
    if(atual->dir != NULL){
        printArvore(atual->dir, novoNivel, fout);
    }
    
}

int getColuna(struct node* atual){
    if(atual->esq != NULL){
        return getColuna(atual->esq);
    }
    int col= atual->coluna;
    if(atual->tipo == tipo_id){
        return col+strlen(atual->nome);
    }else if (atual->tipo == tipo_int || atual->tipo == tipo_real){
        return col+ floor(log10(fabs(atual->valor))) + 1;
    }
    return -1;
}

void enviaString(char* yytext){
    string= malloc(sizeof(yytext)); 
    strcpy(string, yytext);
}