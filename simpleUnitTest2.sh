#!/bin/bash

EXT=/*.emu

#read -p "Dossier : "  DIR
#echo "Dossier $DIR"

DIR=${1?Erreur: pas de dossier rentré}
read -p "Paramètres : " PARAM

find $DIR -type d -exec ./simpleUnitTest.sh -e ./emulMips $PARAM {}$EXT \; #dossier par dossier

#find ./test/ -name "*.emu" -type f -exec ./simpleUnitTest.sh -e ./emulMips  -v -d {} \; #fichier par fichier
#a penser anvant d'exec la toute première fois : chmod +x nom_du_fichier_.sh
