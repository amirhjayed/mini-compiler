#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "nonterms.h"
#include "anal_lex.h"
#include "structs.h"
#include "globals.h"

int traduction_flag=1;

/* external Globals */
extern FILE *fs, *fd;
extern symb_t symbole;
extern idef_t *id_array;
extern int id_size;
extern int id_head;
extern int etat ;
extern int dcl_flag; // utilisé par analyse sémantique pour determiner si declaration mal placée
extern int new_id_flag;
extern int l_sz; //used by liste_id() to track the number of declared variables.
extern int etiq_ctr; // label counter

extern int f_line;
extern int f_column;


int main(int argc, char** argv){
    etiq_ctr=0;
    etat=0;
    eof_flag=0;
    int i = 0 ;
    if(argc != 3){
        printf("arguments manquants\n");
        return -1;
    }
    fs = fopen(argv[1], "r");
    f_line=1;
    f_column=1;

    fd = fopen(argv[2], "w+");
    id_size = 5 ;
    id_head = 0 ;
    id_array = malloc(id_size * sizeof(idef_t));

    if((fs) && (fd)){
        printf("Compilation de fichier source '%s'...\n\n",argv[1]);
        p();
    }
    else {
        printf("\nfichiers non ouvrit correctement.\n");
    }

    if(!traduction_flag){
        fclose(fd);
        remove(argv[2]);
        printf("Traduction echouee.\n\n");
    }
    else{
        printf("\nTraduction reussite!\n Fichier destination: '%s'",argv[2]);
    }

    fclose(fs);
    fclose(fd);

    for(i=0;i!=id_head;++i){
        free(id_array[id_head-i-1].ptr);
    }
    free(id_array);
    return 0;
}
