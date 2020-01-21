#ifndef _ASS_FUNCTIONS_H
#define _ASS_FUNCTIONS_H

#include "mem.h"

extern int brk;

//==============================type R==============================

int ADD(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int ADDU(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int AND(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int BREAK(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int DIV(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int MFHI(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int MFLO(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int MUL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int OR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int NOP(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SLL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SEB(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SLT(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SLTU(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SRL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SRA(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SUB(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SUBU(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int SYSCALL(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int XOR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int JALR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

int JR(reg_t* reg, seg_t* seg, int rd, int rs, int rt, int sa, int* pc);

//==============================Type I==============================

int ADDI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc);

int ADDIU(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc);

int ANDI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc);

int BGEZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc);

int BEQ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc);

int BLEZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc);

int BGTZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc);

int BNE(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc);

int BLTZ(reg_t* reg, seg_t* seg, int rs, int rt, int offset, int* pc);

int LB(reg_t* reg, seg_t* seg, int base, int rt, int offset, int* pc);  //base = reg[].val

int LUI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc);

int LBU(reg_t* reg, seg_t* seg, int base, int rt, int offset, int* pc);

int LW(reg_t* reg, seg_t* seg, int base, int rt, int offset, int* pc);

int SB(reg_t* reg, seg_t* seg, int base, int rt, int offset, int* pc);

int SW(reg_t* reg, seg_t* seg, int base, int rt, int offset, int* pc);

int ORI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc);

int SLTI(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc);

int SLTIU(reg_t* reg, seg_t* seg, int rs, int rt, int imm, int* pc);

//==============================Type J==============================

int J(emul_t vm, int target, int* pc);

int JAL(emul_t vm, int target, int* pc);

#endif
