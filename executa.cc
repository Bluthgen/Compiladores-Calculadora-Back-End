#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/TargetRegistry.h"
 #include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include <vector>
#include <string>
#include <system_error>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <memory>

extern "C" {
  #include "asa.h"
  #include "simbolos.h"
  #include "executa.h"
};

using namespace std;

std::map<std::string, llvm::Value*> vars;
llvm::Function *printFunct;
static llvm::LLVMContext context;

llvm::Module *module = new llvm::Module("calculadora", context);

char buff[4096]= "";
int offset= 0;

llvm::Function* createPrintfFunction(){
   std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(context)); //char*

    llvm::FunctionType* printf_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                *module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;

}

llvm::Function* createPrintFunction(llvm::Function* printfFn) {
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(llvm::Type::getInt8PtrTy(context));

    llvm::FunctionType* echo_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context), echo_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("print"),
                *module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(context, "entry", func, 0);
    //context->pushBlock(bblock);

    std::vector<llvm::Value*> args;

    for (auto arg = func->arg_begin(); arg != func->arg_end(); &*arg++) {
        llvm::Value* val = arg;
        args.push_back(val);
    }

    llvm::CallInst *call = llvm::CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    llvm::ReturnInst::Create(context, bblock);
    //context.popBlock();
    return func;
}


llvm::Value *codegenIdent(char *nome, llvm::BasicBlock *BB){
  return new llvm::LoadInst(vars[nome], "", false, BB);
}

llvm::Value *codegenInt(int valor){
  return llvm::ConstantInt::get(llvm::Type::getDoubleTy(context), (double) valor, true);
}

llvm::Value *codegenFP(double valor){
  return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), valor);
}

llvm::Value *codegenAssign(char* nome, llvm::Value* RHS, llvm::BasicBlock* BB){
  return new llvm::StoreInst(RHS, vars[nome], false, BB);
}

llvm::Value *codegenIntVarDecl(char* nome, int valor, llvm::BasicBlock* BB){
  auto alloc= new llvm::AllocaInst(llvm::Type::getDoubleTy(context), 0, nome, BB);
  auto store= new llvm::StoreInst(codegenInt(valor), alloc, false, BB);
  vars[nome]= alloc;
  return store;
}

llvm::Value *codegenFPVarDecl(char* nome, double valor, llvm::BasicBlock *BB){
  auto alloc= new llvm::AllocaInst(llvm::Type::getDoubleTy(context), 0, nome, BB);
  auto store= new llvm::StoreInst(codegenFP(valor), alloc, false, BB);
  vars[nome]= alloc;
  return store;
}


llvm::Value *codegenBinOp(enum tipoOp op, int tipo, double *valNumerico, double valEsq, double valDir, llvm::Value* LHS, llvm::Value* RHS, llvm::BasicBlock *BB) {
  if(op == t_pow){
    auto function= module->getFunction("pow");
    std::vector<llvm::Value*> args;
    args.push_back(LHS);
    args.push_back(RHS);
    auto call= llvm::CallInst::Create(function, makeArrayRef(args), "", BB);
    *valNumerico= pow(valEsq, valDir);
    return call;
  }
  /*if (tipo == 1){
    switch (op) {
    case t_soma:
      return llvm::BinaryOperator::Create(llvm::Instruction::Add , LHS, RHS, "", BB);
    case t_sub:
      return llvm::BinaryOperator::Create(llvm::Instruction::Sub , LHS, RHS, "", BB);
    case t_mult:
      return llvm::BinaryOperator::Create(llvm::Instruction::Mul , LHS, RHS, "", BB);
    case t_div:
      return llvm::BinaryOperator::Create(llvm::Instruction::SDiv , LHS, RHS, "", BB);  
    default:
      printf("Invalid binary operator!\n");
      return LHS;
    }
  }else*/ if (tipo == 2 || tipo == 1){
    switch (op) {
    case t_soma:
      *valNumerico= valEsq + valDir;
      return llvm::BinaryOperator::Create(llvm::Instruction::FAdd , LHS, RHS, "", BB);
    case t_sub:
      *valNumerico= valEsq - valDir;
      return llvm::BinaryOperator::Create(llvm::Instruction::FSub , LHS, RHS, "", BB);
    case t_mult:
      *valNumerico= valEsq * valDir;
      return llvm::BinaryOperator::Create(llvm::Instruction::FMul , LHS, RHS, "", BB);
    case t_div:
      *valNumerico= valEsq/valDir;
      return llvm::BinaryOperator::Create(llvm::Instruction::FDiv , LHS, RHS, "", BB);
    default:
      printf("Invalid binary operator!\n");
      return LHS;
    }
  }
  return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
}

int noPrint= 0;

void arruma(){
  std::ifstream infile("ir1.ll");
  //FILE* outfile= fopen("ir.ll", "a");
  std::ofstream outfile("ir.ll", std::ofstream::app);
  std::string line;
  while(getline(infile, line)){
      //stdout << line << "\n";
      if(line.find("call double @pow") != std::string::npos){
        line+= " #3\n";
      }else {
        std::size_t found= line.find("call i32 (i8*, ...) @print([4 x i8] c\"#I\0A\00\",");
        if(found != std::string::npos){
          line.replace(found, 49, "call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i32 0, i32 0),");
        }else{
          found= line.find("call i32 (i8*, ...) @print([4 x i8] c\"#F\0A\00\",");
          if(found != std::string::npos){
            line.replace(found, 49, "call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.1, i32 0, i32 0),");
          }
        }
        line+= "\n";
      }
      std::size_t temp= line.find("\%", 0);
      while(temp != std::string::npos){
        std::string num;
        int pos, prox= temp+1;
        pos= prox;
        int iOrF= 1;
        while (strncmp(&line[pos], " ", 1)&&strncmp(&line[pos], "\n", 1)&&strncmp(&line[pos], ")", 1)){
          if(!strncmp(&line[pos], ".", 1))
            iOrF= 2;
          if(!isdigit(line[pos]))
            iOrF= 0;
          num+= line[pos];
          pos++;
        }
        if (iOrF == 1){
          int numLido= std::stoi(num);
          numLido++;
          line.replace(prox, pos-(prox), std::to_string(numLido));
        }else if(iOrF == 2){
          double numLido= std::stod(num);
          numLido++;
          line.replace(prox, pos-(prox), std::to_string(numLido));
        }
        temp= line.find("\%", prox);
      }
      temp= line.find("i0", 0);
      while(temp != std::string::npos){
        line.replace(temp, 2, "double ");
        temp= line.find("i0", temp+1);
      }
      //fprintf(outfile, line.c_str());
      outfile << line;
  }
  //fclose(outfile);
  outfile.close();
}

llvm::Value *exploraArvore(struct node* atual, double* valNumerico, int* tipo, llvm::BasicBlock* BB){
    if(atual == NULL){
        *valNumerico= 0;
        *tipo= INDEF;
        return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
    }
    if(atual->tipo == tipo_int){
        *tipo= T_INT;
        *valNumerico= atual->valor;
        return codegenFP(atual->valor);
    }
    if(atual->tipo == tipo_real){
        *tipo= T_REAL;
        *valNumerico= atual->valor;
        return codegenFP(atual->valor);
    }
    if(atual->tipo == tipo_id){
        noLista* entrada= lookup(atual->nome);
        if(entrada == NULL){
            printf("Erro de variável não inicializada: '%s' na linha %d e coluna %d\n", atual->nome, atual->linha, atual->coluna);
            *valNumerico= 0;
            return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
        }
        *tipo= entrada->st_tipo;
        *valNumerico= entrada->st_valor;
        //return entrada->st_valor;
        if(noPrint)
          return *tipo == T_INT ? codegenFP( *valNumerico) : codegenFP(*valNumerico);
        return codegenIdent(atual->nome, BB);
    }
    if(atual->tipo == tipo_expressao){
        llvm::Value *valor= exploraArvore(atual->esq, valNumerico, tipo, BB);
        return valor;
    }
    if(atual->tipo == tipo_atrib){
        noLista* entrada= lookup(atual->esq->nome);
        if(entrada == NULL){
            int tipoDir= 0;
            double valNumDir= 0;
            
            llvm::Value *valorDir= exploraArvore(atual->dir, &valNumDir, &tipoDir, BB);
            insert(atual->esq->nome, strlen(atual->esq->nome), tipoDir == tipo_int ? T_INT : T_REAL, valNumDir, atual->linha);
            *tipo= tipoDir;
            *valNumerico= valNumDir;
            return atual->tipo == T_INT ? codegenIntVarDecl(atual->esq->nome, (int) valNumDir, BB) : codegenFPVarDecl(atual->esq->nome, valNumDir, BB);
        }
        int tipoDir= 0;
        double valNumDir= 0;
        llvm::Value *valorDir= exploraArvore(atual->dir, &valNumDir ,&tipoDir, BB);
        if(entrada->st_tipo != tipoDir){
            printf("Erro de atribuição da variavel %s: %s (%.14f), antes era %s (%.0f) (linha: %d, coluna: %d)\n", entrada->st_nome, tipoDir == T_INT ? "'int'" : (tipoDir == T_REAL ? "'real'" : "'indef'"), valNumDir, entrada->st_tipo == T_INT ? "'int'" : (entrada->st_tipo == T_REAL ? "'real'" : "'indef'"), entrada->st_valor, atual->linha, getColuna(atual->esq));
            *tipo= INDEF;
            *valNumerico= 0;
            return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
        }
        entrada->st_valor= valNumDir;
        entrada->linha= atual->linha;
        *tipo= entrada->st_tipo;
        *valNumerico= entrada->st_valor;
        return codegenAssign(atual->esq->nome, valorDir, BB);
    }
    if(atual->tipo == tipo_print || atual->tipo == tipo_neg){
        int tipoDir= 0;
        double valNumDir= 0;
        if (atual->tipo == tipo_print)
          noPrint= 1;
        llvm::Value *valorDir= exploraArvore(atual->dir, &valNumDir, &tipoDir, BB);
        if(atual->tipo == tipo_print){
            llvm::Value *valorPrint;
            std::vector<llvm::Value*> args;
            llvm::CallInst *call;
            noPrint= 0;
            
            if(tipoDir == T_INT){
                //offset+= sprintf(buff + offset, "%.0f\n", valorDir);
                // *** 
                std::string format= "#I\n";
                args.push_back(llvm::ConstantDataArray::getString(context, format.c_str()));
                
            }else if(tipoDir == T_REAL){
                //offset+= sprintf(buff + offset, "%.14f\n", valorDir);
                // ***
                std::string format= "#F\n";
                args.push_back(llvm::ConstantDataArray::getString(context, format.c_str()));
                
            }
            else{
              *tipo= INDEF;
              *valNumerico= 0;
              return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
            }
            args.push_back(valorDir);
            call= llvm::CallInst::Create(printFunct, makeArrayRef(args), "", BB);
            *tipo= tipoDir;
            *valNumerico= valNumDir;
            return call;
        }else{
            double valNumDir= 0;
            exploraArvore(atual->dir, &valNumDir, tipo, BB);
            double menos= -1*valNumDir;
            *valNumerico= valNumDir;
            return *tipo == T_INT ? codegenFP( menos) : codegenFP(menos);
        }
    }
    if(atual->tipo == tipo_ligacao){
        int tipoEsq=0, tipoDir=0;
        double valEsq= 0, valDir= 0;
        llvm::Value *esquerda= exploraArvore(atual->esq, &valEsq, &tipoEsq, BB), *direita= exploraArvore(atual->dir, &valDir, &tipoDir, BB);
        if(tipoEsq != INDEF && tipoDir != INDEF){
            *tipo= tipoEsq;
            *valNumerico= valEsq;
            return esquerda;
        }
        *valNumerico= 0;
        *tipo= tipoDir;
        return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
    }
    int tipoEsq=0, tipoDir=0;
    double valNumEsq= 0, valNumDir= 0;
    llvm::Value *valorEsq, *valorDir;
    
    valorEsq= exploraArvore(atual->esq, &valNumEsq, &tipoEsq, BB);
    valorDir= exploraArvore(atual->dir, &valNumDir, &tipoDir, BB);
    if(tipoEsq == INDEF || tipoDir == INDEF){
        printf("Erro de operação com valor indefinido (linha %d e coluna %d)\n", atual->linha, getColuna(atual->esq));
        *tipo= INDEF;
        *valNumerico= 0;
        return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
    }
    if(tipoEsq != tipoDir){
        printf("Erro de tipos: '%s' vs '%s' (linha %d e coluna %d)\n", tipoEsq == T_INT ? "int" : "real", tipoDir == T_INT ? "int" : "real", atual->linha, getColuna(atual->esq));
        *tipo= INDEF;
        *valNumerico= 0;
        return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
    }
    
    *tipo= tipoEsq;
    /*if(atual->op == t_pow){
      if(tipoEsq == T_INT){
        valorEsq= codegenFP(valNumEsq);
        valorDir= codegenFP(valNumDir);
        llvm::Value* powCall= codegenBinOp(atual->op, T_REAL, valorEsq, valorDir, BB);

      }
    }*/
    //*valNumerico= valNumDir;
    
    /*switch(atual->op){
        case t_soma: //return valorEsq + valorDir;
        case t_sub: //return valorEsq - valorDir;
        case t_div: if(valNumDir == 0){
                        printf("Erro de divisão por zero (linha %d e coluna %d)\n", atual->linha, getColuna(atual->esq));
                        *tipo= INDEF;
                        return 0;
                    }
                    if(tipoEsq == T_REAL) {
                      //return valorEsq / valorDir;
                    }
                    //return valorEsq/valorDir;
        case t_mult: //return valorEsq * valorDir;
        case t_pow: //return pow(valorEsq, valorDir);
    }*/
    if(atual->op == t_div && valNumDir == 0){
      printf("Erro de divisão por zero (linha %d e coluna %d)\n", atual->linha, getColuna(atual->esq));
      *tipo= INDEF;
      *valNumerico= 0;
      return llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
    }
    return codegenBinOp(atual->op, tipoEsq, valNumerico, valNumEsq, valNumDir, valorEsq, valorDir, BB);
}

int verificaSemantica(struct node* root, char* saida){
    int tipo= 0;
    double valNum= 0;
    //llvm::IRBuilder<> builder(context);
 
    llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function *mainFunc = 
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entrypoint", mainFunc, 0);
    
    /*std::vector<llvm::Type *> printArgs;
    printArgs.push_back(llvm::Type::getInt8Ty(context)->getPointerTo());
    llvm::ArrayRef<llvm::Type*>  argsRefPrint(printArgs);
    llvm::FunctionType *printType = 
        llvm::FunctionType::get(llvm::Type::getInt32Ty(context), argsRefPrint, true);
    auto *printFunc = module->getOrInsertFunction("printf", printType);
    */

    std::vector<llvm::Type *> powArgs;
    powArgs.push_back(llvm::Type::getDoubleTy(context)->getPointerTo());
    powArgs.push_back(llvm::Type::getDoubleTy(context)->getPointerTo());
    llvm::ArrayRef<llvm::Type*>  argsRefPow(powArgs);
    llvm::FunctionType *powType = 
        llvm::FunctionType::get(llvm::Type::getDoubleTy(context), argsRefPow, false);
    llvm::FunctionCallee powFunc = module->getOrInsertFunction("pow", powType);
    printFunct= createPrintFunction(createPrintfFunction());
    //printArvore(root, 0, stdout);
    initHash();
    llvm::Value* valor= exploraArvore(root, &valNum, &tipo, entry);
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(TargetTriple);
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    /*if (!Target) {
        errs() << Error;
        return 1;
    }*/
    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    //auto targetMachine =
    //    Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);
    //module->setDataLayout(targetMachine->createDataLayout());
    //auto Filename = "output.o";
    //std::error_code EC;
    //raw_fd_ostream dest(Filename, EC, llvm::sys::fs::F_None);

    /*if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }*/

    //llvm::PassManager pass;
    //auto FileType = llvm::TargetMachine::CGFT_ObjectFile;

    /*if (targetMachine->addPassesToEmitFile(pass, dest, FileType)) {
        errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }*/

    //pass.run(*module);
    //dest.flush();
    
    FILE* ir= fopen("ir.ll", "w");
    fprintf(ir, "@.str = private unnamed_addr constant [6 x i8] c\"%%.0f\\0A\\00\", align 1\n@.str.1 = private unnamed_addr constant [7 x i8] c\"%%.14f\\0A\\00\", align 1\n");
    fprintf(ir, "define i32 @main() #0 {\n");
    fclose(ir);
    std::error_code ec (errno,std::generic_category());
    if(tipo != INDEF){
        //puts(buff);
        //std::unique_ptr<llvm::Module> modulePtr(module);
        llvm::raw_fd_ostream S(std::string("ir1.ll"), ec, llvm::sys::fs::OpenFlags::OF_Append);
        for (llvm::inst_iterator I = inst_begin(mainFunc), E = inst_end(mainFunc); I != E; ++I)
          S << *I << "\n";
        arruma();
        FILE* ir= fopen("ir.ll", "a");
        fprintf(ir, "  ret i32 0\n}\n");
        fprintf(ir, "declare double @pow(double, double) #1\ndeclare i32 @printf(i8*, ...) #2");
        fclose(ir);
        system("rm ir1.ll");
        std::string comp("clang ir.ll -Woverride-module -o ");
        comp+= saida;
        comp+= " -lm";
        system(comp.c_str());
        system("rm ir.ll");
        //llvm::ExecutionEngine *ee= llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module)).create();
        //std::vector<llvm::GenericValue> noargs;
	      //llvm::GenericValue v = ee->runFunction(mainFunc, noargs);
    }
    return tipo;
}