%option noyywrap

%{
#include <math.h>
#include <string.h>
#include "calculadora.tab.h"
#include "asa.h"

//void ret_print(char *token_type);
void atualizaLOC();
int linha= 0;
int coluna= 0;
extern FILE *yyin;
extern FILE *yyout;
extern YYLTYPE yylloc;
%}

branco [ ]
tab [\t]
nline [\n]
digit [0-9]
letra [a-zA-Z]
alfanum [_a-zA-Z0-9]
id {letra}+{alfanum}*
intc "0"|[0-9]{digit}*
realc "0"|{digit}*"."{digit}+

%%

{branco} { atualizaLOC(); };

{tab} { atualizaLOC(); }

{nline} { atualizaLOC(); }

{intc} { 
    yylval.ival= atof(yytext);
    atualizaLOC(); 
    return INTEIRO; 
};
{realc} { 
    yylval.fval= atof(yytext);
    atualizaLOC(); 
    return REAL; 
};


"+" { atualizaLOC(); return SOMA;}
"-" { atualizaLOC(); return SUB;}
"*" { atualizaLOC(); return MULT;}
"/" { atualizaLOC(); return DIV;}
"^" { atualizaLOC(); return POW;}
"(" { atualizaLOC(); return PESQ;}
")" { atualizaLOC(); return PDIR;}
"=" { atualizaLOC(); return ATRIB;}
"print" { atualizaLOC(); return PRINT;}
";" { atualizaLOC(); return NOVA; }


{id} { 
    enviaString(yytext); 
    yylval.sval= string; 
    atualizaLOC(); 
    return ID; 
}

. {
    printf("Erro de token não reconhecido na linha %d e coluna %d\n", yylloc.first_line, yylloc.first_column);
    exit(0);
}
%%
void initLOC(){
    yylloc.first_line= 1;
    yylloc.last_line= 1;
    yylloc.first_column= 1;
    yylloc.last_column= 1;
}

void atualizaLOC(){
    if(yytext[0] != '\n'){
        if(yylloc.last_column == 1){
            yylloc.first_column= yylloc.last_column;
            if(yylloc.first_line != yylloc.last_line)
                yylloc.first_line++;
        }
        if(yytext[0] != '\t'){
            if(yylloc.last_column != 1)
                yylloc.first_column= yylloc.last_column+1;
            yylloc.last_column= yylloc.first_column + strlen(yytext)-1;
        }else{
            if(yylloc.last_column != 1)
                yylloc.first_column= yylloc.last_column+1;
            yylloc.last_column+= 4-(yylloc.first_column%4);
        }
    }else{
        yylloc.last_line++;
        yylloc.first_column= yylloc.last_column= 1;
    }
    //printf("[%s] -> F_Line: %d, L_Line: %d, F_Col: %d, L_Col: %d\n", yytext, yylloc.first_line, yylloc.last_line, yylloc.first_column, yylloc.last_column);
}