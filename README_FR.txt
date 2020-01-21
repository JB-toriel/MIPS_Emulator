===Fichiers .c :===

./src/
-Emplacement des fichiers les plus récents et à compiler

emulMips.c 	Contient le main et tout l'embryon de code modifié.

function_cmd.c	Contient toutes les fonctions qui gèrent la grammaire des commandes 
		et le parsing.

function.c	Contient toutes les fonctions d'execution des commandes de l'interpréteur

assfunction.c 	Contient toutes les fonctions des instructions assembleurs

mem.c		Commandes sur la mémoire

is.c		Commande sur les fonctions de type identité (sauf is_range)

elf.c, syms.c, section.c, bits.c, types.c 
		Commandes pour les fichiers ELF


===Fichiers .h :===

./include/common/

notify.h 	Aucun changement apporté.

macro.h		Contient tout les #define

function_cmd.h	Contient tous les prototypes des fonctions de function_cmd.c
		et la structure de l'interpreteur

function.h	-Contient tous les prototypes des fonctions de function.c
		-Les structures "opc_t" et "type_t" pour le disasm.
		-Les prototypes des 3 tableaux de pointeurs de fonctions (R,I,J).

assfunction.h 	Contient toutes les prototypes des instructions assembleurs

mem.h		-Contient tous les prototypes des fonctions de mem.c
		-Les structures des breakpoint, de la machine virtuelle, des registres et des segments

is.h		Prototypes des fonctions de type identité (sauf is_range)

./include/elf/	Contient les fichiers headers pour le bon fonctionnement du load

===Dossiers annexes===

./dico/
-Emplacement des dictionnaires utilisés lors du disasm

type.txt 	Contient tous les opcodes de toutes les fonctions, tous types confondus

R.txt		Contient tous les codes de fonctions de type R

I.txt		Contient tous les opcodes des fonctions de type I

J.txt		Contient tous les opcodes des fonctions de type J

./demo/
-Emplacement des scripts de test fonctionnels  

./test/
-Emplacement de l'ensemble des fichiers de test 


===Makefile===
Pour lancer le programme d'écriture de test FAIL
$ make test

pour executer : ./w

===simpleUnitTest2.sh===
Réutilisation du script simpleUnitTest.sh pour une execution dossier par dossier avec reccurtion

syntaxe : ./simpleUnitTest2.sh test/


