#ifndef _SYMTAB_H_
#define _SYMTAB_H_ 


/* O procedimento st_insert insere números de linha e
 * locais de memória na tabela de símbolos
 * loc = local de memória é inserido apenas o
 * primeira vez, caso contrário, ignorado
 */
void st_insert( char * name, int lineno, int loc, char* scope, char* typeID, char* typeData, int paramQt);

/* A função st_lookup retorna a memória
 * localização de uma variável ou -1 se não for encontrado
 */
int st_lookup (char * name, char* scope, char* typeID);

int st_lookup_memloc (char* name, char* Escopo); 

char* st_lookup_type(char* name, char* scope);

/* Procura o número de parâmetros de uma função, usados ​​em chamadas de função */
int st_lookup_paramQt(char *name, char *scope);

/* O procedimento printSymTab imprime um formatado
 * lista do conteúdo da tabela de símbolos
 * para o arquivo de listagem
 */
void printSymTab(FILE * listing);

#endif