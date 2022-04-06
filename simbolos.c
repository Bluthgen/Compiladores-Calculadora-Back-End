#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simbolos.h"

void initHash(){
	int i; 
	hash_table = malloc(TAM * sizeof(noLista*));
	for(i = 0; i < TAM; i++) hash_table[i] = NULL;
}

unsigned int hash(char *chave){
	unsigned int hashval = 0;
	for(;*chave!='\0';chave++) hashval += *chave;
	hashval += chave[0] % 11 + (chave[0] << 3) - chave[0];
	return hashval % TAM;
}

void insert(char *nome, int len, int tipo, double valor, int linha){
	unsigned int hashval = hash(nome);
	noLista *l = hash_table[hashval];
	
	
	if (l == NULL){
		l = (noLista*) malloc(sizeof(noLista));
		strncpy(l->st_nome, nome, len);  
		l->st_tipo = tipo;
		l->st_valor= valor;
		l->linha = linha;
		hash_table[hashval] = l; 
	}else{
		l->st_valor= valor;
		l->linha= linha;
	}
}

noLista *lookup(char *nome){
	unsigned int hashval = hash(nome);
	noLista *l = hash_table[hashval];
    return l;
}

void dumpTabela(FILE * of){  
  int i;
  fprintf(of,"------------ ------ --------- -------------\n");
  fprintf(of,"Nome         Tipo   Valor     Linha        \n");
  fprintf(of,"------------ ------ --------- -------------\n");
  for (i=0; i < TAM; ++i){ 
	if (hash_table[i] != NULL){ 
		noLista *l = hash_table[i];
		fprintf(of,"%-12s ",l->st_nome);
		if (l->st_tipo == T_INT){ 
			fprintf(of, "%-7s","int");
			fprintf(of, "%8.0f", l->st_valor);
		}else if (l->st_tipo == T_REAL){ 
			fprintf(of,"%-7s","real");
			fprintf(of, "%8f", l->st_valor);
		}else{ 
			fprintf(of,"%-7s","indef");
		}
		fprintf(of,"%4d \n",l->linha);

    }
  }
}