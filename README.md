# Compiladores-Calculadora-Back-End
 Segunda parte da implementação de um compilador para a linguagem de uma calculadora simples
 
 Objetivos:
 
    Passo 1: Implemente o analisador léxico, utilizando a ferramenta flex.
    Passo 2: Implemente o analisador sintático, utilizando a ferramenta bison.
    Passo 3: Implemente o analisador semântico.
    Passo 4: Implemente um gerador de código que transforme o programa de entrada em código LLVM.
    Passo 5: Implemente um compilador que una os passos 1 a 4 e gera código executável.

A linguagem é composta por:

    1. palavras reservadas: print
    2. identificadores: nome de variáveis
    3. números inteiros
    4. números reais
    5. números negativos
    6. símbolos: ( ) + - * / ^ . ; =
    
Números reais são especificados com . (10.5). 

O símbolo ; é o finalizador de comando. O símbolo = indica uma atribuição.

O programa é composto por uma lista contendo atribuições e chamadas a print.

Exemplo 1:

    a = (4-6) * 7 ^ 8
    print a
    
Exemplo 2:

    a = 10
    b = 10,5
    c = a + b
    print c
    
Exemplo 3

    print 5*(5+7)
    
Exemplo 4: (programa inválido)

    5
Exemplo 5 (programa inválido)
    
    7 + 9

** Considerações
1) Sua gramática deve considerar precedência de operadores.

2) Seu frontend deve emitir erros de forma precisa, indicando linha e coluna além da descrição do erro.

3) Seu analisador sintático deve APENAS gerar uma árvore sintática abstrata, nenhum outro tipo de processamento deve ser realizado pelo analisador sintático.
    
4) O analisador semântico deve validar as seguintes regras:

    a) A primeira definição de uma variável determina o seu tipo.

    b) O tipo da variável não muda durante a execução do programa.

    c) As operações (inclusive atribuição) devem ser realizadas em objetos do mesmo tipo.

    d) O tipo do resultado da operação de divisão é o tipo de seus operandos.

5) Você pode unir os passos 3 e 4 em um único código. Os outros passos precisam ser independentes.

6) Utilize getopt para parametrizar seu compilador, da seguinte forma:

    -a <YYY> imprime a árvore sintática abstrata (opcional)

    -o <XXX> nome do arquivo de saída

    -h ajuda

Então uma chamada ao compilador deve ser:

    calc -a teste.tree -o teste teste.calc

7) O arquivo de entrada deve ter extensão .calc.
