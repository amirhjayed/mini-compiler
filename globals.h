#ifndef GLOBALS_H
#define GLOBALS_H
#include "structs.h"
#include <stdio.h>

FILE *fs, *fd;
symb_t symbole;
idef_t *id_array;
int id_size;
int id_head;
int etat ;
int dcl_flag; // utilisé par analyse sémantique pour determiner si declaration mal placée
int new_id_flag;
int deb = 0;
int l_sz; //used by liste_id() to track the number of declared variables.
int etiq_ctr; // label counter
int f_line;
int f_column;
int eof_flag;

#endif // GLOBALS_H
