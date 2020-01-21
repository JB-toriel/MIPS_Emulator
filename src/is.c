#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"
#include "common/is.h"

int is_int(char* chaine){
  int i;
    if (2147483647 < atoi(chaine)) {
      WARNING_MSG("int overflow %s\n",chaine);
      return 0;
    }
    for (i = 0; i < strlen(chaine); i++) {
      if (!isdigit(chaine[i])) return 0;
    }
    return 1;

}//end is_int

int is_signed_int(char* chaine){
  if(chaine[0]=='-') {
    if (2147483648 < atoi(chaine+1)) {
      WARNING_MSG("int overflow %s\n",chaine);
      return 0;
    }
    return is_int(chaine+1);
  }
  return is_int(chaine);

}//end is_signed_int

int is_byte(char* chaine){
  int i;
    if (256 < atoi(chaine)) {
      WARNING_MSG("int overflow %s\n",chaine);
      return 0;
    }
    for (i = 0; i < strlen(chaine)-1; i++) {
      if (!isdigit(chaine[i])) return 0;
    }
    return 1;

}//end is_byte

int is_signed_byte(char* chaine){
  if(chaine[0]=='-') {
    if (257 < atoi(chaine+1)) {
      WARNING_MSG("int overflow %s\n",chaine);
      return 0;
    }
    return is_int(chaine+1);
  }
  return is_int(chaine);

}//end is_signed_byte

int is_hexa(char* chaine) {
  int k=2;
  if (chaine!=NULL && strlen(chaine)>2 && chaine[0]=='0' && chaine[1]=='x' && strlen(chaine)<11){
    while (k < strlen(chaine) ){
      if (!isxdigit(chaine[k])) return 0;
      k++;
    }
    return 1;
  }
  //WARNING_MSG("you must enter an hexadecimal number as following : 0x...\n");
  return 0;

}//end is_hexa

int is_hexa_byte(char* chaine) {
  int k=2;
  if (chaine!=NULL && strlen(chaine)>2 && chaine[0]=='0' && chaine[1]=='x' && strlen(chaine)<5){
    while( k < strlen(chaine) ){
      if (!isxdigit(chaine[k])) return 0;
      k++;
    }
    return 1;
  }
  //WARNING_MSG("you must enter an hexadecimal number as following : 0x...\n");
  return 0;

}//end is_hexa_byte

int is_adresse(char* chaine){
  DEBUG_MSG("is_hexa(%s) = %d", chaine, is_hexa(chaine));
  if(is_hexa(chaine)) {
    if(strlen(chaine)-2<=8) return 1;
    else{
      DEBUG_MSG("at:%d len:%d\n", atoi(chaine+2), (int)strlen(chaine));
      WARNING_MSG("careful, an adress can't be larger than 32 bits and under 0x1000\n");
      return 0;
    }
  }
  else return 0;

}//end is_adresse

/*int is_adresse_load(char* chaine){
  int k=3;
  if(strlen(chaine)-4<=8){
    if(chaine[0]=='[' && chaine[1]=='0' && chaine[2]=='x'){
      if(chaine[strlen(chaine)-1]==']'){
        for(k;isxdigit(chaine[k]);k++);
        return (strlen(chaine)==k+1);
      }
      else{
        WARNING_MSG("syntax error : an adress must be an hexadecimal value written as follow '0x...' and framed by brackets '[]'\n");
        return 0;
      }
    }
    else{
      WARNING_MSG("syntax error : an adress must be an hexadecimal value framed by brackets '[]' and written as follow '0x...'\n");
      return 0;
    }
  }
  else if(strlen(chaine)>4){
  WARNING_MSG("careful, an adress can't be larger dans 32 bits\n");
  return 0;
  }
  else {
    WARNING_MSG("syntax error : an adress must be an hexadecimal value framed by brackets '[]' and written as follow '0x...'\n");
    return 0;
  }
}
*/

int is_registre(char* chaine){
  int cpt=0;
  int res=0;
  char* chaine_cpy = chaine;
  int return_value;

  if(!strcmp(chaine,"all")){
    DEBUG_MSG("all\n");
    return_value = NBR_REG+1;
  }
  else if(!strcmp(chaine,"hi")) return_value = 33;

  else if(!strcmp(chaine,"lo")) return_value = 34;

  else if(!strcmp(chaine,"pc")) return_value = 35;

  else{
    if(chaine[0]!='$'){
      WARNING_MSG("a register always starts with a '$'\n");
      return_value = 0;
    }
    else if(strlen(chaine)==1 || (chaine[1]=='0' && (int)strlen(chaine)>2)){
      WARNING_MSG("no register found with the name %s\n",chaine);
      return_value = 0;
    }
    else if(!isdigit(chaine[1])){
      while( cpt<NBR_REG ){
        if(strcoll(chaine,registre[cpt])==0){
           return_value = cpt+1;
           break;
        }
        cpt++;
      }
      if(cpt>=NBR_REG){
        WARNING_MSG("no register found with the name %s\n",chaine);
        return_value = 0;
      }
    }
    else{
      chaine_cpy=strtok(chaine_cpy,"$");
      if(is_int(chaine_cpy) || (chaine_cpy[0]=='0' && (int)strlen(chaine_cpy)==1)){
        sscanf(chaine_cpy,"%d",&res);
        if(res<NBR_REG && res>=0){
          return_value = res+1;
        }
        else{
          WARNING_MSG("ther's only %d registers, please enter a nuber between 0 and %d after $\n", NBR_REG, NBR_REG-1);
          return_value = 0;
        }
      }
      else{
        WARNING_MSG("no register found with the name %s\n",chaine);
        return_value = 0;
      }
    }
  }
  return return_value;
}//end is_registre
