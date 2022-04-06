#ifndef ASAH

#define ASAH 1

enum tipoOp{
	t_soma,
	t_sub,
	t_mult,
	t_div,
	t_pow,
	t_atrib
};

enum tipoNo{
	tipo_expressao,
    tipo_int,
    tipo_real,
	tipo_id,
	tipo_opMat,
	tipo_neg,
	tipo_atrib,
    tipo_print,
    tipo_ligacao
};

struct node{
	enum tipoNo tipo;
    enum tipoOp op;
	struct node* dir;
	struct node* esq; 
	double valor;
	int linha;
	int coluna;
	char* nome;
};

struct tokenOp{
	enum tipoOp tipo;
	int linha;
	int coluna;
};

struct tokenOp* criaOp(int tipo, int linha, int coluna);

struct node* criaRoot(struct node* filho, int linha, int coluna);

struct node* criaNoInt(int valor, int linha, int coluna);

struct node* criaNoReal(double valor, int linha, int coluna);

struct node* criaNoOpMat(enum tipoOp tipo, struct node* filhoE, struct node* filhoD, int linha, int coluna);

struct node* criaNoId(char* nome, int linha, int coluna);

struct node* criaNoNeg(struct node* filho, int linha, int coluna);

struct node* criaNoAtrib(struct node* nome, struct node* valor, int linha, int coluna);

struct node* criaNoPrint(struct node* expressao, int linha, int coluna);

struct node* criaNoLigacao(struct node* expEsq, struct node* expDir, int linha, int coluna);

void printArvore(struct node* root, int nivel, FILE* fout);

struct node* raizASA;

int getColuna(struct node* atual);

char* string;

void enviaString(char* yytext);

#endif