#include "globals.h"

char *convertIntChar(int x){


    int lenght = snprintf(NULL, 0, "%d", x);


    char* tmp = (char *)malloc((lenght + 1) * sizeof(char));
    if(tmp == NULL) exit(-1);

    snprintf(tmp, lenght + 1, "%d", x);

    return(tmp);
}

int paramCounter(TreeNode *t){

	int tmp = 0;

	t = t->child[0];

	while(t != NULL){
		tmp++;
		t = t->sibling;
	}

	return tmp;
}