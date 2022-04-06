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
Números reais são especificados com . (10.5). O símbolo ; é o finalizador de comando. O símbolo = indica uma atribuição.
O programa é composto por uma lista contendo atribuições e chamadas a print.
Exemplo 1:
    a = (4-6) * 7 ^ 8
    print a
Exemplo 2:
    a = 10
    b = 10,5
    c = a + b
    print c
Exemplo 3:
    print 5*(5+7)
Exemplo 4: (programa inválido)
    5
