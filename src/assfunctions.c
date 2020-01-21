#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"

#include "common/assfunctions.h"
#include "common/function.h"

//Type R

int ADD(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  DEBUG_MSG("rs %x", reg[rs].val);
  if (reg[rs].val + reg[rt].val > 2147483647) {
    WARNING_MSG("Int Overflow : %s", "ADD");
    return 1;
  }
  reg[rd].val = reg[rs].val + reg[rt].val;
  return 0;
}

int ADDU(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  reg[rd].val = reg[rs].val + reg[rt].val;
  return 0;
}

int AND(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  reg[rd].val = reg[rs].val & reg[rt].val;
  return 0;
}

int BREAK(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  brk = 1;
  return 0;
}

int DIV(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  if (reg[rt].val == 0) {
    WARNING_MSG("DIVISION PAR ZEROOOO !!!!! : %s", "DIV");
    return 1;
  }
  reg[32].val = reg[rs].val % reg[rt].val;
  reg[33].val = reg[rs].val / reg[rt].val;
  return 0;
}

int MFHI(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  reg[rd].val = reg[32].val;
  return 0;
}

int MFLO(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  reg[rd].val = reg[33].val;
  return 0;
}

int MUL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  int mult = (reg[rs].val & 0x7FFF)*(reg[rt].val & 0x7FFF);
  reg[32].val = mult & 0xFF00;
  reg[33].val = mult & 0xFF;
  if((reg[rs].val & 0x8000) != (reg[rt].val & 0x8000)) reg[32].val |= 0x8000;
  return 0;
}

int OR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  reg[rd].val = reg[rs].val | reg[rt].val;
  return 0;
}

int NOP(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  return 0;
}

int SLL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  DEBUG_MSG("rs %x", reg[rs].val);
  reg[rd].val = reg[rt].val << sa;
  return 0;
}

int SEB(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  if (reg[rt].val & 0x8000) reg[rd].val = reg[rt].val | 0xFF00;
  else reg[rd].val = reg[rt].val;
  return 0;
}

int SLT(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  if ((reg[rs].val & 0x7FFF) < (reg[rt].val & 0x7FFF) && (reg[rt].val & 0x8000) == 0) reg[rd].val = 0xFFFF;
  else reg[rd].val = 0x0000;
  return 0;
}

int SLTU(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  if ((reg[rs].val & 0x7FFF) < (reg[rt].val & 0x7FFF)) reg[rd].val = 0xFFFF;
  else reg[rd].val = 0x0000;
  return 0;
}

int SRL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  DEBUG_MSG("%x --> %x", reg[rt].val, reg[rt].val >> sa);
  reg[rd].val = reg[rt].val >> sa;
  return 0;
}

int SRA(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  int i = 0;
  int mask = 0x8000;
  while(i < sa) {
    reg[rd].val >>= 1;
    reg[rd].val = reg[rd].val | mask;
    mask >>= 1;
    i++;
  }
  return 0;
}

int SUB(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  reg[rd].val = reg[rs].val - reg[rt].val;
  return 0;
}

int SUBU(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  if (reg[rs].val - reg[rt].val < -2147483648 || reg[rs].val - reg[rt].val > 2147483647){
    WARNING_MSG("Int Overflow : %s", "SUB");
    return 1;
  }
  reg[rd].val = reg[rs].val - reg[rt].val;
  return 0;
}

int XOR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  reg[rd].val = reg[rs].val ^ reg[rt].val;
  return 0;
}

int JALR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  if (rd) reg[31].val = *pc;
  else reg[rd].val = *pc;
  *pc = reg[rs].val;
  return 0;
}

int JR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  *pc = reg[rs].val;
  return 0;
}

int SYSCALL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc){
  int i = 0;
  switch (reg[2].val) {
    case 1:
      printf("%d\n", reg[4].val);
    break;
    case 4:
      while(i<seg[1].size - 1 && (reg[4].val != seg[1].adr + i)){
        i++;
      }
      while (seg[1].val[i]) {
        printf("%c", seg[1].val[i]);
        i++;
      }
      printf("\n");
    break;
    case 5:
      scanf("%d\n", &reg[2].val);
    break;
    case 8:
      scanf("%s", (char*)&reg[4].val);
      if(reg[5].val < strlen((char*)&reg[4].val)){
        WARNING_MSG("String Overflow %d < %s : %s", reg[5].val, (char*)&reg[4].val, "SYSCALL");
      }
    break;
    case 10:
      printf("SYSCALL EXIT!\n");
    break;
  }
  return 0;
}

//Type I

int ADDI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc) {
  DEBUG_MSG("rs %x", reg[rs].val);
  if (reg[rs].val + imm > 2147483647) {
    WARNING_MSG("Int Overflow : %s", "ADDI");
    return 1;
  }
  reg[rt].val = reg[rs].val + imm;
  return 0;
}

int ADDIU(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc){
  reg[rt].val = reg[rs].val + imm;
  return 0;
}

int ANDI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc){
  reg[rt].val = reg[rs].val & imm;
  return 0;
}

int BGEZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  if (reg[rs].val >= 0) {
    *pc = *pc + (offset<<2);
  }
  return 0;
}

int BEQ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  DEBUG_MSG("%x == %x ?", reg[rs].val, reg[rt].val);
  if (reg[rs].val == reg[rt].val) {
    offset = offset<<2;
    *pc = *pc + offset;
  }
  return 0;
}

int BLEZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  if (reg[rs].val <= 0) {
    offset = offset<<2;
    *pc = *pc + offset;
  }
  return 0;
}

int BGTZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  if (reg[rs].val > 0) {
    offset = offset<<2;
    *pc = *pc + offset;
  }
  return 0;
}

int BNE(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  if (reg[rs].val != reg[rt].val) {
    offset = offset<<2;
    *pc = *pc + offset;
  }
  return 0;
}

int BLTZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  if (reg[rs].val < 0) {
    offset = offset<<2;
    *pc = *pc + offset;
  }
  return 0;
}

int LB(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){  //rs = reg[].val
  if(0x8000 & offset){
    rs = rs - (offset & 0x7FFF);
  }
  else{
    rs = rs + offset;
  }
  int i=0;
  for(i=0;i<4096;i++){
    if(rs == seg[0].adr + i){
      reg[rt].val = seg[0].val[i];
      break;
    }
  }
  return 0;
}

int LUI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc){
  reg[rt].val = imm << 16;
  return 0;
}

int LBU(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  rs = rs + offset;
  int i=0;
  for(i=0;i<4096;i++){
    if(rs == seg[0].adr + i){
      reg[rt].val = seg[0].val[i];
      break;
    }
  }
  return 0;
}

int LW(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  DEBUG_MSG("rs %x", reg[rs].val);
  int i=0, j=4, sec = 1;
  if (rs == 29) {
    sec = 3;
    i=seg[sec].size - 1;
    while(i>-1 && (reg[rs].val + offset != seg[sec].adr + i)){
      i--;
    }
  }
  else{
    while(i<seg[sec].size - 1 && (reg[rs].val + offset != seg[sec].adr + i)){
      i++;
    }
  }
  DEBUG_MSG("reg %x", reg[rt].val);
  reg[rt].val = 0;
  for (j = 0; j < 4; j++) {
    reg[rt].val += seg[sec].val[i - j] << 8 *j;
    DEBUG_MSG("reg %x + %x adr %x", reg[rt].val, seg[sec].val[i-j], seg[sec].adr+i-j);
  }
  return 0;
}

int SB(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  int i=0;
  for(i=0;i<4096;i++){
    if(rs == seg[0].adr + i){
      seg[0].val[i] = reg[rt].val & 0xFF;
      break;
    }
  }
  return 0;
}

int SW(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc){
  DEBUG_MSG("rs %x", reg[rs].val);
  int i=0, j=3, sec = 1;
  if (rs == 29) {
    sec = 3;
    i=seg[sec].size - 1;
    while(i>-1 && (reg[rs].val + offset != seg[sec].adr + i)){
      i--;
    }
    for (j = 0; j < 4 ; j++) {
      seg[sec].val[i-j] = (reg[rt].val >> (8*j)) & 0xFF;
    }
  }
  else{
    while(i<seg[sec].size - 1 && (reg[rs].val + offset != seg[sec].adr + i)){
      i++;
    }
    for (j = 0; j < 4 ; j++) {
      seg[sec].val[i+j] = (reg[rt].val >> (8*(3-j))) & 0xFF;
    }
  }
  DEBUG_MSG("reg %x mem %02hhx%02hhx%02hhx%02hhx", reg[rt].val, seg[sec].val[i], seg[sec].val[i-1], seg[sec].val[i-2], seg[sec].val[i-3]);
  return 0;
}

int ORI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc){
  reg[rt].val = reg[rs].val | imm;
  return 0;
}

int SLTI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc){
  if ((reg[rs].val & 0x7FFF) < imm && (imm & 0x8000) == 0) reg[rt].val = 0xFFFF;
  else reg[rt].val = 0x0000;
  return 0;
}

int SLTIU(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc){
  if ((reg[rs].val & 0x7FFF) < imm) reg[rt].val = 0xFFFF;
  else reg[rt].val = 0x0000;
  return 0;
}

//Type J

int J(emul_t vm, int target, int* pc){
  *pc = (*pc & 0xF000) | (target << 2);
  return 0;
}

int JAL(emul_t vm, int target, int* pc){
  vm->reg[31].val = *pc;
  *pc = (*pc & 0xF000) | (target << 2);
  return 0;
}
