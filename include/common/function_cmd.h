#ifndef _FUNCTION_CMD_H
#define _FUNCTION_CMD_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "elf/elf.h"

/* prompt du mode shell interactif */
#define PROMPT_STRING "MipsShell : > "

/* taille max pour nos chaines de char */
#define MAX_STR 1024

/*************************************************************\
Valeur de retour speciales pour la fonction
	int execute_cmd(interpreteur inter) ;
Toute autre valeur signifie qu'une erreur est survenue
 \*************************************************************/
#define CMD_OK_RETURN_VALUE 0
#define CMD_EXIT_RETURN_VALUE -1
#define CMD_UNKOWN_RETURN_VALUE -2


// nombre max de sections que l'on extraira du fichier ELF
#define NB_SECTIONS 4

// nom de chaque section
#define TEXT_SECTION_STR ".text"
#define RODATA_SECTION_STR ".rodata"
#define DATA_SECTION_STR ".data"
#define BSS_SECTION_STR ".bss"

//nom du prefix à appliquer pour la section
#define RELOC_PREFIX_STR ".rel"

//extern REGISTER* mem=NULL;
extern char* const registre[];
extern bp_t* bpp;

/* type de token (exemple) */
enum {HEXA,UNKNOWN};

typedef enum {INIT, FILE_NAME, ADR, MEM, MAP, REG, TYPE, REGISTER, VALUE, WORD, BYTE, END, ADDB, DEL, LIST} STATE_ARG;


/* mode d'interaction avec l'interpreteur (exemple)*/
typedef enum {INTERACTIF,SCRIPT,DEBUG_MODE} inter_mode;


/* structure passée en parametre qui contient la connaissance de l'état de
 * l'interpréteur
 */
typedef struct {
    inter_mode mode;
    char input[MAX_STR];
    char * from;
    char first_token;
} *interpreteur;


char* get_next_token(interpreteur inter);

int debugcmd(interpreteur inter);
int resumecmd(interpreteur inter);

int exitcmd(interpreteur inter);
int assertcmd(interpreteur inter, emul_t vm);
int disasmcmd(interpreteur inter, emul_t vm);
int dispcmd(interpreteur inter, emul_t vm);
int helpcmd(interpreteur inter);
int loadcmd(interpreteur inter, emul_t vm);
int runcmd(interpreteur inter, emul_t vm, int* pc);
int setcmd(interpreteur inter, emul_t vm);
int breakcmd(interpreteur inter, emul_t vm);
int stepcmd(interpreteur inter, emul_t vm, int* pc);

#endif
