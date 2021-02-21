#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/* A função buildSymtab constrói a tabela de símbolos por passagem de pré-ordem da árvore de sintaxe*/
void buildSymtab(TreeNode *);

/* O procedimento typeCheck executa a verificação de tipo por meio de uma varredura
   da árvore de sintaxe do postorder
 */
void typeCheck(TreeNode *);

#endif