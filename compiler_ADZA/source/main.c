#include "globals.h"
#include "util.h"
#include "parse.h"
#include "scan.h"
#include "symtab.h"
#include "analyze.h"
#include "intermediateCode.h"

/*define NO_PARSE como TRUE para obter um compilador somente para scanner*/
#define NO_PARSE FALSE
/* defina NO_ANALYZE como TRUE para obter um compilador apenas de analisador */
#define NO_ANALYZE FALSE

/* alocar variáveis ​​globais */
int  lineno = 0;
FILE *source;
FILE *listing;
FILE *tokenList;
FILE *symTab;
FILE *synTree;
FILE *intermediateCode;


int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;



int main( int argc, char * argv[] ){ 

    TreeNode * syntaxTree;
    QuadrupleListElem interCodeList;

    char pgm[120]; 
    
    if (argc != 2){ 
        fprintf(stderr,"Arquivo: %s <filename>\n",argv[0]);
        exit(1);
    }
    
    strcpy(pgm,argv[1]) ;
    
    source = fopen(pgm,"r");
    
    if (source==NULL){ 
        fprintf(stderr,"Não foi possível encontrar o arquivo %s\n",pgm);
        exit(1);
    }
 
    listing = stdout; 

    tokenList = fopen("tokenList.txt","w");
    symTab = fopen("symbolTable.txt","w");
    synTree = fopen("syntaxTree.txt","w"); 
    intermediateCode = fopen("intermediateCode.txt","w");

    fprintf(listing, "\n\n\n\n\n----------\n");
    
    if(NO_PARSE){
        while (getToken()!=ENDFILE);
    }
    else{
        

    fprintf(listing, "----------\n");

	if(!Error){
		syntaxTree = parse();

        	if (TraceParse && !Error) {
            		fprintf(listing,"Gerando arvore sintatica...\n");
            		printTree(syntaxTree);
        	}
    	
        if(!NO_ANALYZE){

            if (! Error){ 
                if (TraceAnalyze){
                    fprintf(listing,"Gerando tabela de simbolos...\n");
                    buildSymtab(syntaxTree);
                }

                if (TraceAnalyze){ 
                    fprintf(listing,"Verificando tipos...\n");
                    typeCheck(syntaxTree);
                }

               
                }
            }
        }
    }

    
    interCodeList = intermediateCodeGenerator(syntaxTree);
    printIntermediateCode(interCodeList);
   

    fprintf(listing, "----------\n");

    if(!Error){
        fprintf(listing,"\nSucesso\n");
    }
    
    fclose(source);
    fclose(tokenList);
    fclose(symTab);
    fclose(synTree);
    fclose(intermediateCode);
   
    return 0;

}