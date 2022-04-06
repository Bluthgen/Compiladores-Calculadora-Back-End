#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "asa.h"
#include "simbolos.h"
#include "executa.h"


extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern FILE *yyout;

int erro_sintatico= 0;

int main( int argc, char **argv ){
    int c;
    char* entrada= "", *saida= "", *arvoreFile= "";
    int o= 0;
    FILE* fout= NULL;
    int mostraArvore= 0;
    /*++argv, --argc;
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else{
        printf("Por favor forneça o nome de um arquivo contendo os comandos a serem executados\n");
        exit(0);
    }*/

    while((c= getopt(argc, argv, "a:o:h")) != -1){
        switch(c){
            case 'a': mostraArvore= 1; arvoreFile= optarg; break;
            case 'h': printf("Utilização: './calc [-a <Nome_do_arquivo> -o <Nome_do_arquivo> -h] <Nome_do_arquivo.calc>' \nOpções:\n  -a \t\tMostrar a árvore sintática abstrata gerada a partir da entrada\n  -o\t\tNomear o arquivo executável gerado pela compilação (O nome padrão é o mesmo do arquivo de entrada)\n  -h\t\tMostrar esta informação sobre a utilização do programa\n"); exit(0);
            case 'o': saida= optarg; o= 1; break;
            case '?': if(optopt == 'o') printf("Erro: a opção '-o' requer o nome de um arquivo como argumento\n");
                    else if(optopt == 'a') printf("Erro: a opção '-a' requer o nome de um arquivo como argumento\n");
                    else if (isprint(optopt)){
                        printf("Aqui - %s\n", optopt);
                        strcpy(entrada, optopt);
                    }else
                        printf("Erro: opção desconhecida `\\x%x'\n", optopt);
                    exit(0);
            default: exit(0);
        }   
    }
    //printf("OptInd: %d\n", optind);
    //strcpy(entrada, argv[optind]);
    
    entrada= (char*)malloc(strlen(argv[optind])*sizeof(char));
    strcpy(entrada, argv[optind]);
    //printf("Entrada: %s\n", strtok(entrada, "."));
    if (strlen(entrada) > 5){
        if(strcmp(strrchr(entrada, '\0') - 5, ".calc")){
            printf("Erro: Por favor forneça o nome de um arquivo de entrada com extensão .calc como entrada\n");
            exit(0);
        }
        yyin = fopen(entrada, "r");
        if(yyin == NULL){
            printf("Erro na leitura do arquivo de entrada %s! O arquivo pode exigir permissões especiais para ser aberto, ou pode não existir.\n", entrada);
            exit(0);
        }
    }else{
        printf("Erro: Por favor forneça o nome de um arquivo válido como entrada - %s\n", entrada);
        exit(0);
    }
    
    if(o == 0){
        saida= (char*)malloc(strlen(entrada)-5*sizeof(char));
        strcpy(saida, strtok(entrada, "."));
    }
    struct node* root= raizASA;
    yyparse();
    fclose(yyin);
    if(erro_sintatico)
        exit(0);

    

    if(strcmp("", raizASA->nome)){
        int analise= verificaSemantica(raizASA, saida);
        //dumpTabela(tabela);
        //fclose(tabela);
        if(analise == INDEF){
            printf("O código não pode ser executado devido aos erros\n");
            exit(0);
        }
        if(mostraArvore){
            fout= fopen(arvoreFile, "w");
            if(fout == NULL){
                printf("Erro na criação do arquivo de saída da ávore %s!\n", arvoreFile);
                exit(0);
            }
            printArvore(raizASA, 0, fout);
            fclose(fout);
        }
        //printf("\nAnálise: %d\n", analise);
        
    }
    return 0;
    
}
