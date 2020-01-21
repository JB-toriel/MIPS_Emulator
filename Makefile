TARGET=emulMips
TEST=test.c
EXETEST=w

# noms des executables utilisés durant la compilation/edition des liens
# names of executables used during compilation / edition of links
CC=`which gcc`
LD=`which gcc`
RM=`which rm` -f
DOXYGEN=`which doxygen`

#options de compilation/edition des liens
#editing options / link compilation  
INCLUDE=-I$(INCDIR)
CFLAGS=-Wall $(INCLUDE)
LFLAGS=-lreadline -lm -lcurses
CFLAGS_DBG=$(CFLAGS) -g -DDEBUG -DVERBOSE
CFLAGS_RLS=$(CFLAGS)


# definition des repertoires de source/destination
# definition of directories source / destination 
SRCDIR=src
INCDIR=include
DOCDIR=doc

# les fichiers dont on peut se débarasser
# the files we can get rid of
GARBAGE=*~ $(SRCDIR)/*~ $(INCDIR)/*~ $(TESTDIR)/*~ $(SRCDIR)/*.orig $(INCDIR)/*.orig

# ou se trouve les sources (i.e., le *.c)
# where the sources are located (i.e., the * .c)
SRC=$(wildcard $(SRCDIR)/*.c)

# les objets avec l'option DEBUG s'appeleront fichier.dbg.o
# ceux de la release fichier.rls.o
# objects with the DEBUG option will be called file.dbg.o
# those of the release file.rls.o

OBJ_DBG=$(SRC:.c=.dbg.o)
OBJ_RLS=$(SRC:.c=.rls.o)

# 1er target (celui executé par défaut quand make est lancé sans nom de cible) 
# affiche l'aide
# 1st target (the one executed by default when make is launched without target name)
# show help
all : 
	@echo ""
	@echo "Usage:"
	@echo ""
	@echo "make debug   => build DEBUG   version"
	@echo "make release => build RELEASE version"
	@echo "make doc     => produce the doxygen documentation"
	@echo "make clean   => clean everything"
	@echo "make tarball => produce archive .tar.gz in ../ directory"

debug   : $(OBJ_DBG)
	$(LD) $^ $(LFLAGS) -o $(TARGET)

release : $(OBJ_RLS)
	$(LD) $^ $(LFLAGS) -o $(TARGET)

test 	: $(OBJ_DBG)
	$(LD) $(TEST) $(LFLAGS) -o $(EXETEST)

%.rls.o : %.c
	$(CC) $< $(CFLAGS_RLS) -c -o $(basename $<).rls.o

%.dbg.o : %.c
	$(CC) $< $(CFLAGS_DBG) -c -o $(basename $<).dbg.o

doc : 
	$(DOXYGEN)

clean : 
	$(RM) $(TARGET) $(EXETEST) $(SRCDIR)/*.o $(GARBAGE) 
	$(RM) -r $(DOCDIR)/*	

# créé l'archive 
# created the archive

archive : 
	make clean 
	tar -czvf ../$(notdir $(PWD) )-`whoami`-`date +%d-%m-%H-%M`.tgz .
	echo "Fichier archive ../emulMips-`whoami`-`date +%d-%m-%H-%M`.tgz genere"
