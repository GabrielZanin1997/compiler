#ifndef _INTERMEDIATECODE_H_
#define _INTERMEDIATECODE_H_

#include "globals.h"
 
/* Todas as definições de estruturas para a geração de código intermediário */


typedef enum{ADDc = 0, SUBc, MULc, DIVc, EQc, DIFc, LTc, LETc, GTc, GETc, ANDc, ORc, 
ASSIGNc, ALLOCc, IMMEc, LOADc, STOREc, ARRc, GOTOc, IFFc, RETc, FUNCc,
ENDc, PARAMc, CALLc, ARGc, LABc, HALTc} OpKind;

/*
    * String = variaveis, arqtemporarios, palavras 
    * IntConst = imediatos
    * Empty = para end vazio
    */
/* Estrutura que define o tipo de cada endereço */
typedef enum{String, IntConst, Empty} AddrKind;
    
/* Estrutura que define e descreve todas as informações relevantes presentes no endereço*/
typedef struct address{
    AddrKind kind;
    union{
        int val;
        struct{
            char *name;
            char *scope;
        } var;
    } contents;
} *Address;

/*Estrutura que define um quádruplo (op, addr, addr, addr) */
typedef struct quadruple{
    OpKind op;
    Address addr1, addr2, addr3;
} *Quadruple; 

/* Estrutura que define uma lista unidirecional de quádruplos */
typedef struct QuadListElem{
    int location; 
    Quadruple quadruple;
    struct QuadListElem *next;
} *QuadrupleListElem;

/* Todas as declarações de função e labbels  para a geração de código intermediário */

/* Função para criar um novo elemento de endereço */
Address newAddress(); 

/* Função para criar um novo quádruplo */
Quadruple newQuad(); 

/* Função para criar um novo  elemento de lista quádrupla*/
QuadrupleListElem newQubashadListElem();

/* Função que adiciona o elemento à lista de elementos quádruplos*/
QuadrupleListElem addElemToList(QuadrupleListElem listtop, QuadrupleListElem element);

/* Função que imprime a lista quádrupla no console e em um arquivo */
void printIntermediateCode(QuadrupleListElem list);

/*Função que gera uma string com base no endereço do quádruplo*/
char *addrPrintGenerator(Address addr);

/* Função principal do gerador de código intermediário*/
QuadrupleListElem intermediateCodeGenerator(TreeNode *syntaxTree);

/*Função auxiliar para geração de código */
QuadrupleListElem icGen(TreeNode *syntaxTree, QuadrupleListElem list);


QuadrupleListElem icGenArg(TreeNode *syntaxTree, QuadrupleListElem list);

/* Gera os argumentos quádruplos*/
QuadrupleListElem argGenerator(TreeNode *synTree, QuadrupleListElem list);

/*Função que avalia um tipo de nó de instrução */
QuadrupleListElem statementGenerator(TreeNode *syntaxTree, QuadrupleListElem list);

/* Função que avalia um tipo de nó de expressão*/
QuadrupleListElem expressionGenerator(TreeNode *syntaxTree, QuadrupleListElem list);

/* Cria uma string para o nome do registro*/
char *createsRegName(int x);

/* Cria uma string para o nome do rótulo */
char *createsLabName(int x);


void regIncrease(void); 

#endif