#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"
#include "common/mem.h"

/**
 * Creates a virtual memory and performs the necessary memory allocations
 * @param size  the number of segments that composes this virtual memory
 * @return the new allocated virtual memory or NULL in case of failure
 */
emul_t  init_mem( unsigned int n_seg ) {

    emul_t vm = (emul_t)malloc( sizeof( *vm ) );

    if ( NULL == vm ) {
        WARNING_MSG( "Unable to allocate host memory for vmem" );
        return NULL;
    }
    else {
        int i;

        vm->seg = (seg_t*)malloc( n_seg*sizeof( *(vm->seg) ) );
        if ( NULL == vm->seg ) {
            WARNING_MSG( "Unable to allocate host memory for vmem segment" );
            free(vm->seg);
            free( vm );
            return NULL;
        }

        // each segment is initialised to a null value
        // Note that though this is unnecessary since a malloc is used
        // this permits future evolution of the default initialisation values
        for ( i= 0; i< n_seg; i++ ) {
            vm->seg[i].name = NULL;
            vm->seg[i].size = 0x0;
            vm->seg[i].val = NULL;
            vm->seg[i].adr = 0x0;
            vm->seg[i].right = 0;
        }

        vm->n_seg = n_seg;
    }

    return vm;
}//end init_mem

reg_t* init_reg(emul_t vm){
  int i;
  vm->reg = (reg_t*)malloc(NBR_REG*sizeof(*(vm->reg)));//Initialisation des 32 registres
  for(i=0;i<NBR_REG;i++){
    vm->reg[i].adr = registre[i];
    vm->reg[i].val = 0;
  }
  vm->reg[29].val = 0xFFFFF000 - 1;

  return vm->reg;
}

/**
 * allocates an available segment in the virtual memory to the section named 'name'.
 * @param name the name of the segment (e.g., ".text", ".data"...)
 * @param attribute an integer value corresponding to the access and execution permissions of the segment (@see mem.h)
 * @return 0 in case of success, a non null value otherwise
 */

int attach_scn_to_mem( emul_t vm, char *name, uint32_t right ) {

    if ( NULL != vm ) {
        int i;

        for ( i= 0; i< vm->n_seg; i++ ) {
            if ( NULL == vm->seg[i].name ) {
                vm->seg[i].right = right;
                vm->seg[i].name = strdup( name );//copy name
                return 0;
            }
        }

        WARNING_MSG( "All virtual memory segments already allocated" );
        return 1;
    }
    else {
        WARNING_MSG( "Virtual memory not initialized" );
        return 1;
    }
}//end allocate_seg


/**
* prints the content of a virtual memory
* @param a virtual memory
*/

void print_mem( emul_t vm ) {
    if ( NULL != vm ) {
        uint i;
        uint n = 0;

        for ( i= 0; i< vm->n_seg; i++ ) {
            if ( UNDEF != SCN_RIGHTS( vm->seg[i].right ) ) {
                n++;
            }
        }

        printf( "Virtual memory map (%u segments)\n", n );

        for ( i= 0; i< vm->n_seg; i++ ) {

            if ( UNDEF == SCN_RIGHTS( vm->seg[i].right ) ) {
                continue;
            }

            printf( "%-8s\t", vm->seg[i].name );
            switch( SCN_RIGHTS( vm->seg[i].right ) ) {
            case R__ :
                printf( "r--" );
                break;
            case RW_ :
                printf( "rw-" );
                break;
            case R_X :
                printf( "r-x" );
                break;
            default :
                printf( "???" );
            }
            printf( "\tVaddr: " );
            printf( "0x%08x", vm->seg[i].adr );
            printf( "\tSize: " );
            printf( "%u", vm->seg[i].size );
            printf( " bytes\n" );

            printf( " bytes\t(%s)\n", vm->seg[i].val==NULL?"empty":"filled" );

        }
    }

    return;
}


/**
 * Destroys a virtual memory by making all the necessary free operations
 * @param a virtual memory
 */

void del_mem( emul_t vm ) {

    if ( NULL != vm ) {

        if ( NULL != vm->seg ) {
            int i;

            for ( i= 0; i< vm->n_seg; i++ ) {
                if ( NULL != vm->seg[i].val ) {
                    free( vm->seg[i].val );
                }
                if ( NULL != vm->seg[i].name ) {
                    free( vm->seg[i].name );
                }
            }

            free( vm->seg );
        }

        free( vm );
    }

    return;
}//end del_mem


/**
 * Sets a segment with all the information about a section read from a ELF file
 * @param vm the virtual memory in which the segment to fill in can be found
 * @param name the name of the segment to look for (e.g., ".text", ".data"...)
 * @param size the size of the section content (in bytes)
 * @param start the virtual address in the virtual memory at which the beginning of the segment is located
 * @param content the real address at which the bytes of the section is located
 * @return 0 if succeed, a non null value otherwise
 */

int fill_mem_scn( emul_t vm, char *name, uint32_t sz, unsigned int start, byte *content ) {

    if ( NULL != vm ) {
        uint i;

        for ( i= 0; i< vm->n_seg; i++ ) {
            if ( 0 == strncasecmp( vm->seg[i].name, name, strlen(name) ) ) {
              vm->seg[i].size = sz;
              vm->seg[i].adr = start;
              vm->seg[i].val = (unsigned char*)malloc( sz );
              if ( NULL == vm->seg[i].val ) {
                  WARNING_MSG( "Unable to allocate host memory for vmem segment %s val", name );
                  vm->seg[i].size = 0;
                  vm->seg[i].adr = 0;
                  return 1;
              }
              if ( NULL != content ) {
                  memcpy( vm->seg[i].val, content, sz );
              }
              return 0;
            }
        }

        WARNING_MSG( "Segment %s not attached yet", name );
        return 1;
    }
    else {
        WARNING_MSG( "Virtual memory not initialized" );
        return 1;
    }
}
