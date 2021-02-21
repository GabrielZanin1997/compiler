#ifndef _UTIL_H_
#define _UTIL_H_

/* O procedimento printToken imprime um token
 * e seu lexema ao arquivo de listagem
 */
void printToken(FILE* file, TokenType , const char* );

/* A função newStmtNode cria uma nova instrução
 * nó para construção de árvore de sintaxe
 */
TreeNode * newStmtNode(StmtKind);

/* A função newExpNode cria uma nova expressão
 * nó para construção de árvore de sintaxe
 */
TreeNode * newExpNode(ExpKind);

/* A função copyString aloca e cria um novo
 * cópia de uma string existente
 */
char * copyString( char * );

/* procedimento printTree imprime uma árvore de sintaxe para o
 * arquivo de listagem usando indentação para indicar subárvores
 */
void printTree( TreeNode * );

void aggScope(TreeNode*, char*);
#endif