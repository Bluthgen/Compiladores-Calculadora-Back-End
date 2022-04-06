all: calc

calculadora.tab.c calculadora.tab.h:	calculadora.y
	bison -d calculadora.y --locations

lex.yy.c: calculadora.l calculadora.tab.h
	flex calculadora.l

flex.o: lex.yy.c 
	clang lex.yy.c -c -g -o flex.o 

bison.o: calculadora.tab.c calculadora.tab.h
	clang calculadora.tab.c -c -g -o bison.o

asa.o: asa.c
	clang -Wswitch asa.c -c -g -o asa.o

simbolos.o: simbolos.c
	clang simbolos.c -c -g -o simbolos.o 

executa.o: executa.cc
	clang++ executa.cc -c -g -o executa.o `llvm-config-9 --cxxflags` -fPIC 

calculadora.o: calculadora.c
	clang -Wformat -Wint-conversion calculadora.c -c -g -o calculadora.o

calc: flex.o bison.o executa.o calculadora.o asa.o simbolos.o
	clang++ asa.o simbolos.o bison.o flex.o executa.o calculadora.o -o calc `llvm-config-9 --ldflags --libs` -lfl -lm

clean:
	rm calc calculadora.tab.c lex.yy.c calculadora.tab.h executa.o asa.o simbolos.o flex.o bison.o
