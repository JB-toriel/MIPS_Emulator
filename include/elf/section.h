/**
 * @file section.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Tue Nov  5 14:54:23 2013
 * @brief Section handling.
 *
 * Section handling.
 */

#ifndef _SECTION_H_
#define _SECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

typedef struct {
    uint32_t  addr;
    char   *name;
} scn;

typedef struct {
    uint   size;
    scn   *scn;
} scntab;

scntab new_scntab( uint size );
void   del_scntab( scntab scntab );
void   print_scntab( scntab scntab );

#ifdef __cplusplus
}
#endif

#endif /* _SECTION_H_ */
