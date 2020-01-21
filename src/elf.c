#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>


#include "common/notify.h"
#include "elf/elf.h"



int assert_elf_file( FILE *fp ) {

    if ( NULL != fp ) {
        Elf32_Ehdr ehdr;

        rewind( fp );

        if ( fread( &ehdr, sizeof(ehdr), 1, fp ) < 1 ) {
            WARNING_MSG( "Can't read ELF file header" );
            return 0;
        }

        return ( ehdr.e_ident[EI_MAG0] == 0x7f &&
                 ehdr.e_ident[EI_MAG1] == 'E'  &&
                 ehdr.e_ident[EI_MAG2] == 'L'  &&
                 ehdr.e_ident[EI_MAG3] == 'F'     );
    }

    return 0;
}


/**
 * Swaps symbol entry
 * @param sym the symbol entry to swap
 * @param width the size of the target processor word in bits
 * @param e the target endianness
 * @return the address of the swapped bytes (same as input)
 */

// static makes this function unknown outside this compilation unit (i.e., this file)
static byte * __Elf_Sym_flip_endianness( byte * sym, endianness e ) {
    if ( e == get_host_endianness() ) return sym;

    Elf32_Sym *e32 = (Elf32_Sym*)sym;

    FLIP_ENDIANNESS( e32->st_name );
    FLIP_ENDIANNESS( e32->st_value );
    FLIP_ENDIANNESS( e32->st_size );
    FLIP_ENDIANNESS( e32->st_shndx );

    return sym;
}



/**
 * Swaps section table
 * @param shdr the section table to swap
 * @param width the size of the target processor word in bits
 * @param e the target endianness
 * @return the address of the swapped section table (same as input)
 */

// static makes this function unknown outside this compilation unit (i.e., this file)
static byte * __Elf_Shdr_flip_endianness( byte * shdr, endianness e ) {
    if ( e == get_host_endianness() ) return shdr;

    Elf32_Shdr *e32 = (Elf32_Shdr*)shdr;

    FLIP_ENDIANNESS( e32->sh_name );
    FLIP_ENDIANNESS( e32->sh_type );
    FLIP_ENDIANNESS( e32->sh_flags );
    FLIP_ENDIANNESS( e32->sh_addr );
    FLIP_ENDIANNESS( e32->sh_offset );
    FLIP_ENDIANNESS( e32->sh_size );
    FLIP_ENDIANNESS( e32->sh_info );
    FLIP_ENDIANNESS( e32->sh_addralign );
    FLIP_ENDIANNESS( e32->sh_entsize );

    return shdr;
}


/**
 * Swaps ELF header
 * @param ehdr the header content to swap
 * @param width the size of the target processor word in bits
 * @param e the target endianness
 * @return the address of the swapped header content (same as input)
 */

// static makes this function unknown outside this compilation unit (i.e., this file)
static byte * __Elf_Ehdr_flip_endianness( byte * ehdr, endianness e ) {

    if ( e == get_host_endianness() ) return ehdr;

    Elf32_Ehdr *e32 = (Elf32_Ehdr*)ehdr;

    FLIP_ENDIANNESS( e32->e_type );
    FLIP_ENDIANNESS( e32->e_machine );
    FLIP_ENDIANNESS( e32->e_version );
    FLIP_ENDIANNESS( e32->e_entry );
    FLIP_ENDIANNESS( e32->e_phoff );
    FLIP_ENDIANNESS( e32->e_shoff );
    FLIP_ENDIANNESS( e32->e_flags );
    FLIP_ENDIANNESS( e32->e_ehsize );
    FLIP_ENDIANNESS( e32->e_phentsize );
    FLIP_ENDIANNESS( e32->e_phnum );
    FLIP_ENDIANNESS( e32->e_shentsize );
    FLIP_ENDIANNESS( e32->e_shnum );
    FLIP_ENDIANNESS( e32->e_shstrndx );

    return ehdr;
}


/**
 * Extracts the ELF header from ELF file
 * @param fp the pointer to the opened ELF file
 * @return the address of the raw content of the header in case of success, NULL otherwise
 */

byte * __elf_get_ehdr( FILE * fp ) {
    byte       *ehdr=NULL;
    char        ident[ EI_NIDENT ];
    endianness  endian;




    if ( NULL == fp ) {
        WARNING_MSG( "No file to read from" );
        return NULL;
    }

    rewind( fp );

    if ( fread( ident, EI_NIDENT, 1, fp ) < 1 ) {
        WARNING_MSG( "Corrupted ELF file" );
        return NULL;
    }

    rewind( fp );
    endian = ident[EI_DATA]==ELFDATA2LSB?LSB:MSB;


    ehdr = malloc( sizeof( Elf32_Ehdr ) );
    if ( fread( ehdr, sizeof( Elf32_Ehdr ), 1, fp ) < 1 ) {
        WARNING_MSG( "Corrupted ELF file" );
        return NULL;
    }
    __Elf_Ehdr_flip_endianness( ehdr, endian );

    return ehdr;
}



/*--------------------------------------------------------------------------  */
/** Fonction permettant de verifier si une chaine de caracteres
 * est bien dans la liste des symboles du fichier ELF
 * @param name le nom de la chaine recherchée
 * @param symtab la table des symboles
 *
 * @return 1 si present, 0 sinon
 */



int is_in_symbols(char* name, stab symtab) {
    int i;
    for (i=0; i<symtab.size; i++) {
        if (!strcmp(symtab.sym[i].name,name)) return 1;
    }
    return 0;
}

/*--------------------------------------------------------------------------  */
/**
 * Cette fonction calcule le nombre de segments à prevoir
 * Elle cherche dans les symboles si les sections predefinies
 * s'y trouve
 * @param symtab la table des symboles
 *
 * @return le nombre de sections trouvées
 */



unsigned int get_nsegments(stab symtab,char* section_names[],int nb_sections) {
    unsigned int n=0;
    int i;
    for (i=0; i<nb_sections; i++) {
        if (is_in_symbols(section_names[i],symtab)) n++;
    }
    return n;
}


/**
 * Gives the details of the architecture targeted by a given ELF file
 * @param fp INPUT the pointer to the opened ELF file
 * @param machine OUTPUT the family of processors this code is intended to (@see formats.h)
 * @param endianness OUTPUT the order with which bytes or a word are written/read in memory (@see types.h)
 * @param width OUTPUT the size of the target processor word in bits
 * @return 0 in case of success, a non null value otherwise
 */


int elf_get_arch_info( FILE *fp, endianness *endianness ) {

    if ( NULL != fp ) {
        Elf32_Ehdr ehdr;

        rewind( fp );

        if ( fread( &ehdr, sizeof(ehdr), 1, fp ) < 1 ) {
            WARNING_MSG( "Can't read ELF file header" );
            return 1;
        }

        switch( ehdr.e_ident[EI_DATA] ) {
        case ELFDATA2LSB :
            *endianness = LSB;
            break;
        case ELFDATA2MSB :
            *endianness = MSB;
            break;
        default :
            *endianness = UNDEF;
            break;
        }
        __Elf_Ehdr_flip_endianness( (byte*)&ehdr, *endianness );
        return 0;
    }

    return 1;
}



/**
 * Extracts section name table from ELF file
 * @param fp INPUT the pointer to the opened ELF file
 * @param size OUTPUT the size in bytes of the extracted content
 * @return the address of the raw content of the section name table in case of success, NULL otherwise
 */
byte *elf_extract_section_names( FILE *fp, uint *size ) {

    byte *sstab  = NULL;
    byte *ehdr   = __elf_get_ehdr( fp );
    uint  endian = 0 ;

    if ( NULL == ehdr ) {
        WARNING_MSG( "Can't read ELF file" );
        return NULL;
    }

    endian = ENDIANNESS_FROM_EHDR( ehdr );
    *size  = 1;

    if ( endian != ELFDATA2LSB && endian != ELFDATA2MSB ) {
        WARNING_MSG( "Wrong endianness" );
        free( ehdr );
        return NULL;
    }

    Elf32_Ehdr *e = (Elf32_Ehdr*)ehdr;

    if ( e->e_shstrndx == SHN_UNDEF ) {
        WARNING_MSG( "ELF file has no section names table" );
        return NULL;
    }

    if ( 0 == e->e_shnum ) {
        WARNING_MSG( "File has no section header table" );
        free( ehdr );
        return NULL;
    }

    fseek( fp, e->e_shstrndx*e->e_shentsize+e->e_shoff, SEEK_SET );

    Elf32_Shdr shdr;
    endianness en = (endian==ELFDATA2LSB?LSB:MSB);
    if ( fread( &shdr, sizeof( shdr ), 1, fp ) < 1 ) {
      WARNING_MSG( "Corrupted ELF file" );
      free( ehdr );
      return NULL;
    }

    __Elf_Shdr_flip_endianness( (byte*)&shdr, en );

    *size = shdr.sh_size;

    sstab = malloc( *size );

    if ( NULL == sstab ) {
        WARNING_MSG( "Can't allocate memory for section names table" );
        free( ehdr );
        return NULL;
    }

    fseek( fp, shdr.sh_offset, SEEK_SET );

    if ( fread( sstab, *size, 1, fp ) < 1 ) {
      WARNING_MSG( "Corrupted ELF file" );
      free( ehdr );
      return NULL;
    }
  free( ehdr );

  return sstab;
}

/**
 * Extracts the content of a section using its section name.
 * @param ehdr INPUT the ELF header data
 * @param fp INPUT the pointer to the opened ELF file
 * @param name INPUT the name of the section to extract
 * @param secsz OUTPUT the size in bytes of the extracted content
 * @param file_offset OUTPUT the offset in bit from the beginning of the file where the raw content of the section is stored
 * @return the address of the allocated memory space in which the raw content of the section is stored in case of success, NULL otherwise
 */

byte *elf_extract_scn_by_name( byte *ehdr, FILE *fp, char *name, uint *secsz, uint *file_offset ) {
    uint        sstabsz    = 0;
    byte       *sstab      = elf_extract_section_names( fp, &sstabsz );
    uint        eendian    = ENDIANNESS_FROM_EHDR( ehdr );
    endianness  endian     = eendian==ELFDATA2LSB?LSB:MSB;
    byte       *section    = NULL;


    if ( NULL == sstab ) {
        WARNING_MSG( "Unable to read section names table" );
        return NULL;
    }

    Elf32_Ehdr *e      = (Elf32_Ehdr*)ehdr;
    Elf32_Shdr  shdr;
    uint        i;

    for ( i= 0; i< e->e_shnum; i++ ) {
        fseek( fp, e->e_shoff+i*e->e_shentsize, SEEK_SET );

        if ( fread( &shdr, sizeof( shdr ), 1, fp ) < 1 ) {
            WARNING_MSG( "Corrupted ELF file: i == %u %u %u", i, e->e_shoff, e->e_shentsize );
            return NULL;
        }

        __Elf_Shdr_flip_endianness( (byte*)&shdr, endian );
        if ( 0 == strncmp( (char*)&sstab[shdr.sh_name], name, strlen(name) ) ) {
            break;
        }
    }

    if ( i == e->e_shnum ) {
        free( sstab );
        return NULL;
    }

    section = malloc( shdr.sh_size );

    if ( NULL == section ) {
        WARNING_MSG( "Unable to allocate memory for section %s", name );
        return NULL;
    }

    *secsz = shdr.sh_size;
    if ( file_offset )
        *file_offset = shdr.sh_offset;

    if ( SHT_NOBITS != shdr.sh_type ) {
        fseek( fp, shdr.sh_offset, SEEK_SET );
        if ( shdr.sh_size > 0 && fread( section, shdr.sh_size, 1, fp ) < 1 ) {
            WARNING_MSG( "Corrupted ELF file" );
            return NULL;
        }
    }
    free( sstab );
    return section;
}

/**
 * Extracts the section header table
 * @param ehdr the ELF header data
 * @param fp the pointer to the opened ELF file
 * @return the address at which the raw data is stored in case of success, NULL otherwise
 */

byte *elf_extract_section_header_table( byte * ehdr, FILE * fp ) {
    uint        sz      = 0;
    uint        eendian = ENDIANNESS_FROM_EHDR( ehdr );

    Elf32_Ehdr *e     = (Elf32_Ehdr*)ehdr;
    Elf32_Shdr *s     = NULL;
    Elf32_Shdr *shtab = NULL;
    uint i;

    if ( 0 == e->e_shnum  ) {
        WARNING_MSG( "No section header table to extract" );
        return NULL;
    }

    sz = e->e_shnum*sizeof(*s);

    shtab = malloc( sz );


    fseek( fp, e->e_shoff, SEEK_SET );
    fread( shtab, e->e_shnum, sizeof(*s), fp );

    for ( i= 0; i< e->e_shnum; i++ ) {
        __Elf_Shdr_flip_endianness( (byte*)&shtab[i], eendian );
    }

    return (byte*)shtab;
}

/**
 * Extracts the symbol table
 * @param ehdr INPUT the ELF header data
 * @param fp INPUT the pointer to the opened ELF file
 * @param name INPUT the name of the section
 * @param nsymbol OUTPUT the number of symbols in the table
 * @return the address at which the raw symbol table is stored in case of success, NULL otherwise
 */



byte *elf_extract_symbol_table( byte *ehdr, FILE *fp, char *name, unsigned int *nsymbol ) {
    uint        sz         = 0;
    byte       *symtab     = elf_extract_scn_by_name( ehdr, fp, name, &sz, NULL );

    if ( NULL == symtab ) {
        WARNING_MSG( "No symbol table found" );
        return NULL;
    }
    *nsymbol = sz/sizeof(Elf32_Sym);
    return symtab;
}

/**
 * load the symbol table into an symtab structure
 * @param fp INPUT the pointer to the opened ELF file
 * @param width INPUT the size of the target processor word in bits
 * @param endian INPUT the byte order of the targeted architecture of the ELF file
 * @param symtab OUTPUT the memory space in with the data must be stored
 * @return 0 in case of success, a non null value otherwise
 */



int elf_load_symtab( FILE *fp, unsigned int endian, stab *symtab ) {
    // remove all previous allocation
    if (NULL == symtab)
        ERROR_MSG( "Cannot load symbols in an unallocated memory space" );
    del_stab(*symtab);
    uint        nsyms   = 0;
    Elf32_Ehdr *ehdr    = (Elf32_Ehdr*)__elf_get_ehdr( fp );
    Elf32_Sym  *elf_sym = (Elf32_Sym*)elf_extract_symbol_table( (byte*)ehdr, fp, ".symtab", &nsyms );

    if ( 0 == nsyms ) {
        *symtab = new_stab( 0 );
        free( ehdr );
        free( elf_sym );
        return 0;
    }
    else {
        uint   i;
        uint   namesz = 0;
        uint   sz     = 0;
        char  *snames = (char*)elf_extract_scn_by_name( (byte*)ehdr, fp, ".strtab", &namesz, NULL );
        char *scnname = (char*)elf_extract_scn_by_name( (byte*)ehdr, fp, ".shstrtab", &sz, NULL );

        if ( NULL == scnname ) {
            WARNING_MSG( "Unable to extract section name table" );
            free( ehdr );
            free( elf_sym );
            return 1;
        }

        if ( NULL == snames ) {
            WARNING_MSG( "Unable to read ELF symbol names" );
            free( scnname );
            free( ehdr );
            free( elf_sym );
            return 1;
        }

        *symtab = new_stab( nsyms );

        for ( i= 0; i< nsyms; i++ ) {
            sym_type t;
            uint     shndx;


            __Elf_Sym_flip_endianness( (byte*)&elf_sym[i], endian );

            shndx = elf_sym[i].st_shndx;

            switch( ELF32_ST_TYPE( elf_sym[i].st_info ) ) {
            case STT_NOTYPE :
                t = notype;
                break;
            case STT_OBJECT :
                t = object;
                break;
            case STT_FUNC :
                t = function;
                break;
            case STT_SECTION :
                t = section;
                break;
            case STT_FILE :
                t = file;
                break;
            }


            //printf( "ST_NAME[%02u] info %d: %u : %s \n", i,t, elf_sym[i].st_name, &snames[elf_sym[i].st_name] );


            if ( section == t ) {

                // test the value of the st_name.
                if ('\0' == snames[elf_sym[i].st_name] ) {
                    // In case there is no symbol name for section... (used for .text)
                    uint _i, _j = 0;
                    for ( _i= 0; _i < shndx; _i++ ) {
                        _j += 1+strlen( &snames[_i] );
                    }
                    symtab->sym[i] = new_sym32( &snames[_j], elf_sym[i].st_value, elf_sym[i].st_size, t, shndx );
                }
                else {
                    symtab->sym[i] = new_sym32( &snames[elf_sym[i].st_name], elf_sym[i].st_value, elf_sym[i].st_size, t, shndx );
                }
            }
            else {
                symtab->sym[i] = new_sym32( &snames[elf_sym[i].st_name], elf_sym[i].st_value, elf_sym[i].st_size, t, shndx );

            }
        }

        free( snames );
        free( scnname );
    } // non-empty symtab
    free( ehdr );
    free( elf_sym );

    return 0;
}

/*--------------------------------------------------------------------------  */
/**
 * fonction permettant d'extraire une section du fichier ELF et de la chargée dans le segment du même nom
 * @param fp  		le pointeur du fichier ELF
 * @param memory      	la structure de mémoire virtuelle
 * @param scn         	le nom de la section à charger
 * @param permission  	l'entier représentant les droits de lecture/ecriture/execution
 * @param add_start   	l'addresse virtuelle à laquelle la section doit être chargée
 *
 * @return  0 en cas de succes, une valeur non nulle sinon
 */



int elf_load_section_in_memory(FILE* fp, emul_t memory, char* scn, unsigned int permissions,unsigned long long add_start) {
    byte *ehdr    = __elf_get_ehdr( fp );
    byte *content = NULL;
    uint  textsz  = 0;
    unsigned int sz;
    unsigned int addr;

    byte *useless = elf_extract_section_header_table( ehdr, fp );
    free( useless );

    if ( NULL == ehdr ) {
        WARNING_MSG( "Can't read ELF file" );
        return 1;
    }

    if ( 1 == attach_scn_to_mem(memory, scn, permissions )) {
        WARNING_MSG( "Unable to create %s section", scn );
        free( ehdr );
        return 1;
    }

    content = elf_extract_scn_by_name( ehdr, fp, scn, &textsz, NULL );
    if ( NULL == content ) {
        WARNING_MSG( "Corrupted ELF file" );
        free( ehdr );
        return 1;
    }

    sz   = textsz/*+8*/; /* +8: In case adding a final sys_exit is needed */
    addr = add_start;
    if ( 1 == fill_mem_scn(memory, scn, sz, addr, content )) {
        free( ehdr );
        free( content );
        WARNING_MSG( "Unable to fill in %s segment", scn );
        return 1;
    }

    free( content );
    free( ehdr );

    return 0;
}


/**
 * Extracts the section table
 * @param fp INPUT the pointer to the opened ELF file
 * @param width the size of the target processor word in bits
 * @param scntab OUTPUT the memory space in with the section table must be stored
 * @return 0 in case of success, a non null value otherwise
 */

int elf_load_scntab( FILE *fp, scntab *scntab ) {
    Elf32_Ehdr *ehdr     = (Elf32_Ehdr*)__elf_get_ehdr( fp );
    uint        nscns    = NULL!=ehdr?ehdr->e_shnum:0;
    Elf32_Shdr *elf_shdr = (Elf32_Shdr*)elf_extract_section_header_table( (byte*)ehdr, fp );

    if ( NULL == elf_shdr ) {
        WARNING_MSG( "Unable to extract section header table" );
        free( ehdr );
        return 1;
    }
    else {
        uint  i;
        uint  sz;
        char *scnname = (char*)elf_extract_scn_by_name( (byte*)ehdr, fp, ".shstrtab", &sz, NULL );

        if ( NULL == scnname ) {
            WARNING_MSG( "Unable to extract section name table" );
            free( ehdr );
            free( elf_shdr );
            return 1;
        }

        *scntab = new_scntab( nscns );

        for ( i= 0; i< nscns; i++ ) {
            scntab->scn[i].name = strdup( &scnname[elf_shdr[i].sh_name] );
            scntab->scn[i].addr = elf_shdr[i].sh_addr;
        }

        free( ehdr );
        free( elf_shdr );
        free( scnname );


        //print_scntab(*scntab );

        return 0;
    } /* has shdr */


    return 0;
}


// fonction affichant les octets d'un segment sur la sortie standard
// parametres :
//   seg        : le segment de la mémoire virtuelle à afficher

void print_segment_raw_content(seg_t* seg) {
    int k;
    int word =0;
    if (seg!=NULL && seg->size>0) {
        for(k=0; k<seg->size; k+=4) {
            if(k%16==0) printf("\n  0x%08x :",k);
            word = *((unsigned int *) (seg->val+k));
            FLIP_ENDIANNESS(word);
            printf("%08x ",	word);
        }
    }
}

/*--------------------------------------------------------------------------  */
/**
 * @param fp le fichier elf original
 * @param seg le segment à reloger
 * @param mem l'ensemble des segments
 *
 * @brief Cette fonction effectue la relocation du segment passé en parametres
 * @brief l'ensemble des segments doit déjà avoir été chargé en memoire.
 *
 * VOUS DEVEZ COMPLETER CETTE FONCTION POUR METTRE EN OEUVRE LA RELOCATION !!
 */
 void reloc_segment(FILE* fp, seg_t seg, emul_t vm, stab symtab, int n_relsc) {
     int i = 0, j= 0;
     byte *ehdr    = __elf_get_ehdr( fp );
     //ehdr = __Elf_Rel_flip_endianness(ehdr, MSB);
     uint32_t  scnsz  = 0;
     Elf32_Rel *rel = NULL;
     //Elf32_Rela *rela = NULL;
     char* reloc_name = malloc(strlen(seg.name)+strlen(RELOC_PREFIX_STR)+1);
     scntab section_tab;

     // on recompose le nom de la section
     memcpy(reloc_name,RELOC_PREFIX_STR,strlen(RELOC_PREFIX_STR)+1);
     strcat(reloc_name,seg.name);

     // on récupere le tableau de relocation et la table des sections
     rel = (Elf32_Rel *)elf_extract_scn_by_name( ehdr, fp, reloc_name, &scnsz, NULL );
     elf_load_scntab(fp , &section_tab);


 		//TODO : faire la relocation ci-dessous !
     if (rel != NULL && seg.val!=NULL && seg.size!=0) {
         int b = 0, cpt_w = 0;
         int v = 0, p = 0, a = 0;
         int ahi = 0;
         int alo = 0;
         int ahl = (ahi << 16) + (0xFFFF & alo);

         INFO_MSG("--------------Relocation de %s (0x%08x)-------------------\n",seg.name, seg.adr) ;
         INFO_MSG("Nombre de symboles a reloger: %ld\n",scnsz/sizeof(*rel)) ;

  	      //------------------------------------------------------

         printf("\n[%s]\nOffset\tInfo\tType\tSym.value\tSym.name\n", reloc_name);
         for (i = 0; i < scnsz/sizeof(*rel); i++) {
           if ( LSB == get_host_endianness() ){
             FLIP_ENDIANNESS( (rel+i)->r_offset );
             FLIP_ENDIANNESS( (rel+i)->r_info );
           }

           int sym = ELF32_R_SYM((rel+i)->r_info);
           int type = ELF32_R_TYPE((rel+i)->r_info);
           int offset = (rel+i)->r_offset;

           int s = vm->seg[sym-1].adr;

           a = 0;
           for (cpt_w = 0; cpt_w < 4; cpt_w++) {
             a += seg.val[offset + cpt_w] << (8 * (3 - cpt_w));
           }


           while (j < seg.size && j != (rel+i)->r_offset) {
             p = seg.adr + j + 1;
             j++;
           }

           switch (type) {
             case 2:
               v = s + a;
             break;
             case 4:
               p += 6;
               v = ((a << 2)|((p & 0xF0000000) + s)) >> 2;
             break;
             case 5:
               p += 16;
               ahi = a & 0xFFFF;
               alo = (seg.val[offset + 6] << 8) + seg.val[offset + 7];
               ahl = (ahi << 16) + (short)(alo);
               v = (seg.val[j] << 24) | (seg.val[j+1] << 16) | ((ahl + s) - (short)(ahl + s)) >> 16;
             break;
             case 6:
               p += 16;
               v = (seg.val[j] << 24) | (seg.val[j+1] << 16) | (short)(ahl + s);
             break;
           }

           for(cpt_w=0;cpt_w<4;cpt_w++){
             b = v << 8*cpt_w;
             b = b >> 24;
             vm->seg[n_relsc].val[j + cpt_w] =  0xFF & b;
           }
           printf("\n");
           printf("%x\t%x\t%x\t%x\t%s\n", (rel+i)->r_offset, (rel+i)->r_info, ELF32_R_TYPE((rel+i)->r_info), ELF32_R_SYM((rel+i)->r_info), symtab.sym[ELF32_R_SYM((rel+i)->r_info)].name);
           }
       }

         /*rela->r_offset = rel->r_offset;
         rela->r_info = rel->r_offset;
         rela->r_offset = rel->r_offset;
                 ELF32_R_SYM(i);
         ELF32_R_TYPE(i);*/

         //------------------------------------------------------

     del_scntab(section_tab);
     free( rel );
     free( reloc_name );
     free( ehdr );

 }//end reloc_segment
