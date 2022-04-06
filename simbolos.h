#ifndef SIMBH

#define SIMBH 1
#define TAM 1024

#define TAMMAX 40

#define INDEF 0
#define T_INT 1
#define T_REAL 2


typedef struct noLista{
    char st_nome[TAMMAX];
    int st_tam;
    int linha;
    double st_valor;
    int st_tipo;
}noLista;


static noLista **hash_table;

void initHash(); 
unsigned int hash(char *key); 
void insert(char *nome, int len, int tipo, double valor, int linha);
noLista *lookup(char *nome); 
void dumpTabela(FILE *of);

#endif