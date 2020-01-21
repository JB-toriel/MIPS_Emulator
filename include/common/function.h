#ifndef _FUNCTION_H
#define _FUNCTION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"

#include "elf/elf.h"
#include "common/is.h"
#include "common/function_cmd.h"
#define NBR_REG 35


extern char* const registre[];
extern bp_t* bpp;
extern stab symtab;


//struct associant une instruction assembleur a son code binaire
typedef struct{
  int val;
  int exec;
  char code[255];
}opc_t;

//struct pour les operations de types R,I ou J
typedef struct{
  char* name_fnc;
  char* rs_c;
  char* rt_c;
  char* rd_c;
  char* sa_c;
  int exec;
  int opc;
  int fnc;
  int rs;
  int rt;
  int rd;
  int sa;
  int imd;
  int jmp;
}type_t;

/*Tableau de pointeur de fonction pour les fonctions d'execution de type R*/
typedef int (*FP_rop[21])(reg_t*, seg_t*, int, int, int, int, int*);


/*Tableau de pointeur de fonction pour les fonctions d'execution de type R*/
typedef int (*FP_iop[19])(reg_t*, seg_t*, int, int, int, int*);


/*Tableau de pointeur de fonction pour les fonctions d'execution de type R*/
typedef int (*FP_jop[2])(emul_t, int, int*);


int is_range(char* chaine, interpreteur inter, char* valeurs[]);

int assert_byte(emul_t vm, int adr, char val);
int assert_word(emul_t vm, int adr, int val);
int assert_reg(reg_t* reg, int adr, int val);

int disp_mem(int adr, int adr2 ,emul_t vm);
int disp_reg(char* token,reg_t* reg);
void disp_map(emul_t vm);

int load(emul_t vm, int adr, char* name);

int set_mem_byte(char* token, unsigned int n_adr, emul_t vm);
int set_mem_word(char* token, unsigned int n_adr, emul_t vm);
int set_reg(char* token, int n_reg, reg_t reg[]);

int char_to_bin(char c[], int len);
int disasm_word(int cpt, type_t* rij, seg_t seg[]);
int print_disasm(emul_t vm, int disasm, type_t rij, int cpt);
int search_adr(int* adresse, int* range, seg_t seg[]);

type_t init_rij(void);

bp_t* create_bp(int adr);
bp_t* add_bp(bp_t* bp, int adr);
bp_t* sort_bp(bp_t* bp);
bp_t* del_first_bp(bp_t* bp);
int del_bp_by_adr(bp_t** bp, int adr);
bp_t* del_bp_all(bp_t* bp);
void print_bp(bp_t* bp);

int stepinto(emul_t vm, type_t rij, int* pc);
int step(emul_t vm, type_t rij, int* pc);
int run(emul_t vm, type_t rij, int* pc);

int exec(type_t rij, int* pc, int check, emul_t vm);

#endif
