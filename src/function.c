#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"

#include "elf/elf.h"
#include "common/function.h"
#include "common/is.h"
#include "common/assfunctions.h"

int assert_byte(emul_t vm, int adr, char val){
  int cpt = 0;
  int n_seg = -1;
  seg_t* seg = vm->seg;


  for(cpt=0;cpt<vm->n_seg;cpt++){
    if(adr>=seg[cpt].adr && adr<(seg[cpt].adr + 4096)) n_seg=cpt;
  }
  if(n_seg==-1){
    WARNING_MSG("invalid adresse,doesn't match the memory : %x\n", adr);
    return 1;
  }
  return (val & 0xFF) != (seg[n_seg].val[adr - seg[n_seg].adr] & 0xFF);//masque sur val car val = ffffffxx
}//end assert_byte

int assert_word(emul_t vm, int adr, int val){
  int cpt = 0;

  for (cpt = 0; cpt < 4; cpt++) {
    if(assert_byte(vm, adr - cpt + 3, val & 0xFF)){
      return 1;
    }
    val = val >> 8;
  }
  return 0;
}//end assert_word

int assert_reg(reg_t* reg, int adr, int val){
  return val != reg[adr].val;
}//end assert_reg

int disp_mem(int adr, int length ,emul_t vm){

  seg_t* seg = vm->seg;
  int nb_seg = vm->n_seg;

  int cpt=0;
  int n_seg=-1;

  for(cpt=0;cpt<nb_seg;cpt++){
    if(adr>=seg[cpt].adr && adr<(seg[cpt].adr + seg[cpt].size - 1)) n_seg=cpt;
  }
  DEBUG_MSG("adresse réelle disp %p cpt = %d n_seg = %d\n", &seg[n_seg], cpt, n_seg);
  if(n_seg==-1){
    WARNING_MSG("invalid adresse,doesn't match the memory : 0x%08x\n", adr);
    return 1;
  }
  if (length > seg[n_seg].size - 1) {
    length = seg[n_seg].size - 1;
  }
  for (cpt = 0; cpt < length + 1; cpt++) {
    if (cpt % 16==0) {
      printf("\n0x%08x : ", adr + cpt);
    }
    printf("%02hhx ", seg[n_seg].val[adr-seg[n_seg].adr + cpt]);
  }
  printf("\n");
  return 0;

}//end disp_mem

int disp_reg(char* token,reg_t* reg){
  int n_reg=is_registre(token)-1;

  if(n_reg==-1) return 1;

  else if(n_reg==NBR_REG){
    for(n_reg=0;n_reg<NBR_REG;n_reg++){
      if((n_reg)%4==0) puts("\n");
      printf("%5s : | %08x\t", reg[n_reg].adr, reg[n_reg].val);
    }
  }
  else{
    printf("%s : | %08x\t", registre[n_reg], reg[n_reg].val);
  }
  return 0;
}//end disp_reg

void disp_map(emul_t vm){
  printf("Virtual memory map\n");
  int i;
  char* right=NULL;
  for (i = 0; i < vm->n_seg; i++) {
    switch( SCN_RIGHTS( vm->seg[i].right ) ) {
    case R__ :
        right="r--";
        break;
    case RW_ :
        right="rw-";
        break;
    case R_X :
        right="r-x";
        break;
    default :
        right="???";
    }
  printf("%-7s \t %s \t Vaddr: 0x%08x   Size: %d bytes\n", vm->seg[i].name, right/*vm->seg[i].right*/, vm->seg[i].adr, vm->seg[i].size);
  }

}//end disp_map

int set_mem_byte(char* token, unsigned int n_adr, emul_t vm){
  int cpt=0;
  seg_t* seg = vm->seg;


  for(cpt=0;cpt<vm->n_seg;cpt++){
    if(n_adr>=seg[cpt].adr && n_adr<(seg[cpt].adr + seg[cpt].size)){
      if(is_hexa(token)){
        sscanf(token, "%hhx", &(seg[cpt].val[n_adr-seg[cpt].adr]));
        return 0;
      }
      else if (is_signed_int(token)){
      sscanf(token, "%hhx", &(seg[cpt].val[n_adr-seg[cpt].adr]));
      //sprintf(&token, "%hhx", seg[cpt].val[n_adr-seg[cpt].adr]);
      return 0;
      }
      else{
        WARNING_MSG("value %s is not a valid argument of command %s\n",token,"setcmd");
        return 1;
      }
    }
    else{
      WARNING_MSG("value %x is not in range of memory or memory not initialized : %s\n",n_adr,"setcmd");
      return 1;
    }
  }//end for
  WARNING_MSG("SHOULD NOT BE HERE %s\n","setcmd");
  return 1;
}//end set_mem_byte

int set_mem_word(char* token, unsigned int n_adr, emul_t vm){
  DEBUG_MSG("entree exec\n");

  seg_t* seg = vm->seg;
  int token_b;
  int token_w;
  int cpt=0;
  int cpt_w=0;

  DEBUG_MSG("set adr = 0x%08x (%d), /4 = %d", n_adr, n_adr, n_adr % 4);

  if (n_adr % 4) {
    WARNING_MSG("the writting adress : %x cannot be divided by 4 %s\n",n_adr,"setcmd");
    return 1;
  }

  DEBUG_MSG("n_seg = %d", vm->n_seg);

  for(cpt=0;cpt<vm->n_seg;cpt++){
    DEBUG_MSG("cpt = %d :   n_adr(0x%08x)   >= 0x%08x ==== %d", cpt, n_adr, seg[cpt].adr, n_adr>=seg[cpt].adr);
    if(n_adr>=seg[cpt].adr && n_adr + 4 < (seg[cpt+1].adr)){
      if(is_signed_int(token)){
        sscanf(token, "%d", &token_w);
        sprintf(token, "%x", token_w);
      }
      else if(is_hexa(token)) {
        sscanf(token, "%x", &token_w);
      }
      else{
        WARNING_MSG("value %s is not a valid argument of command %s\n",token,"setcmd");
        return 1;
      }
      DEBUG_MSG("token_w:%x", token_w);
      for(cpt_w=0;cpt_w<4;cpt_w++){
        token_b = token_w << 8*cpt_w;
        token_b = token_b >> 24;
        seg[cpt].val[n_adr-seg[cpt].adr + cpt_w] =  0xFF & token_b;
        DEBUG_MSG("mem.val : %x\n",seg[cpt].val[n_adr-seg[cpt].adr + cpt_w]);
      }
      return 0;
    }
  }
  WARNING_MSG("value 0x%08x is not in range of memory %s\n",n_adr,"setcmd");
  return 1;
}//end set_mem_word

int set_reg(char* token, int n_reg, reg_t reg[]){
  reg[n_reg].adr = registre[n_reg];
  if (is_hexa(token)) {
    sscanf(token, "%x", &reg[n_reg].val);
  }
  else if (is_signed_int(token)) {
    sscanf(token, "%d", &reg[n_reg].val);
  }
  else{
    WARNING_MSG("value %s is not a valid argument of command %s\n",token,"set_reg");
    return 1;
  }
  DEBUG_MSG("n_reg : %d\n", n_reg);
  DEBUG_MSG("%s : | 0x%x \n", reg[n_reg].adr, reg[n_reg].val);
  return 0;
}//end set_reg

int load(emul_t vm, int adr, char* name){

  char* section_names[NB_SECTIONS]= {TEXT_SECTION_STR,RODATA_SECTION_STR,DATA_SECTION_STR,BSS_SECTION_STR};
  unsigned int segment_permissions[NB_SECTIONS]= {R_X,R__,RW_,RW_};
  unsigned int n_segments;
  int i=0, j=0;
  unsigned int endianness;   //little ou big endian
  unsigned int next_segment_start = adr; // compteur pour designer le début de la prochaine section

  symtab= new_stab(0); // table des symboles
  FILE* pf_elf = NULL;

  if ((pf_elf = fopen(name,"r")) == NULL) {
      ERROR_MSG("cannot open file %s", name);
  }

  if (!assert_elf_file(pf_elf)) ERROR_MSG("file %s is not an ELF file", name);
  elf_get_arch_info(pf_elf, &endianness);
  elf_load_symtab(pf_elf, endianness, &symtab);

  n_segments = get_nsegments(symtab,section_names,NB_SECTIONS);

  // Ne pas oublier d'allouer les differentes sections
  j=0;
  for (i=0; i<NB_SECTIONS; i++) {
      if (is_in_symbols(section_names[i],symtab)) {
          elf_load_section_in_memory(pf_elf, vm, section_names[i], segment_permissions[i], next_segment_start);
          next_segment_start+= ((vm->seg[j].size+0x1000)>>12 )<<12; // on arrondit au 1k suppérieur
          print_segment_raw_content(&(vm->seg[j]));
          j++;
      }
  }

  for (i=0; i<n_segments; i++) {
      reloc_segment(pf_elf, vm->seg[i], vm, symtab, i);
  }

  if ( 1 == attach_scn_to_mem(vm, "[stack]", RW_)) {
      WARNING_MSG( "Unable to create %s section", "[stack]" );
      return 1;
  }

  if ( 1 == fill_mem_scn(vm, "[stack]", 0xfffff000 - 0xff7ff000, 0xff7ff000, NULL )) {
      WARNING_MSG( "Unable to fill in %s segment", "[stack]" );
      return 1;
  }

  printf("\n------ Fichier ELF \"%s\" : sections lues lors du chargement ------\n", name) ;
  print_mem(vm);
  stab32_print(symtab);


  fclose(pf_elf);
  DEBUG_MSG("adresse réelle %p\n", &vm->seg[0]);
  puts("");
  return 0;

}//end load

int disasm_word(int cpt, type_t* rij, seg_t seg[]){

  *rij = init_rij();

  char bin[33];//mots binare à traduire
  char val[255];

  opc_t A[36];//all opcodes
  opc_t R[36];//R-type opcode
  opc_t I[36];//I-type opcode
  opc_t J[36];//J-type opcode


  int i=0;
  int cpt2=3;

  FILE* ft = NULL;
  FILE* fr = NULL;
  FILE* fi = NULL;
  FILE* fj = NULL;

  //enregistre tous les types
  ft = fopen("dico/type.txt", "r");

  if(ft==NULL){
    WARNING_MSG("An error as occured...\n");
    return 0;
  }

  for(i=1;!feof(ft);i++){
      fscanf(ft, "%s\n", val);
      A[i].val = char_to_bin(val,6);
      fscanf(ft, "%s\n", A[i].code);
  }
  fclose(ft);

  while(cpt2>=0){
    bin[cpt2] = seg[0].val[cpt];
    cpt2--;
    cpt--;
  }
  rij->opc = (bin[0]>>2) & 0b00111111;//reccupere l'opcode

  char res_type[255];
  //Cherche quel type c'est
  for(i=1;i<36;i++){
    if(A[i].val==rij->opc) {
      strcpy(res_type,A[i].code);
      break;
    }
  }

  //si R-type
  if(res_type[0]=='R'){
    rij->fnc = bin[3] & 0x3F;
    rij->rs = ((bin[0] & 0x3) << 3) | ((bin[1] >> 5) & 0x7);
    rij->rt = bin[1] & 0b00011111;
    rij->rd = bin[2] >> 3;
    rij->sa = (bin[2] & 0x7) + ((bin[3] >> 6) & 0x3);

      //recherche et reccupere les noms de registres
      for(i=0;i<32;i++){
        if(i==rij->rs) rij->rs_c = registre[i];
        if(i==rij->rt) rij->rt_c = registre[i];
        if(i==rij->rd) rij->rd_c = registre[i];
      }

      fr = fopen("dico/R.txt", "r");

      if(fr==NULL){
        WARNING_MSG("An error as occured...\n");
        return 0;
      }

      //Reccupere les R-type codes
      for(i=1;!feof(fr);i++){
        fscanf(fr, "%s\n", val);
        R[i].val = char_to_bin(val,6);
        fscanf(fr, "%s\n", R[i].code);
        R[i].exec = i;
      }
      fclose(fr);

      //cherche le nom de la fnc
      if(rij->opc==31){
        rij->name_fnc="SEB";
        rij->exec=21;
      }
      else{
        for(i=1;i<36;i++){
          if(R[i].val==rij->fnc) {
            if(bin[0]==0 && bin[1]==0 && bin[2]==0 && bin[3]==0) rij->name_fnc = "NOP";
            else rij->name_fnc = R[i].code;
            rij->exec = R[i].exec;
            break;
          }
        }
        if(rij->name_fnc==NULL){
          WARNING_MSG("No disasm function found for R fnc : 0x%x\n", rij->fnc);
          return 0;
        }
      }
     DEBUG_MSG("DISASM : %s rs = %s, rt = %s, rd = %s, sa = %d\n", rij->name_fnc, rij->rs_c, rij->rt_c, rij->rd_c, rij->sa); //affichage du code assembleur
     return 1;
  }//end if R


  //si I-type
  else if(res_type[0]=='I'){

      //rij->opc = bin[0] >> 2;//reccupere fnc
      rij->rs = ((bin[0] & 0x3) << 3) | ((bin[1] >> 5) & 0x7);
      rij->rt = bin[1] & 0b00011111;
      rij->imd = (bin[2] << 8) | bin[3];

      //recherche et reccupere les noms de registres
      for(i=0;i<32;i++){
        if(i==rij->rs) rij->rs_c = registre[i];
        if(i==rij->rt) rij->rt_c = registre[i];
      }

      fi = fopen("dico/I.txt", "r");

      if(fi==NULL){
        WARNING_MSG("An error as occured...\n");
        return 0;
      }

      //Reccupere les I-type codes
      for(i=1;!feof(fi);i++){
        fscanf(fi, "%s\n", val);
        I[i].val = char_to_bin(val,6);
        fscanf(fi, "%s\n", I[i].code);
        I[i].exec = i;
      }
      fclose(fi);

      //cherche le nom de la fnc
      for(i=1;i<36;i++){
        if(I[i].val==rij->opc) {
          if(I[i].val==1 && rij->rt==0) rij->name_fnc="BLTZ";
          else rij->name_fnc = I[i].code;
          rij->exec = I[i].exec;
          break;
        }
      }

      DEBUG_MSG("DISASM : %s rs = %s, rt = %s, imm = %d\n", rij->name_fnc, rij->rs_c, rij->rt_c, rij->imd); //affichage du code assembleur
      return 2;
  }//end if I-type

  //si J-type
  else if(res_type[0]=='J'){

      rij->opc = bin[0] >> 2;//reccupere opc
      rij->jmp = ((bin[0] & 0x2) << 24) | (bin[1] << 16) | (bin[2] << 8) | bin[3];

      fj = fopen("dico/J.txt", "r");

      if(fj==NULL){
        WARNING_MSG("An error as occured...\n");
        return 0;
      }

      //Reccupere les I-type codes
      for(i=1;!feof(fj);i++){
        fscanf(fj, "%s\n", val);
        J[i].val = char_to_bin(val,6);
        fscanf(fj, "%s\n", J[i].code);
      }
      fclose(fj);

      //cherche le nom de l'opc
      for(i=1;i<36;i++){
        if(J[i].val==rij->opc) {
          rij->name_fnc = J[i].code;
          break;
        }
      }

      DEBUG_MSG("DISASM : %s, offset = %d\n", rij->name_fnc, rij->jmp); //affichage du code assembleur
      return 3;
  }//end if J-type

  else{
    WARNING_MSG("NOOOOOOOOOOOOOOOOOOOOOOOOOOON !!\n");
    return 0;
  }


}//end disasm_word

int print_disasm(emul_t vm, int disasm, type_t rij, int cpt){
  seg_t seg = vm->seg[0];
  reg_t* reg = vm->reg;
  int i;
  switch (disasm) {
    case 1: //R
      if (rij.opc == 31) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %s\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rd_c, rij.rt_c);
      }
      else if (32 <= rij.fnc) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %s, %s\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rd_c, rij.rs_c, rij.rt_c);
      }
      else if (rij.fnc == 24 || rij.fnc == 26) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %s\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rs_c, rij.rt_c);
      }
      else if (rij.fnc == 8 || rij.fnc == 9) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s\t", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rs_c);
        for (i = 0; i<symtab.size;i++) {
          if (reg[rij.rs].val == seg.adr + symtab.sym[i].addr) {
            printf("%s", symtab.sym[i].name);
          }
        }
        printf("\n");
      }
      else if (rij.fnc == 16 || rij.fnc == 18) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rd_c);
      }
      else if (rij.fnc <= 3 && rij.name_fnc[0] != 'N') {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %s, %d\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rd_c, rij.rt_c, rij.sa);
      }
      else if (!(rij.opc | rij.rd | rij.rt | rij.rs | rij.fnc) || rij.fnc == 12) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc);
      }
    break;
    case 2: //I
      if (rij.opc <= 13 && 8 <= rij.opc) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %s, %d\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rt_c, rij.rs_c, rij.imd);
      }
      else if (rij.opc == 4 || rij.opc == 5) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, 0x%x \t", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rt_c, rij.imd << 2);
        for (i = 0; i<symtab.size;i++) {
          if (seg.adr + cpt + 4 + (rij.imd << 2) == seg.adr + symtab.sym[i].addr) {
            printf("%s", symtab.sym[i].name);
          }
        }
        printf("\n");      }
      else if (rij.opc == 15) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %d\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rs_c, rij.imd);
      }
      else if (rij.opc == 1){
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %d \t", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rs_c, rij.imd );
        for (i = 0; i<symtab.size;i++) {
          if (seg.adr + (rij.imd << 2) == seg.adr + symtab.sym[i].addr) {
            printf("%s", symtab.sym[i].name);
          }
        }
        printf("\n");
      }
      else if (rij.opc == 6 || rij.opc == 7) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %d \t", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rt_c, seg.adr + (rij.imd << 2));
        for (i = 0; i<symtab.size;i++) {
          if (seg.adr + (rij.imd << 2) == seg.adr + symtab.sym[i].addr) {
            printf("%s", symtab.sym[i].name);
          }
        }
        printf("\n");
      }
      else if (35 <= rij.opc) {
        printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %s, %d(%s)\n", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, rij.rt_c, rij.imd, rij.rs_c);
      }
    break;
    case 3: //J
      printf("%x :: %02hhx%02hhx%02hhx%02hhx \t %s \t %x\t", seg.adr + cpt, seg.val[cpt], seg.val[cpt+1], seg.val[cpt+2], seg.val[cpt+3], rij.name_fnc, (seg.adr & 0xF000) | (rij.jmp << 2));
      for (i = 0; i<symtab.size;i++) {
        if (((seg.adr & 0xF000) | (rij.jmp << 2)) == seg.adr + symtab.sym[i].addr) {
          printf("%s", symtab.sym[i].name);
        }
      }
      printf("\n");
    break;
    default:
      WARNING_MSG("SHOULD NOT BE HERE -> print_disasm");
    break;
  }
  return 0;
}//end print_disasm

int search_adr(int* adresse, int* range, seg_t seg[]){

  int i=0;
  int cpt=0;

  *adresse += *range;
  for(i=0;i<4096;i++){
    if(*adresse == seg[0].adr + i){
    cpt=i;
    break;
    }
  }
  if(i>4092){
      WARNING_MSG("ERROR !!!! \n");
      return -1;
  }
  return cpt;
}//end search_adr

int stepinto(emul_t vm, type_t rij, int* pc){
  seg_t* seg = vm->seg;
  int end = seg[0].adr + seg[0].size;
  int fnc_type;
  if(*pc == 0) *pc = seg[0].adr;
  else if (seg[0].adr <= *pc && *pc + 4 < end) {
    fnc_type = disasm_word(*pc - seg[0].adr + 3, &rij, seg);
    *pc += 4;
    exec(rij, pc, fnc_type, vm);
    return 0;
  }
  else if(*pc + 4 == end){
    fnc_type = disasm_word(*pc - seg[0].adr + 3, &rij, seg);
    *pc += 4;
    exec(rij, pc, fnc_type, vm);
    printf("END OF PROGRAMM\n");
    return 0;
  }
  else if(*pc == end){
    char* userans = NULL;
    WARNING_MSG("You have reach the end (0x%08x) but still insist on stepping.\n", *pc);
    userans = readline("If you want a reset of PC type \"y\" else type any key: ");
    printf("%s\n", userans);
    if(!strcmp(userans, "y")) *pc = 0;
  }
  else if(*pc < seg[0].adr || *pc > end + 1) {
    WARNING_MSG("Address 0x%x doesn't match the memory of segment .text, must be betwin 0x%x and 0x%x.\n", *pc, seg[0].adr, (seg[0].adr + seg[0].size - 4));
  }
  return 0;
}

int step(emul_t vm, type_t rij, int* pc){
  seg_t seg = vm->seg[0];
  int pcstep = 0;
  pcstep+= *pc;
  int end = seg.adr + seg.size;
  bp_t* bpp_cpy = bpp;
  stepinto(vm, rij, pc);
  int i = 0;
  while (i < symtab.size) {
    if (seg.adr + symtab.sym[i].addr == *pc && symtab.sym[i].addr != 0) {
      while (*pc != pcstep + 4 && *pc < end) {
        if (bpp_cpy != NULL){
          if(*pc == bpp_cpy->adr && pcstep != bpp_cpy->adr) {
            printf("if %x\n", bpp_cpy->adr);
            return 0;
          }
        }
        stepinto(vm, rij, pc);
      }
    }
    i++;
  }
  return 0;
}

int run(emul_t vm, type_t rij, int* pc){
  seg_t seg = vm->seg[0];
  int pcrun = 0;
  pcrun+= *pc;
  int end = seg.adr + seg.size;
  bp_t* bpp_cpy = bpp;
  while (*pc < end) {
  if (bpp_cpy != NULL){
      if(*pc == bpp_cpy->adr && pcrun != bpp_cpy->adr) {
        return 0;
      }
    }
    stepinto(vm, rij, pc);
  }
  return 0;
}//end run

type_t init_rij(void){
  type_t rij;
  rij.name_fnc=NULL;
  rij.rs_c=NULL;
  rij.rt_c=NULL;
  rij.rd_c=NULL;
  rij.sa_c=NULL;
  rij.exec = 0;
  rij.opc=0;
  rij.fnc=0;
  rij.rs=0;
  rij.rt=0;
  rij.rd=0;
  rij.sa=0;
  rij.imd=0;
  rij.jmp=0;

  return rij;
}

bp_t* create_bp(int adr){

  bp_t* bp = malloc(sizeof(*bp));


  if(bp==NULL){
    WARNING_MSG("Unable to initialise break points at adress %d\n", adr);
    return NULL;
  }

  bp->adr = adr;
  bp->flag = 0;
  bp->suiv = NULL;

  return bp;
}//end create_bp

//Ajoute un élement à la fin de la liste de bp
bp_t* add_bp(bp_t* bp, int adr){

    unsigned int bp_nb=1;
    bp_t** bp_p = &bp;

    if(bp==NULL) *bp_p = create_bp(adr);

    else{
      do{
        if((*bp_p)->adr == adr){
          WARNING_MSG("You've already set a break point at address 0x%x, so nothing have been changed\n", adr);
          WARNING_MSG("You can check youre break points if you want, by typing \"break list\": into the MipsShell\n");
          return bp;
        }
        bp_p = &(*bp_p)->suiv;
        bp_nb++;
      }while(*bp_p != NULL);
      *bp_p = create_bp(adr);
    }

    return bp;
}//end bp_t add_bp

bp_t* del_first_bp(bp_t* bp){

  bp_t* bp_p = bp;

  if(bp!=NULL){
    bp_p = bp->suiv;
    free(bp);
  }
  return bp_p;
}//end del_bp_first_bp

int del_bp_by_adr(bp_t** bp, int adr){

  if(bp==NULL){
    WARNING_MSG("No break points set\n");
    return 1;
  }

  bp_t** bp_p = bp;

  while((*bp_p)->adr!=adr){
    bp_p = &(*bp_p)->suiv;
    if(*bp_p==NULL){
      WARNING_MSG("No break points set at address 0x%x\n", adr);
      return 1;
    }
  }

  *bp_p = del_first_bp(*bp_p);
  return 0;

}//end del_bp_by_adr

bp_t* del_bp_all(bp_t* bp){

  if(bp==NULL){
    WARNING_MSG("No break points set\n");
    return NULL;
  }

  while(bp!=NULL) bp = del_first_bp(bp);

  return bp;

}//end del_bp_all

void print_bp(bp_t* bp){

    bp_t* bp_c = bp;
    int nb_bp=1;

    if(bp==NULL) WARNING_MSG("No break points set\n");

    else{
      do{
        printf("Break point n°%d is at address : 0x%x\n", nb_bp, bp_c->adr);
        bp_c = bp_c->suiv;
        nb_bp++;
      }while(bp_c!=NULL);
    }

}//end print_bp

int exec(type_t rij, int* pc, int check, emul_t vm){

  int opc = rij.opc;
  int rd = rij.rd;
  int rs = rij.rs;
  int rt = rij.rt;
  int sa = rij.sa;
  int imm = rij.imd;
  int offset = rij.jmp;

  //int ashr = (fnc%2 + fnc)/2;
  //int ashi = (opc & 0b1111) -  ( (opc & 0b1000) & ( (opc & 0b100000) >> 2 ) ) + ( (opc & 0b100000) >> 1 ) ;

  FP_jop exej = {J, JAL};
  FP_iop exei = {ADDI, ADDIU, ANDI, ORI, SLTI, SLTIU, LW, SW,
                 LB, LBU, SB, LUI, BEQ, BNE, BGEZ,
                 BGTZ, BLEZ, BLTZ};

  FP_rop exer = {ADD, ADDU, AND, SLT, SUB, XOR, OR, SRA,
                 SUBU, MUL, DIV, SLL, SRL, SLTU, MFHI,
                 MFLO, JALR, JR, BREAK, SYSCALL, SEB};



  switch (check) {
    case 1://type R
      exer[rij.exec -1 ](vm->reg, vm->seg, rd, rs, rt, sa, pc);
    break;

    case 2://type I
      exei[rij.exec -1 ](vm->reg, vm->seg, rs, rt, imm, pc);
    break;

    case 3://type J
      exej[opc-2](vm, offset, pc);
    break;

    default:
    break;
  }
return 0;

}//end exec
