#ifndef _SCAN_H_
#define _SCAN_H_


/* MAXTOKENLEN é o tamanho máximo de um token */
#define MAXTOKENLEN 40

/* tokenString :array armazena o lexema de cada token */
extern char tokenString[MAXTOKENLEN+1];


/* function getToken retorna o
 * próximo token no arquivo de origem
 */
 
TokenType getToken(void);

#endif