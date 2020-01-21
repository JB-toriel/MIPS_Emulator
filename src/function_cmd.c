#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"

#include "elf/elf.h"
#include "common/function_cmd.h"
#include "common/function.h"
#include "common/is.h"


/**
* commande exit qui ne necessite pas d'analyse syntaxique
* @param inter l'interpreteur qui demande l'analyse
* @return 0 en case de succes, un nombre positif sinon
*/
int exitcmd(interpreteur inter) {
  if(get_next_token(inter)!=NULL){
    WARNING_MSG("to many arguments are given to command %s\n","exitcmd");
    return 1;
  }
   INFO_MSG("Bye bye !");
   return CMD_EXIT_RETURN_VALUE;
}//end exitcmd

int debugcmd(interpreteur inter){
  if(get_next_token(inter)!=NULL){
    WARNING_MSG("to many arguments are given to command %s\n","debugcmd");
    return 1;
  }
  if(inter->mode == SCRIPT) inter->mode = INTERACTIF;
  return 0;
}

int resumecmd(interpreteur inter){
  if(get_next_token(inter)!=NULL){
    WARNING_MSG("to many arguments are given to command %s\n","resumecmd");
    return 1;
  }
  if(inter->mode == INTERACTIF) inter->mode = SCRIPT;
  return 0;
}

int assertcmd(interpreteur inter, emul_t vm) {
    DEBUG_MSG("Chaine : %s", inter->input);

    reg_t* reg = vm->reg;
    seg_t* seg = vm->seg;
    int val = 0;
    char val_byte = 0;
    int adr = 0;
    int return_value=0; //gestion d'erreur

    STATE_ARG state=INIT; //decompte d'arguments
    char* token=NULL;

    token = get_next_token(inter);

    while(token!=NULL && return_value==0 && state != END)
    {
      switch (state) {
      case INIT:
        if (!strcmp(token, "word")){
          DEBUG_MSG("Chaine : %s", "word");
          state=WORD;
          token = get_next_token(inter);
        }
        else if(!strcmp(token, "byte")){
          DEBUG_MSG("Chaine : %s", "byte");
          state=BYTE;
          token = get_next_token(inter);
        }
        else {
          WARNING_MSG("value %s is not a valid argument of command %s\n",token,"assertcmd");
          token = get_next_token(inter);
          if(token!=NULL){
            token=get_next_token(inter);
            if(token!=NULL) state=END;
          }
          return_value=1;
        }
      break;

      case WORD:
        if(is_hexa(token)){
          DEBUG_MSG("Chaine : %s", "hexa num");
          sscanf(token, "%x", &val);
          state=ADR;
          token = get_next_token(inter);
          }
        else if (is_signed_int(token)) {
          DEBUG_MSG("Chaine : %s", "signed value");
          sscanf(token, "%d", &val);
          state=ADR;
          token = get_next_token(inter);
        }
        else {
          WARNING_MSG("value %s is not a valid argument of command %s\n",token,"assertcmd");
          if(get_next_token(inter)!=NULL) state=END;
          return_value=1;
        }
      break;

      case ADR:
        if(get_next_token(inter)!=NULL){
          WARNING_MSG("to many arguments are given to command %s\n","assertcmd");
          return_value=1;
        }
        else if(is_adresse(token)){
          DEBUG_MSG("Chaine : %s", "adress");
          sscanf(token, "%x", &adr);
          if(seg[0].adr==0){
            WARNING_MSG("No file has been loaded...\n");
            return 1;
          }
          else if( adr < seg[0].adr || ( adr > (seg[0].adr + seg[0].size - 4) ) ){
            WARNING_MSG("Address 0x%x doesn't match the memory of segment .text, must be betwin 0x%x and 0x%x.\n", adr, seg[0].adr, (seg[0].adr + seg[0].size - 4));
            return_value = 1;
          }
          else printf("(0x%08x != %08x) == %d\n", adr, val, assert_word(vm, adr, val));
          }
        else if(is_registre(token)){
          DEBUG_MSG("Chaine : %s", "register");
          adr = is_registre(token)-1;
          printf("(reg[%d] != %d) == %d\n", adr, val, assert_reg(reg, adr, val));
          DEBUG_MSG("Chaine : %s", "la cmd est bien execute");
          }
        else {
          WARNING_MSG("value %s is not a valid argument of command %s\n",token,"assertcmd");
          return_value=1;
        }
        state=END;
      break;

      case BYTE:
        if(is_signed_byte(token) || is_hexa_byte(token)){
          DEBUG_MSG("Chaine : %s", "signed value");
          sscanf(token, "%hhx", &val_byte);
          state=VALUE;
          token = get_next_token(inter);
          }
        else {
          WARNING_MSG("value %s is not a valid byte argument of command %s\n", token,"assertcmd");
          if(get_next_token(inter)!=NULL) state=END;
          return_value=1;
        }
      break;

      case VALUE:
        if(get_next_token(inter)!=NULL){
          WARNING_MSG("to many arguments are given to command %s\n","assertcmd");
          return_value=1;
        }
        else if(is_adresse(token)){
          DEBUG_MSG("adress\n");
          sscanf(token, "%x", &adr);
          if(seg[0].adr==0){
            WARNING_MSG("No file has been loaded...\n");
            return 1;
          }
          else if( adr < seg[0].adr || ( adr > (seg[0].adr + seg[0].size - 1) ) ){
            WARNING_MSG("Address 0x%x doesn't match the memory of segment .text, must be betwin 0x%x and 0x%x.\n", adr, seg[0].adr, (seg[0].adr + seg[0].size - 1));
            return_value = 1;
          }
          else printf("(0x%08x != %hhx) == %d\n", adr, val_byte, assert_byte(vm, adr, val_byte));
        }
        else{
          WARNING_MSG("value %s is not a valid byte argument of command %s\n", token,"assertcmd");
          return_value=1;
        }
        state=END;
      break;

      default:
      break;
    }//end switch
  }//end while
  if(state!=END){
    WARNING_MSG("to few arguments are given to command %s\n","assertcmd");
    return_value=1;
  }
  return return_value;
}//end assertcmd

int disasmcmd(interpreteur inter, emul_t vm) {
  DEBUG_MSG("Chaine : %s", inter->input);

  if(vm->seg[0].adr==0){
    WARNING_MSG("No file has been loaded...\n");
    return 1;
  }

  int return_value=0; //gestion d'erreur

  STATE_ARG state=INIT; //decompte d'arguments
  char* token=get_next_token(inter);
  char* valeurs[2];
  valeurs[0]=malloc(sizeof(char*));
  valeurs[1]=malloc(sizeof(char*));

  int vadr=0;
  int vadr_range=0;
  int cpt=0;
  type_t rij = init_rij();

  int disasm=0;

  int nb_disasm=0;

  /* la boucle permet d'executé la commande de manière recurrente*/
  while(token!=NULL && return_value==0 && state!=END) {
    switch (state) {
      case INIT:
        if (is_range(token, inter, valeurs)) {
          //Cherche l'adresse
              sscanf(valeurs[0], "%x", &vadr);
              if(is_hexa(valeurs[1])){
                sscanf(valeurs[1], "%x", &vadr_range);
                vadr_range -= vadr;
              }
              else if (is_int(valeurs[1])) {
                sscanf(valeurs[1], "%d", &vadr_range);
              }
              else {
                WARNING_MSG("value %s is not a valid argument of command %s\n",valeurs[1],"disasmcmd");
                return_value = 1;
              }
              if((vadr_range+1)%4){
                WARNING_MSG("the range %d must be a multiple of 4", vadr_range+1);
                return_value = 1;
              }
              else if((vadr)%4){
                WARNING_MSG("Adresse 0x%x must be a multiple of 4", vadr);
                return_value = 1;
              }
              if( vadr < vm->seg[0].adr || vadr > (vm->seg[0].adr + vm->seg[0].size - 4) ){
                WARNING_MSG("Address 0x%x doesn't match the memory of segment .text, must be betwin 0x%x and 0x%x.\n", vadr, vm->seg[0].adr, vm->seg[0].adr + vm->seg[0].size - 4);
                return_value = 1;
              }
              else{
                if(vadr_range>vm->seg[0].size){
                  WARNING_MSG(".text segment only goes from 0x%x to 0x%x", vm->seg[0].adr, vm->seg[0].adr + vm->seg[0].size);
                  vadr_range=vm->seg[0].size -1;
                }
                cpt = search_adr(&vadr, &vadr_range, vm->seg);

                if(cpt==-1) return_value = 1;

                nb_disasm = (vadr_range+1)/4;

                while(nb_disasm>0){
                  disasm = disasm_word(cpt - vadr_range + 3, &rij, vm->seg);
                  if(disasm==0) return_value = 1;
                  print_disasm(vm, disasm, rij, cpt - vadr_range);
                  cpt+=4;
                  nb_disasm--;
                }
              }
              state=END;
            }//end if
            else{
                WARNING_MSG("value %s is not a valid argument of command %s\n",token,"disasmcmd");
                return_value = 1;
            }
        break;

        default:
        break;
      }//end switch

      return return_value;
  }//end while

  if(state!=END) {
    WARNING_MSG("to few arguments given to command %s\n","disasmcmd");
    return 1;
  }

  free(valeurs[0]);
  free(valeurs[1]);
  return return_value;

}//end disasmcmd

int dispcmd(interpreteur inter, emul_t vm) {

  DEBUG_MSG("Chaine : %s", inter->input);

  int return_value=0; //gestion d'erreur

  STATE_ARG state=INIT; //decompte d'arguments
  char* valeurs[2];
  valeurs[0]=malloc(sizeof(char*));
  valeurs[1]=malloc(sizeof(char*));

  reg_t* reg = vm->reg;

  int adr=0,adr2 = 0,length=0,n_reg=1;
  char* token=get_next_token(inter);

  while((token!=NULL && return_value==0 && state!= END))
    {
    switch(state){
      case INIT:
        if(!strcmp(token, "mem")) { //premier argument
            DEBUG_MSG("Chaine : %s", "mem");
            token = get_next_token(inter);
            state = MEM;
        }
        else if(!strcmp(token, "reg")){
          DEBUG_MSG("Chaine : %s", "reg");
          token = get_next_token(inter);
          state = REG;
        }
        else {
          WARNING_MSG("value %s is not a valid argument of command %s\n",token,"dispcmd");
          return_value=1;
        }
      break;

      case MEM:
        if(!strcmp(token,"map")){
          if(get_next_token(inter)!=NULL){
            WARNING_MSG("to many arguments are given to command %s\n","dispcmd");
            state=END;
            return_value=1;
          }
          else if(vm->seg[0].adr==0){
            WARNING_MSG("No file has been loaded...\n");
            state=END;
            return_value = 1;
          }
          else{
            disp_map(vm);
            state = END;
          }
        }
        else if(is_range(token, inter, valeurs)){
          if(vm->seg[0].adr==0){
            WARNING_MSG("No file has been loaded...\n");
            state=END;
            return_value = 1;
          }
          else{
            do {
              sscanf(valeurs[0], "%x", &adr);
              if (is_hexa(valeurs[1])) {
                sscanf(valeurs[1], "%x", &adr2);
                length = adr2 - adr;
                if (length < 0) {
                  WARNING_MSG("Invalid syntax of command %s\n\t\t\t\t\tFirst adress 0x%08x must be smaller than the second 0x%08x\n",
                  "dispcmd", adr, adr2);
                  return_value=1;
                }
                else{
                  if(disp_mem(adr, length, vm)==1) return_value=1;;
                }
              }
              else if (is_int(valeurs[1])) {
                sscanf(valeurs[1], "%d", &length);
                if(disp_mem(adr, length, vm)==1) return_value=1;;
              }
              else {
                WARNING_MSG("value %s is not a valid argument of command %s\n",valeurs[1],"dispcmd");
                state=END;
                return_value = 1;
              }
              token = get_next_token(inter);
            } while(token != NULL && is_range(token, inter, valeurs));
          }
          state = END;
        }
        else {
          WARNING_MSG("value %s is not a valid argument of command %s\n",token,"dispcmd");
          state=END;
          return_value=1;
        }
      break;

      case REG:
        if(!strcmp(token,"all") && get_next_token(inter)!=NULL){
          WARNING_MSG("to many arguments are given to command %s\n","dispcmd");
          return_value=1;
        }
        else if(!strcmp(token,"all")){
          disp_reg(token, reg);
        }
        else{
          while(token!=NULL){
            if(!is_registre(token)) return_value=1;
            else if(disp_reg(token, reg)!=0){
              return_value=1;
              break;
            }
            if(n_reg%4==0) puts("\n");
            token=get_next_token(inter);
            n_reg++;
          }
        }
        puts("\n");
        state = END;
      break;

      default:
      break;
    }//end switch
  }//end while
    if(state!=END){
      WARNING_MSG("to few arguments are given to command %s\n","dispcmd");
      return_value=1;
    }

    free(valeurs[0]);
    free(valeurs[1]);
    return return_value;
}//end dispcmd

int helpcmd(interpreteur inter){
  printf("...\n");
  return 0;
}//end helpcmd

int loadcmd(interpreteur inter, emul_t vm) {

  DEBUG_MSG("Chaine : %s", inter->input);

  int return_value=0; //gestion d'erreur
  int adr = 0;
  char* filename = NULL;
  filename = malloc(20);

  STATE_ARG state=FILE_NAME; //decompte d'arguments
  char* token=get_next_token(inter);

  if(token==NULL) return_value = 1;

/*  if(vm->seg[0].name!=NULL){
    del_stab(symtab);
    del_mem(vm);
    vm = init_mem(4);
  }*/

  /* la boucle permet d'executé la commande de manière recurrente*/
  while(return_value==0 && state != END) {
    switch(state){
      case FILE_NAME:
        if (token[strlen(token)-1]=='o' && token[strlen(token)-2]=='.'){
          strcpy(filename, token);
          state=ADR;
          token = get_next_token(inter);
        }
        else {
          WARNING_MSG("value %s is not a valid argument of command %s\n",token,"loadcmd");
          return_value=1;
        }
      break;
      case ADR:
        if (token == 0) {
          load(vm, 0x1000, filename);
          state=END;
        }
        else if(get_next_token(inter)!=NULL){
          WARNING_MSG("to many arguments are given to command %s\n","loadcmd");
          state = END;
          return_value=1;
        }
        else if(is_adresse(token)){
          sscanf(token, "%x", &adr);
          if (adr != 0 && filename != NULL) {
            if (adr % 0x1000) {
              WARNING_MSG("Load adress must be multiple of 0x1000");
              return_value = 1;
            }
            load(vm, adr, filename);
          }
          state=END;
        }
        else{
          WARNING_MSG("value %s is not a valid argument of command %s. You must enter an address\n",token,"loadcmd");
          state = END;
          return_value=1;
        }
      break;

      default:
      break;
    }//end switch
  }//end while
  if(state!=END){
    WARNING_MSG("to few arguments are given to command %s\n","loadcmd");
    return_value=1;
  }
  free(filename);
  return return_value;
}//end loadcmd

int runcmd(interpreteur inter, emul_t vm, int* pc){

  if(vm->seg[0].adr==0){
    WARNING_MSG("No file has been loaded...\n");
    return 1;
  }

  int return_value=0;
  STATE_ARG state=INIT; //decompte d'arguments
  char* token=get_next_token(inter);

  type_t rij = init_rij();
  seg_t* seg = vm->seg;
  int end = seg[0].adr + seg[0].size;

  while(return_value==0 && state != END) {

    switch(state){
      case INIT:

        if(token==NULL){
          if(seg[0].adr!=0){
            if(*pc==0 || *pc == end){
              *pc = seg[0].adr;
            }
            run(vm, rij, pc);
            state = END;
          }
        }

        else if(is_adresse(token)){
          if(get_next_token(inter)!=NULL){
            WARNING_MSG("To many arguments are given to command %s\n","runcmd");
            state = END;
            return_value = 1;
            break;
          }
          else{
            sscanf(token, "%x", pc);
            if( *pc < seg[0].adr || ( *pc > (seg[0].adr + seg[0].size - 4) ) ){
              WARNING_MSG("Address 0x%x doesn't match the memory of segment .text, must be betwin 0x%x and 0x%x.\n", *pc, seg[0].adr, (seg[0].adr + seg[0].size - 4));
              state = END;
              return_value = 1;
            }
            else if(*pc%4!=0){
              WARNING_MSG("Programm can only run at multiple of 4 address\n");
              state = END;
              return_value = 1;
            }
            else{
              DEBUG_MSG("pc = 0x%x adress .text = 0x%x", *pc, seg[0].adr);
              state = ADR;
            }
          }
        }
        else {
          WARNING_MSG("value %s is not a valid argument of command %s\n",token,"runcmd");
          state = END;
          return_value = 1;
        }
      break;

      case ADR:
        run(vm, rij, pc);
        state=END;
      break;

      default:
      break;

    }//end switch
  }//end while

  if(state!=END){
    WARNING_MSG("to few arguments are given to command %s\n","runcmd");
    return_value=1;
  }

  return return_value;

}//end runcmd

int setcmd(interpreteur inter, emul_t vm) {
  DEBUG_MSG("Chaine : %s", inter->input);

  int return_value=0; //gestion d'erreur
  reg_t* reg = vm->reg;

  STATE_ARG state=INIT; //decompte d'arguments
  char* token=NULL;
  int flag=0;
  int n_reg=0;
  int n_adr=0;

  token = get_next_token(inter);

  while(token!=NULL && return_value==0 && state != END)
  {
    switch (state) {
    case INIT:
      if (!strcmp(token, "mem")){
        DEBUG_MSG("Chaine : %s", "mem");
        state=MEM;
        token = get_next_token(inter);
      }
      else if(!strcmp(token, "reg")){
        DEBUG_MSG("Chaine : %s", "reg");
        state=REG;
        token = get_next_token(inter);
      }
      else {
        WARNING_MSG("value %s is not a valid argument of command %s\n",token,"setcmd");
        if(get_next_token(inter)!=NULL) state=END;
        return_value=1;
      }
    break;

    case MEM:
      DEBUG_MSG("SET_MEM");
      if(is_adresse(token)){
        sscanf(token, "%x", &n_adr);
        state=TYPE;
        token = get_next_token(inter);
        }
      else {
        WARNING_MSG("value %s is not a valid argument of command %s\n",token,"setcmd");
        if(get_next_token(inter)!=NULL) state=END;
        return_value=1;
      }
    break;

    case REG:
      if(is_registre(token)){
        n_reg = is_registre(token)-1;
        DEBUG_MSG("Chaine : %s", "register");
        state=VALUE;
        token = get_next_token(inter);
        }
      else {
        WARNING_MSG("value %s is not a valid argument of command %s\n",token,"setcmd");
        if(get_next_token(inter)!=NULL) state=END;
        return_value=1;
      }
    break;

    case TYPE:
      if(!strcmp(token, "byte")){
        DEBUG_MSG("Chaine : %s", "byte");
        flag = 1;
        state=VALUE;
        token = get_next_token(inter);
        }
      else if(!strcmp(token, "word")){
        DEBUG_MSG("Chaine : %s", "word");
        flag = 2;
        state=VALUE;
        token = get_next_token(inter);
        }
      else {
        WARNING_MSG("value %s is not a valid argument of command %s\n",token,"setcmd");
        if(get_next_token(inter)!=NULL) state=END;
        return_value=1;
      }
    break;

    case VALUE:
      if(get_next_token(inter)!=NULL){
        WARNING_MSG("to many arguments are given to command %s\n","setcmd");
        return_value=1;
      }
      else if(is_signed_int(token) || is_hexa(token)){
        if (flag==2) {
          set_mem_word(token, n_adr, vm);
        }
        else if(flag == 1){
          set_mem_byte(token, n_adr, vm);
        }
        else {
          set_reg(token, n_reg, reg);
        }
      }
      else{
        WARNING_MSG("value %s is not a valid argument of command %s\n",token,"setcmd");
        return_value=1;
      }
      state=END;
    break;
    default:
    break;
    }//end switch
  }//end while
  if(state!=END){
    WARNING_MSG("to few arguments are given to command %s\n","setcmd");
    return_value=1;
  }
  return return_value;
}//end setcmd

int breakcmd(interpreteur inter, emul_t vm){

    DEBUG_MSG("Chaine : %s", inter->input);

    int return_value=0; //gestion d'erreur

    int adr=0;

    STATE_ARG state=INIT; //decompte d'arguments
    char* token=get_next_token(inter);

    /* la boucle permet d'executé la commande de manière recurrente*/
    while(token!=NULL && return_value==0 && state != END) {
      switch(state){
        case INIT:
          if(!strcmp(token, "list")){
            state=LIST;
          }
          else if(!strcmp(token, "add")){
            token = get_next_token(inter);
            if(token==NULL){
               return_value = 1;
               break;
             }
            else if(!is_adresse(token)){
              WARNING_MSG("Value %s invalid argument of command %s, it must be an address of 32 bits long\n",token, "breakcmd");
              state = END;
              return_value = 1;
            }
            else{
              sscanf(token, "%x", &adr);
              if(vm->seg[0].adr==0){
                WARNING_MSG("No file has been loaded...\n");
                state=END;
                return_value = 1;
                break;
              }
              if(adr%4!=0){
                WARNING_MSG("You can only place break points at addresses multiple of 4\n");
                state = END;
                return_value = 1;
                break;
              }
              else state=ADDB;
            }
          }
          else if(!strcmp(token, "del")) state=DEL;
          else{
            WARNING_MSG("value %s invalid argument of command %s\n",token, "breakcmd");
            if(get_next_token(inter)!=NULL) state=END;
            return_value = 1;
          }
        break;
        case LIST:
          if(get_next_token(inter)!=NULL){
            WARNING_MSG("to many arguments are given to command %s\n","breakcmd");
            state=END;
            return_value=1;
          }
          else{
            print_bp(bpp);
            state=END;
          }
        break;
        case ADDB:
        while(token != NULL){
          if(!is_adresse(token)){
            WARNING_MSG("Value %s invalid argument of command %s, it must be an address of 32 bits long\n",token, "breakcmd");
            return_value = 1;
          }
          else{
            if( adr < vm->seg[0].adr || adr > (vm->seg[0].adr + vm->seg[0].size - 4) ){
              WARNING_MSG("Address 0x%x doesn't match the memory of segment .text, must be betwin 0x%x and 0x%x.\n", adr, vm->seg[0].adr, vm->seg[0].adr + vm->seg[0].size - 4);
              return_value = 1;
            }
            else if(adr%4!=0){
              WARNING_MSG("You can only place break points at addresses multiple of 4\n");
              return_value = 1;
            }
            else{
              bpp = add_bp(bpp, adr);
              if(token!=NULL) sscanf(token, "%x", &adr);
            }
            token=get_next_token(inter);
          }
        }
        state=END;
        break;
        case DEL:
          token = get_next_token(inter);
          if(token==NULL){
            return_value = 1;
          }
          if(vm->seg[0].adr==0){
            WARNING_MSG("No file has been loaded...\n");
            if(return_value!=1) state=END;
            return_value = 1;
          }
          else if(bpp==NULL){
            WARNING_MSG("No break points set\n");
            if(return_value!=1) state=END;
            return_value=1;
          }
          else if(!strcmp(token,"all")){
            if(get_next_token(inter)!=NULL){
              WARNING_MSG("to many arguments are given to command %s\n","breakcmd");
              state=END;
              return_value=1;
              break;
            }
            else{
              bpp = del_bp_all(bpp);
              state = END;
            }
          }
          else{
            while(token != NULL && bpp!=NULL){
              if(is_adresse(token)){
                sscanf(token, "%x", &adr);
                if(del_bp_by_adr(&bpp, adr)!=0) return_value=1;
                token=get_next_token(inter);
              }
              else{
                WARNING_MSG("Value %s invalid argument of command %s, it must be an address of 32 bits long\n",token, "breakcmd");
                break;
              }
            }//end while
            state=END;
          }//end else
        break;

        default:
        break;
      }//end switch
    }//end while

    if(state!=END){
      WARNING_MSG("to few arguments are given to command %s\n","breakcmd");
      return_value=1;
    }

    return return_value;
}//end breakcmd

int stepcmd(interpreteur inter, emul_t vm, int* pc){
  DEBUG_MSG("input '%s'", inter->input);

  if(vm->seg[0].adr==0){
    WARNING_MSG("No file has been loaded...\n");
    return 1;
  }

  char* token=get_next_token(inter);

  type_t rij = init_rij();

  if(token==NULL){
    return step(vm, rij, pc);
  }

  else if(!strcmp(token, "into")) {
    if(get_next_token(inter)!=NULL){
      WARNING_MSG("to many arguments are given to command %s\n","stepcmd");
      return 1;
    }
    return stepinto(vm,  rij, pc);
  }
  else{
    WARNING_MSG("value %s is not a valid argument of command %s\n",token,"stepcmd");
    return 1;
  }
}

int is_range(char* chaine, interpreteur inter,  char* valeurs[]) {

  int offset, offset2;
  if (is_adresse(chaine)){
    char chainep[10];
    strcpy(chainep, chaine);

    chaine = get_next_token(inter);

    if(chaine==NULL) return 0;
    else if(!strcmp(chaine,"+")) {

      chaine = get_next_token(inter);
      if(chaine==NULL) return 0;
      else if(!strcmp(chaine,"0")) {
        strcpy(valeurs[0],chainep);
        strcpy(valeurs[1],chaine);
        return 1;
      }
      else{
      offset = atoi(chaine);
      strcpy(valeurs[0],chainep);
      strcpy(valeurs[1],chaine);
      return offset > 0;
      }
    }
    else if (!strcmp(chaine,":")) {
      chaine = get_next_token(inter);

      if(chaine==NULL) return 0;
      else if(is_adresse(chaine)){
      sscanf(chainep, "%x", &offset);
      sscanf(chaine, "%x", &offset2);
      offset = is_adresse(chaine);
      strcpy(valeurs[0],chainep);
      strcpy(valeurs[1],chaine);
      return offset;
      }
    }
  }
  WARNING_MSG("Invalid syntax, one argument or more isn't a 32 bits adresse\n");

  return 0;
}//end is_range

int char_to_bin(char c[], int len){
  int i= 0;
  int res =0;

  for(i=0;i<len;i++){
    res+= (c[len-i-1]-48)*pow(2,i);
    // printf("res = %d c = %d pow = %d\n",res, c[len-i-1]-48, pow(2,i));
  }
  return res;
}//end char_to_bin
