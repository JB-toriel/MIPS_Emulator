#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(){
  char* rep=NULL;
  char name[1024];
  char comment[1024];
  char command[1024];
  char fname[1024];

while(1){
  //scanf("%s",rep);
  rep = readline( "\nEntrez le repértoire du fichier : " );
        if (rep == NULL || strlen(rep) == 0) {
            if (rep != NULL) free(rep);
            return 1;
        }
	if(!strcmp(rep,"q")){
	    free(rep);
	    return 0;
  	}

while(1){

  FILE* f=NULL;
  strcpy(fname,rep);
  bzero(name, 1024);
  bzero(comment, 1024);
  bzero(command, 1024);

  printf("\nEntrez le nom du fichier : ");
  scanf("%s",name);
  if(!strcmp(name,"q")) break;
  strcat(name,".emu");
  strcat(fname,name);
  printf("\nEntrez le commentaire : ");
  scanf("\n%[^\n]s",comment);
  printf("\nEntrez la commande : ");
  scanf("\n%[^\n]s", command);

  f=fopen(fname, "w+");
  if(f==NULL){
    printf("\nMauvais dossier...\n");
    return 1;
  }

  fprintf(f,"#-------\n#TEST_RETURN_CODE=FAIL\n#TEST_COMMENT=\"%s\"\n#-------\n\nload boucle.o\n%s", comment, command);

  fclose(f);

}
}
}
