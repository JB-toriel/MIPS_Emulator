#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Comprendre valgrind...

int cpy(char *t,char* u, char* l[]){
  char n[10];
  char b[10];

  strcpy(n,t);
  strcpy(b,u);

  strcpy(l[0],n);
  strcpy(l[1],b);
  //printf("chaine = %s cpy = %s\n", n, l[0]);

  return 0;

}

int main(){

char* valeurs[2];
valeurs[0]=malloc(sizeof(char*));
valeurs[1]=malloc(sizeof(char*));
char* t="bonjour";
char* u="salut";
cpy(t,u,valeurs);


printf("chaine = %s %s\n", valeurs[0], valeurs[1]);
free(valeurs[0]);
free(valeurs[1]);
return 0;

}
