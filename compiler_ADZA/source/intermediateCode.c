#include "intermediateCode.h"
#include "symtab.h"

int cur_lab = 0, cur_reg = 0; 

char opNameList[28][7] = {"ADD", "SUB", "MUL", "DIV", "EQ", "DIF", "LT", "LET", "GT", "GET", "AND", "OR", 
"ASSIGN", "ALLOC", "IMME", "LOAD", "STORE", "ARR", "GOTO", "IFF", "RET", "FUNC",
"END", "PARAM", "CALL", "ARG", "LAB", "HALT"};
    /*
    * LT = Less than
    * LET = Less and Equal than
    * GT = Grather than
    * GET = Greather and equal than
    * IMME = Immidiate 
    * ARR = Array
    * IFF = If false
    * END = End of function
    * LAB = Labbel
    */



void regIncrease(void){
   
    if(cur_reg == 21)
        cur_reg = 0;
    
    else 
        cur_reg++;
}

Address newAddress(){

    Address tmp = (Address)malloc(sizeof(struct address));

    /* verifica alocação */
    if(tmp == NULL) return NULL;

    tmp->contents.var.name = NULL;
    tmp->contents.var.scope = NULL;

    return tmp;
}

Quadruple newQuad(){

    Quadruple tmp = (Quadruple)malloc(sizeof(struct quadruple));

        /* verifica alocação */
    if(tmp == NULL) return NULL;

    return tmp;
}

QuadrupleListElem newQuadListElem(){

    QuadrupleListElem tmp = (QuadrupleListElem)malloc(sizeof(struct QuadListElem));

        /* verifica alocação */
    if(tmp == NULL) return NULL;;

    /* Inicialize sem nenhum próximo-elemento */
    tmp->next = NULL;

    tmp->quadruple = newQuad();

    tmp->quadruple->addr1 = newAddress();
    tmp->quadruple->addr2 = newAddress();
    tmp->quadruple->addr3 = newAddress();

    return tmp;
}

QuadrupleListElem addElemToList(QuadrupleListElem listtop, QuadrupleListElem element){

    QuadrupleListElem aux;

    /* lista vazia*/
    if(listtop == NULL){
        listtop = element;
    }

    /* apenas 1 elemento */
    else if(listtop->next == NULL){ 
        listtop->next = element;
    }

    else{
        aux = listtop->next;

        /* roda ate achar o ultimo elemento */
        while(aux->next != NULL)
            aux = aux->next;

        aux->next = element; 
    }

    return listtop;
}

char *addrPrintGenerator(Address addr){

    /* Seleciona com base nos tipos de endereços */
    switch (addr->kind){

        case String:
            return(addr->contents.var.name); 
        break;

        case IntConst:
            return(convertIntChar(addr->contents.val));
        break;

        case Empty:
            return("-");
        break;

        default:
        break;
    }
}

void printIntermediateCode(QuadrupleListElem list){

   
    char *buffer;
    const int bufferSize = 30;


    char *tmp;

    fprintf(listing, "Gerando código intermediario...\n");


    while(list != NULL){

        /* alocação de Buffer  */
        buffer = (char*)malloc(bufferSize * sizeof(char));
        if(buffer == NULL) exit(-1);

        /* gera "(op, " */
       
        strcat(buffer, "(");

        
        strcat(buffer, opNameList[list->quadruple->op]);
        strcat(buffer, ", ");
      

        /* gera "(op, addr1, " */
        
        tmp = addrPrintGenerator(list->quadruple->addr1);
        strcat(buffer, tmp);
        strcat(buffer, ", ");
        tmp = NULL;
        
    
        /* gera "(op, addr1, addr2, " */
        
        tmp = addrPrintGenerator(list->quadruple->addr2);
        strcat(buffer, tmp);
        strcat(buffer, ", ");
        tmp = NULL;
        

        /* gera "(op, addr1, addr2, addr3)" */
        
        tmp = addrPrintGenerator(list->quadruple->addr3);
        strcat(buffer, tmp);
        strcat(buffer, ")");
        tmp = NULL;
        


        fprintf(intermediateCode, "%s\n", buffer);

      
        free(buffer);
        buffer = NULL;
        
        /* Proximo elemento */
        list = list->next;
    }
}

char *createsRegName(int x){
    
   
    char *buf;
    int buf_len = 4;

    buf = (char*)malloc(buf_len * sizeof(char));
    if(buf == NULL) exit(-1);

    strcat(buf, "$t");

    strcat(buf, convertIntChar(x));

    return(buf);
}

char *createsLabName(int x){
    
    
    char *buf;
    int buf_len = 3;

    buf = (char*)malloc(buf_len * sizeof(char));
    if(buf == NULL) exit(-1);

    strcat(buf, "L");

    strcat(buf, convertIntChar(x));

    return(buf);
}

QuadrupleListElem argGenerator(TreeNode *syntaxTree, QuadrupleListElem list){

    QuadrupleListElem quad;

    while(syntaxTree != NULL){

        /* Chama icGen, mas não vai automaticamente para o irmão, porque queremos gerar o argumento 
            quádruplo toda vez que um novo argumento for encontrado */
        list = icGenArg(syntaxTree, list);
        

        quad = newQuadListElem();
        quad->quadruple->op = ARGc;
        quad->quadruple->addr1->kind = String;
        quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->icTemp);
        quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
        quad->quadruple->addr2->kind = Empty;
        quad->quadruple->addr3->kind = Empty;
        
      
        list = addElemToList(list, quad);

        quad = NULL;
        

        syntaxTree = syntaxTree->sibling;
    }

    return(list);
}

QuadrupleListElem statementGenerator(TreeNode *syntaxTree, QuadrupleListElem list){
    
   
    TreeNode *k1, *k2, *k3;
 
    QuadrupleListElem quad;

    int tmp_iff1, tmp_iff2, tmp_lab1, tmp_lab2;
 
    switch(syntaxTree->kind.stmt){

        case ifK:
           
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];
            k3 = syntaxTree->child[2];         
            
          
            list = icGen(k1, list);
            
            quad = newQuadListElem();
            quad->quadruple->op = IFFc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);;
            quad->quadruple->addr2->kind = String;
           
            quad->quadruple->addr2->contents.var.name = createsLabName(cur_lab);
            
            tmp_lab1 = cur_lab;
            
            cur_lab++;
            quad->quadruple->addr3->kind = Empty;
 
            list = addElemToList(list, quad);

            quad = NULL;

            list = icGen(k2, list);

            
            quad = newQuadListElem();
            quad->quadruple->op = GOTOc;
            quad->quadruple->addr1->kind = String;
           
            quad->quadruple->addr1->contents.var.name = createsLabName(cur_lab);
           
            tmp_lab2 = cur_lab;
            cur_lab++;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

          
            list = addElemToList(list, quad);

            quad = NULL;
            

            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
           
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab1);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            list = addElemToList(list, quad);

            quad = NULL;

            list = icGen(k3, list);

            
            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab2);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            list = addElemToList(list, quad);

            quad = NULL;
            
        break;

        case whileK:
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
       
            quad->quadruple->addr1->contents.var.name = createsLabName(cur_lab);
            tmp_lab1 = cur_lab;
            cur_lab++;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            list = addElemToList(list, quad);

            quad = NULL;

            list = icGen(k1, list);

           
            quad = newQuadListElem();
            quad->quadruple->op = IFFc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            quad->quadruple->addr2->kind = String;
            
            quad->quadruple->addr2->contents.var.name = createsLabName(cur_lab);
            tmp_lab2 = cur_lab;
            cur_lab++;
            quad->quadruple->addr3->kind = Empty;

            list = addElemToList(list, quad);

            quad = NULL;

            list = icGen(k2, list);

         
            quad = newQuadListElem();
            quad->quadruple->op = GOTOc;
            quad->quadruple->addr1->kind = String;
           
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab1);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

        
            list = addElemToList(list, quad);

                    
            quad = NULL;

         
          
            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
       
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab2);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

        
            list = addElemToList(list, quad);

                
            quad = NULL;
        break;

        case assignK:
            
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

           
            list = icGen(k1, list);

            list = icGen(k2, list);


            quad = newQuadListElem();
            quad->quadruple->op = ASSIGNc;
            quad->quadruple->addr1->kind = String;
          
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);
            quad->quadruple->addr3->kind = Empty;

          
            list = addElemToList(list, quad);

            
            quad = NULL;
           
     
            
            if(k1->kind.exp == vectK){
               
                
                quad = newQuadListElem();
                quad->quadruple->op = STOREc;
                quad->quadruple->addr1->kind = String;
                quad->quadruple->addr1->contents.var.name = syntaxTree->child[0]->attr.name;
                quad->quadruple->addr2->kind = String;
                quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                quad->quadruple->addr3->kind = String;
                quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[0]->idxReg);

              
                list = addElemToList(list, quad);

                quad = NULL;
               
            }

            else{
               
                quad = newQuadListElem();
                quad->quadruple->op = STOREc;
                quad->quadruple->addr1->kind = String;
                quad->quadruple->addr1->contents.var.name = syntaxTree->child[0]->attr.name;
                quad->quadruple->addr2->kind = String;
                quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                quad->quadruple->addr3->kind = Empty;

                
                list = addElemToList(list, quad);

              
                quad = NULL;
                
            }
        break;

        case varK:
            
            quad = newQuadListElem();
            quad->quadruple->op = ALLOCc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr2->kind = IntConst;
            quad->quadruple->addr2->contents.val = syntaxTree->attr.len;
            quad->quadruple->addr3->kind = String;
            quad->quadruple->addr3->contents.var.name = quad->quadruple->addr3->contents.var.scope =  syntaxTree->attr.scope;
            
           
            list = addElemToList(list, quad);

            
            quad = NULL;
           
        break;

        case funcK:

            
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

            quad = newQuadListElem();
            quad->quadruple->op = FUNCc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = IntConst;
            quad->quadruple->addr3->contents.val = st_lookup_paramQt(syntaxTree->attr.name, syntaxTree->attr.scope);

            list = addElemToList(list, quad);

            
            quad = NULL;
            
            list = icGen(k1, list);
            list = icGen(k2, list);

            quad = newQuadListElem();
            quad->quadruple->op = ENDc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;
            
            
            list = addElemToList(list, quad);

            
            quad = NULL;
           
        break;

        case callK:
            
            k1 = syntaxTree->child[0];


            list = argGenerator(k1, list);

           
            quad = newQuadListElem();
            quad->quadruple->op = CALLc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name =  syntaxTree->attr.name;
            quad->quadruple->addr2->contents.var.scope = syntaxTree->attr.scope;
            quad->quadruple->addr3->kind = IntConst;
            

            if(strcmp(syntaxTree->attr.name, "input") == 0 || strcmp(syntaxTree->attr.name, "output") == 0)
                quad->quadruple->addr3->contents.val = paramCounter(syntaxTree);
            else 
                quad->quadruple->addr3->contents.val = st_lookup_paramQt(syntaxTree->attr.name, syntaxTree->attr.scope);
            
            list = addElemToList(list, quad);

            quad = NULL;
        break;

        case returnK:
            k1 = syntaxTree->child[0];

            list = icGen(k1, list);

            quad = newQuadListElem();
            quad->quadruple->op = RETc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;
            
            list = addElemToList(list, quad);
            
            quad = NULL;
        break;

        case paramK:
          

            quad = newQuadListElem();
            quad->quadruple->op = PARAMc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = String;
            quad->quadruple->addr3->contents.var.name = syntaxTree->attr.scope;
            
          
            list = addElemToList(list, quad);

            quad = NULL;
        break;

        default:
        break;
    }
}

QuadrupleListElem expressionGenerator(TreeNode *syntaxTree, QuadrupleListElem list){

    
    TreeNode *k1, *k2;
    QuadrupleListElem quad;

    /* Seleciona o tipo de declaração a que o nó corresponde */
    switch(syntaxTree->kind.exp){
    
        case opK:
          
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

            list = icGen(k1, list);
            list = icGen(k2, list);

            /* Seleciona pela operação */
            switch(syntaxTree->attr.op){
                
                case SOM:
                    
                    quad = newQuadListElem();
                    quad->quadruple->op = ADDc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();                   
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                   
                    list =addElemToList(list, quad);
                    
                    quad = NULL;
                    
                break;

                case SUB:
                    
                    quad = newQuadListElem();
                    quad->quadruple->op = SUBc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();                   
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    list =addElemToList(list, quad);

   
                    quad = NULL;
                break;

                case MUL:
                
                    quad = newQuadListElem();
                    quad->quadruple->op = MULc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();                
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    list =addElemToList(list, quad);
                    
                    quad = NULL;
                break;

                case DIV:
                    
                    quad = newQuadListElem();
                    quad->quadruple->op = DIVc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);
                    list =addElemToList(list, quad);
                    
                    quad = NULL;
                   
                break;

                case IGUALIGUAL:
                    
                    quad = newQuadListElem();
                    quad->quadruple->op = EQc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);
                    list =addElemToList(list, quad); 
                    quad = NULL;
              
                break;

                case DIFERENTE:
                   
                    quad = newQuadListElem();
                    quad->quadruple->op = DIFc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();                    
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                   
                    list =addElemToList(list, quad);

                   
                    
                    quad = NULL;
                    
                break;

                case MENOR:
                   
                    quad = newQuadListElem();
                    quad->quadruple->op = LTc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    list =addElemToList(list, quad);
                    
                    quad = NULL;
                break;

                case MENORIGUAL:
                    
                    quad = newQuadListElem();
                    quad->quadruple->op = LETc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    list =addElemToList(list, quad);

                    
                    quad = NULL;
                    
                break;

                case MAIOR:
                    
                    quad = newQuadListElem();
                    quad->quadruple->op = GTc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);


                    list =addElemToList(list, quad);

                    
                    quad = NULL;
                break;

                case MAIORIGUAL:
                   
                    quad = newQuadListElem();
                    quad->quadruple->op = GETc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    list =addElemToList(list, quad);

                    
                    quad = NULL;
                break;
            }
        break;

        case idK:
          
            quad = newQuadListElem();
            quad->quadruple->op = LOADc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr3->kind = Empty;

           
            list =addElemToList(list, quad);

            
            
            quad = NULL;
        break;

        case constK:
         
            quad = newQuadListElem();
            quad->quadruple->op = IMMEc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = IntConst;
            quad->quadruple->addr2->contents.val = syntaxTree->attr.val;
            quad->quadruple->addr3->kind = Empty;

        
            list =addElemToList(list, quad);

      
            
            quad = NULL;

        break;

        case vectK:
         
            k1 = syntaxTree->child[0];

            list = icGen(k1, list);

            quad = newQuadListElem();
            quad->quadruple->op = ARRc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr3->kind = String;
            quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            syntaxTree->idxReg = syntaxTree->child[0]->icTemp;
            list =addElemToList(list, quad);

            quad = NULL;

        break;

        default:
        break;
    }
}

QuadrupleListElem icGen(TreeNode *syntaxTree, QuadrupleListElem list){

    if(syntaxTree != NULL){

        /* seleciona no baseado no tipo */
        switch (syntaxTree->nodekind){

            case statementK:
                list = statementGenerator(syntaxTree, list);
            break;
 
            case expressionK:
                
                if(syntaxTree->kind.exp == typeK)
                    list = icGen(syntaxTree->child[0], list);
                else 
                    list = expressionGenerator(syntaxTree, list);
            break;
        
            default:
            break;
        }

        list = icGen(syntaxTree->sibling, list);
    }

    return(list);
}

QuadrupleListElem icGenArg(TreeNode *syntaxTree, QuadrupleListElem list){

    if(syntaxTree != NULL){

          /* seleciona no baseado no tipo */
        switch (syntaxTree->nodekind){

            case statementK:
                list = statementGenerator(syntaxTree, list);
            break;
 
            case expressionK:
                list = expressionGenerator(syntaxTree, list);
            break;
        
            default:
            break;
        };
    }

    return(list);
}

QuadrupleListElem intermediateCodeGenerator(TreeNode *syntaxTree){

    QuadrupleListElem list = NULL, quad;

    /* Fazer as devidas inicializações da geração do código */

    list = icGen(syntaxTree, list);
    quad = newQuadListElem();
    quad->quadruple->op = HALTc;
    quad->quadruple->addr1->kind = Empty;
    quad->quadruple->addr2->kind = Empty;
    quad->quadruple->addr3->kind = Empty;
    list = addElemToList(list, quad);


    quad = NULL;
  

    return(list);
}
