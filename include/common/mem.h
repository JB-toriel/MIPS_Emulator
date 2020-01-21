#ifndef _MEM_H
#define _MEM_H


#include <stdlib.h>
#include <stdio.h>

//#include "common/macro.h"
#include "elf/types.h"

#define R__   1
#define RW_   2
#define R_X   3

#define SCN_RIGHTS(right)         (((right)&0x000000ff))

#define NBR_REG 35

extern char* const registre[];

/*Structure des registres*/
typedef struct{
  int val;
  char* adr;
} reg_t;


typedef struct{
  char* name;
  unsigned char* val;
  unsigned int size;
  unsigned int adr;
  uint32_t right;
} seg_t;

typedef struct{
  reg_t* reg;
  unsigned int n_seg;
  seg_t* seg;
} *emul_t;

//breakpoints struct
typedef struct bp_t{
  unsigned int adr;
  unsigned int flag;
  struct bp_t* suiv;
}bp_t;


emul_t  init_mem( unsigned int n_seg );
reg_t* init_reg(emul_t vm);
int attach_scn_to_mem( emul_t vm, char *name, uint32_t right );
int fill_mem_scn( emul_t vm, char *name, uint32_t sz, unsigned int start, byte *content );
void print_mem( emul_t vm );
void del_mem( emul_t vm );

#endif
