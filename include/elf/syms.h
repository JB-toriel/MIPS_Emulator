
/**
 * @file syms.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Tue Nov  5 13:46:17 2013
 * @brief Binary symbols.
 *
 * Binary symbols.
 */

#ifndef _SYMS_H_
#define _SYMS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


typedef enum {
    notype, object, function, section, file, linked
}
sym_type;

typedef struct {
    char      *name;
    uint32_t   addr;
    uint       size;
    sym_type   type;
    uint     scnidx;
    uint     scnidx_link;
} sym;

typedef struct {
    uint       size;
    sym       *sym;
} stab;

sym  new_sym32( char *name, vaddr32 addr, uint size, sym_type type, uint scnidx );
void sym32_print( sym s );


stab new_stab( uint size );
void stab32_print( stab s );
void del_stab( stab s );

#ifdef __cplusplus
}
#endif

#endif /* _SYMS_H_ */
