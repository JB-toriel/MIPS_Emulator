#ifndef _ELF_H_
#define _ELF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "elf/syms.h"
#include "elf/section.h"
#include "common/mem.h"
#include "common/bits.h"

// nombre max de sections que l'on extraira du fichier ELF
#define NB_SECTIONS 4

// nom de chaque section
#define TEXT_SECTION_STR ".text"
#define RODATA_SECTION_STR ".rodata"
#define DATA_SECTION_STR ".data"
#define BSS_SECTION_STR ".bss"

//nom du prefix à appliquer pour la section
#define RELOC_PREFIX_STR ".rel"


typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

#define EI_NIDENT     16

typedef struct {

    unsigned char e_ident[EI_NIDENT];
    Elf32_Half    e_type;
    Elf32_Half    e_machine;
    Elf32_Word    e_version;
    Elf32_Addr    e_entry;
    Elf32_Off     e_phoff;
    Elf32_Off     e_shoff;
    Elf32_Word    e_flags;
    Elf32_Half    e_ehsize;
    Elf32_Half    e_phentsize;
    Elf32_Half    e_phnum;
    Elf32_Half    e_shentsize;
    Elf32_Half    e_shnum;
    Elf32_Half    e_shstrndx;

} Elf32_Ehdr;

#define ET_NONE    0
#define ET_REL     1
#define ET_EXEC    2
#define ET_DYN     3
#define ET_CORE    4
#define ET_LOPROC  0xff00
#define ET_HIPROC  0xffff

#define EV_NONE    0
#define EV_CURRENT 1

#define EI_MAG0    0
#define EI_MAG1    1
#define EI_MAG2    2
#define EI_MAG3    3
#define EI_CLASS   4
#define EI_DATA    5
#define EI_VERSION 6
#define EI_PAD     7
#define EI_NIDENT  16

#define ELF_MAG0   0x7f
#define ELF_MAG1   'E'
#define ELF_MAG2   'L'
#define ELF_MAG3   'F'

#define ELFDATANONE  0
#define ELFDATA2LSB  1
#define ELFDATA2MSB  2

#define SHN_UNDEF     0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC    0xff00
#define SHN_HIPROC    0xff1f
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2
#define SHN_HIRESERVE 0xffff

typedef struct {

    Elf32_Word   sh_name;
    Elf32_Word   sh_type;
    Elf32_Word   sh_flags;
    Elf32_Addr   sh_addr;
    Elf32_Off    sh_offset;
    Elf32_Word   sh_size;
    Elf32_Word   sh_link;
    Elf32_Word   sh_info;
    Elf32_Word   sh_addralign;
    Elf32_Word   sh_entsize;

} Elf32_Shdr;

#define SHT_NULL       0
#define SHT_PROGBITS   1
#define SHT_SYMTAB     2
#define SHT_STRTAB     3
#define SHT_RELA       4
#define SHT_HASH       5
#define SHT_DYNAMIC    6
#define SHT_NOTE       7
#define SHT_NOBITS     8
#define SHT_REL        9
#define SHT_SHLIB      10
#define SHT_DYNSYM     11
#define SHT_LOPROC     0x70000000
#define SHT_HIPROC     0x7fffffff
#define SHT_LOUSER     0x80000000
#define SHT_HIUSER     0xffffffff

#define SHF_WRITE      0x1
#define SHF_ALLOC      0x2
#define SHF_EXECINSTR  0x4
#define SHF_MASKPROC   0xf0000000

#define STN_UNDEF      0


typedef struct {

    Elf32_Word      st_name;
    Elf32_Addr      st_value;
    Elf32_Word      st_size;
    unsigned char   st_info;
    unsigned char   st_other;
    Elf32_Half      st_shndx;

} Elf32_Sym;


#define ELF32_ST_BIND(i)    ((i)>>4)
#define ELF32_ST_TYPE(i)    ((i)&0xf)
#define ELF32_ST_INFO(b,t)  (((b)<<4)+((t)&0xf))

#define STB_LOCAL       0
#define STB_GLOBAL      1
#define STB_WEAK        2
#define STB_LOPROC      13
#define STB_HIPROC      15

#define STT_NOTYPE      0
#define STT_OBJECT      1
#define STT_FUNC        2
#define STT_SECTION     3
#define STT_FILE        4
#define STT_LOPROC      13
#define STT_HIPROC      15


typedef struct {

    Elf32_Addr   r_offset;
    Elf32_Word   r_info;

} Elf32_Rel;

typedef struct {

    Elf32_Addr   r_offset;
    Elf32_Word   r_info;
    Elf32_Sword  r_addend;

} Elf32_Rela;


#define ELF32_R_SYM(i)      ((i)>>8)
#define ELF32_R_TYPE(i)     ((unsigned char)(i))
#define ELF32_R_INFO(s,t)   (((s)<<8)+(unsigned char)(t))

typedef struct {

    Elf32_Word      p_type;
    Elf32_Off       p_offset;
    Elf32_Addr      p_vaddr;
    Elf32_Addr      p_paddr;
    Elf32_Word      p_filesz;
    Elf32_Word      p_memsz;
    Elf32_Word      p_flags;
    Elf32_Word      p_align;

} Elf32_Phdr;


#define PT_NULL      0
#define PT_LOAD      1
#define PT_DYNAMIC   2
#define PT_INTERP    3
#define PT_NOTE      4
#define PT_SHLIB     5
#define PT_PHDR      6
#define PT_LOPROC    0x70000000
#define PT_HIPROC    0x7fffffff

#define ENDIANNESS_FROM_EHDR(ehdr) ((unsigned char*)ehdr)[EI_DATA]


byte * __Elf_Rel_flip_endianness( byte * ehdr, endianness e );


unsigned int get_nsegments(stab symtab,char* section_names[],int nb_sections);
int is_in_symbols(char* name, stab symtab);
int elf_load_section_in_memory(FILE* fp, emul_t memory, char* scn,unsigned int permissions,unsigned long long add_start);
void print_segment_raw_content(seg_t* seg);
int elf_load_scntab( FILE *fp, scntab *scntab );
void reloc_segment(FILE* fp, seg_t seg, emul_t memory, stab symtab, int n_relsc);

int  assert_elf_file( FILE *fp );
byte * __elf_get_ehdr( FILE * fp );
int elf_get_arch_info( FILE *fp, endianness *endianness );
byte *elf_extract_scn_by_name( byte *ehdr, FILE *fp, char *name, uint *secsz, uint *file_offset );
byte *elf_extract_string_table( byte *ehdr, FILE *fp, char *name, uint *sz );
byte *elf_extract_section_names( FILE *fp, uint *size );
void  print_string_table( byte *stab, unsigned int sz );
byte *elf_extract_symbol_table( byte *ehdr, FILE *fp, char *name, unsigned int *nsymbol );

byte *elf_extract_section_header_table( byte * ehdr, FILE * fp );
int elf_load_scntab( FILE *fp, scntab *scntab );
int elf_load_symtab( FILE *fp, unsigned int endian, stab *symtab );

#ifdef __cplusplus
}
#endif

#endif /* _ELF_H_ */
