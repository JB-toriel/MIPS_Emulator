#ifndef _IS_H
#define _IS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"
#define NBR_REG 35

extern char* const registre[];


int is_int(char* chaine);
int is_signed_int(char* chaine);
int is_signed_byte(char* chaine);
  int is_hexa_byte(char* chaine);
int is_hexa(char* chaine) ;
int is_adresse(char* chaine);
int is_adresse_load(char* chaine);
int is_registre(char* chaine);

#endif
