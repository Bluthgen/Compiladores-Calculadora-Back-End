#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "asa.h"
#include "simbolos.h"
#include "executa.h"

char buff[4096]= "";
int offset= 0;

double exploraArvore(struct node* atual, int* tipo){
    if(atual == NULL){
        return 0;
    }
    if(atual->tipo == tipo_int){
        *tipo= T_INT;
        return atual->valor;
    }
    if(atual->tipo == tipo_real){
        *tipo= T_REAL;
        return atual->valor;
    }
    if(atual->tipo == tipo_id){
        noLista* entrada= lookup(atual->nome);
        if(entrada == NULL){
            printf("Erro de variável não inicializada: '%s' na linha %d e coluna %d\n", atual->nome, atual->linha, atual->coluna);
            return 0;
        }
        *tipo= entrada->st_tipo;
        return entrada->st_valor;
    }
    if(atual->tipo == tipo_expressao){
        double valor= exploraArvore(atual->esq, tipo);
        return valor;
    }
    if(atual->tipo == tipo_atrib){
        noLista* entrada= lookup(atual->esq->nome);
        if(entrada == NULL){
            int tipoDir= 0;
            double valorDir= exploraArvore(atual->dir, &tipoDir);
            insert(atual->esq->nome, strlen(atual->esq->nome), tipoDir == tipo_int ? T_INT : T_REAL, valorDir, atual->linha);
            *tipo= tipoDir;
            return valorDir;
        }
        int tipoDir= 0;
        double valorDir= exploraArvore(atual->dir, &tipoDir);
        if(entrada->st_tipo != tipoDir){
            printf("Erro de atribuição da variavel %s: %s (%.14f), antes era %s (%.0f) (linha: %d, coluna: %d)\n", entrada->st_nome, tipoDir == T_INT ? "'int'" : (tipoDir == T_REAL ? "'real'" : "'indef'"), valorDir, entrada->st_tipo == T_INT ? "'int'" : (entrada->st_tipo == T_REAL ? "'real'" : "'indef'"), entrada->st_valor, atual->linha, getColuna(atual->esq));
            *tipo= INDEF;
            return 0;
        }
        entrada->st_valor= valorDir;
        entrada->linha= atual->linha;
        *tipo= entrada->st_tipo;
        return entrada->st_valor;
    }
    if(atual->tipo == tipo_print || atual->tipo == tipo_neg){
        int tipoDir= 0;
        double valorDir= exploraArvore(atual->dir, &tipoDir);
        if(atual->tipo == tipo_print){
            if(tipoDir == T_INT){
                offset+= sprintf(buff + offset, "%.0f\n", valorDir);
            }else if(tipoDir == T_REAL){
                offset+= sprintf(buff + offset, "%.14f\n", valorDir);
            }
            *tipo= tipoDir;
            return 0;
        }else{
            double menos= -1*exploraArvore(atual->dir, tipo);
            return menos;
        }
    }
    if(atual->tipo == tipo_ligacao){
        int tipoEsq=0, tipoDir=0;
        double esquerda= exploraArvore(atual->esq, &tipoEsq), direita= exploraArvore(atual->dir, &tipoDir);
        if(tipoEsq != INDEF && tipoDir != INDEF){
            *tipo= tipoEsq;
            return 1;
        }
        return 0;
    }
    int tipoEsq=0, tipoDir=0;
    double valorEsq, valorDir;
    valorEsq= exploraArvore(atual->esq, &tipoEsq);
    valorDir= exploraArvore(atual->dir, &tipoDir);
    if(tipoEsq == INDEF || tipoDir == INDEF){
        printf("Erro de operação com valor indefinido (linha %d e coluna %d)\n", atual->linha, getColuna(atual->esq));
        *tipo= INDEF;
        return 0;
    }
    if(tipoEsq != tipoDir){
        printf("Erro de tipos: '%s' vs '%s' (linha %d e coluna %d)\n", tipoEsq == T_INT ? "int" : "real", tipoDir == T_INT ? "int" : "real", atual->linha, getColuna(atual->esq));
        *tipo= INDEF;
        return 0;
    }
    *tipo= tipoEsq;
    switch(atual->op){
        case t_soma: return valorEsq + valorDir;
        case t_sub: return valorEsq - valorDir;
        case t_div: if(valorDir == 0){
                        printf("Erro de divisão por zero (linha %d e coluna %d)\n", atual->linha, getColuna(atual->esq));
                        *tipo= INDEF;
                        return 0;
                    }
                    if(tipoEsq == T_REAL) return valorEsq / valorDir;
                    return valorEsq/valorDir;
        case t_mult: return valorEsq * valorDir;
        case t_pow: return pow(valorEsq, valorDir);
    }
    return 0;
}

int verificaSemantica(struct node* root){
    int tipo= 0;
    initHash();
    double valor= exploraArvore(root, &tipo);
    if(tipo != INDEF){
        puts(buff);
    }
    return tipo;
}