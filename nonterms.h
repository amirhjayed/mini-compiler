#ifndef NONTERMS_H
#define NONTERMS_H
// Procédurs non terminaux
/* Missing error treatments */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "nonterms.h"
#include "anal_lex.h"
#include "structs.h"
#include "globals.h"

int facteur();
int termep();
int terme();
int expr_simplep();
int expr_simple();
int exprp();
int expr();
void i();
void inst_comp();
int type();
void list_instp();
void list_inst();
void liste_id();
void liste_idp();
void dcl();
void p();

const char * const oprel_2str[]={"==","<>","<=",">=",">","<"};
const char * const opadd_2str[]={"+","-","||"};
const char * const opmul_2str[]={"*","/","%","&&"};

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

// Analyse synatxique

int creer_etiq(){
    return etiq_ctr++;
}
void syntax_error(int ul,int s_ul){
    if(s_ul!=-2){
        printf("SYNTAX ERROR (%d, %d): Expected '%s' but found '%s'.\n",f_line,f_column,ul_words[ul],ul_words[s_ul]);
    }
    else{
        printf("LEXICAL ERROR (%d, %d).\n",f_line,f_column);
    }
}
int accepter(int ul){
    int ret_att;
    if(ul == symbole.ul){
        if(symbole.ul == ID){
            ret_att=symbole.att;
            symbole = anal_lex();
            if((new_id_flag)&&(!dcl_flag)){ // if misplaced declaration
                traduction_flag = 0;
                printf("WARNING (%d, %d): misplaced declaration of %s\n",f_line,f_column,id_array[id_head-1].id);
            }
            return ret_att;
        }
        symbole = anal_lex();
    }
    else{
        traduction_flag = 0;
        syntax_error(ul,symbole.ul);
    }
    return -1;
}
int facteur(){
    int type=VOID_TYPE;
    int ind_array;
    if(symbole.ul == ID){
        ind_array=accepter(ID);
        type=id_array[ind_array].type;
        fprintf(fd,"valeurd %p\n",id_array[ind_array].ptr);
    }
    else if(symbole.ul == NB){
        accepter(NB);
        type=INTEGER;
    }
    else if(symbole.ul == PO){
        accepter(PO);
        type=expr();
        accepter(PF);
    }
    else{
        traduction_flag=0;
        printf("SYNTAX ERROR (%d, %d): facteur manquant \n",f_line, f_column);
    }
    return type;
}

int termep(){
    int t1,t2;
    int att ;
    if (symbole.ul == OPMUL){
        att=symbole.att;
        accepter(OPMUL);
        t1=facteur();
        t2=termep();
        fprintf(fd,"%s\n",opmul_2str[att]);
        if(t2 != VOID_TYPE){
            if(t1==t2){
                return t1;
            }
            else{
                return TYPE_ERROR;
            }
        }
        else{
            return t1;
        }
    }
    else{
        return VOID_TYPE;
        //epsilon
    }
}

int terme(){
    int t1,t2;
    t1=facteur();
    t2=termep();
    if(t2 != VOID_TYPE){
        if(t1==t2){
            return t1;
        }
        else{
            return TYPE_ERROR;
        }
    }
    else{
        return t1;
    }
}

int expr_simplep(){
    int att;
    if(symbole.ul == OPADD){
        int t1,t2;
        att=symbole.att;
        accepter(OPADD);
        t1=terme();
        t2=expr_simplep();
        fprintf(fd,"%s\n",opadd_2str[att]);
        if(t2 != VOID_TYPE){
            if(t1==t2){
                return t1;
            }
            else{
                return TYPE_ERROR;
            }
        }
        else{
            return t1;
        }
    }
    else{
        return VOID_TYPE;
        //epsilon
    }
}
int expr_simple(){
    int t1,t2;
    t1=terme();
    t2=expr_simplep();
    if(t2 != VOID_TYPE){
        if(t1==t2){
            return t1;
        }
        else{
            return TYPE_ERROR;
        }
    }
    else{
        return t1;
    }
}

int exprp(){
    int att;
    int type;
    if(symbole.ul == OPREL){
        att=symbole.att;
        accepter(OPREL);
        type=expr_simple();
        fprintf(fd,"%s\n",oprel_2str[att]);
        return type;
    }
    else{
        return VOID_TYPE;
        //epsilon
    }
}

int expr(){
    int t1,t2;
    t1=expr_simple();
    t2=exprp();
    if(t2 != VOID_TYPE){
        if(t1==t2){
            return t1;
        }
        else{
            return TYPE_ERROR;
        }
    }
    else{
        return t1;
    }
}

void i(){
    int ind_array;
    int t1,t2;
    if(symbole.ul == ID){
       ind_array=accepter(ID);
       fprintf(fd,"valeurg %p\n",id_array[ind_array].ptr);
       accepter(OPAFF);
       t1=expr_simple();
       t2=id_array[ind_array].type;
       if(t1!=t2){
            traduction_flag = 0;
            printf("WARNING (%d, %d): type mismatch %s and expr_simple()\n",f_line,f_column,id_array[ind_array].id);
       }
       fprintf(fd,":=\n");
    }
    else if(symbole.ul == IF){
        int sortie;
        sortie=creer_etiq();
        accepter(IF);
        expr();
        fprintf(fd,"allersifaux %d \n",sortie);
        accepter(THEN);
        i();
        accepter(ELSE);
        fprintf(fd,"etiq %d : \n",sortie);
        i();
    }
    else if(symbole.ul == WHILE){
        int test,sortie;
        test=creer_etiq();
        sortie=creer_etiq();
        accepter(WHILE);
        fprintf(fd,"etiq %d\n",test);
        expr();
        fprintf(fd,"allersifaux %d\n",sortie);
        accepter(DO);
        i();
        fprintf(fd,"allerà %d\n",test);
        fprintf(fd,"etiq %d\n",sortie);
    }
    else if(symbole.ul == READ){
        accepter(READ);
        accepter(PO);
        ind_array=accepter(ID);
        fprintf(fd,"read valeurg %p\n",id_array[ind_array].ptr);
        accepter(PF);
    }
    else if(symbole.ul == READLN){
        accepter(READLN);
        accepter(PO);
        ind_array=accepter(ID);
        fprintf(fd,"readln valeurg %p\n",id_array[ind_array].ptr);
        accepter(PF);
    }
    else if(symbole.ul == WRITE){
        accepter(WRITE);
        accepter(PO);
        ind_array=accepter(ID);
        fprintf(fd,"write valeurd %p\n",id_array[ind_array].ptr);
        accepter(PF);
    }
    else if(symbole.ul == WRITELN){
        accepter(WRITELN);
        accepter(PO);
        ind_array=accepter(ID);
        fprintf(fd,"writeln valeurd %p\n",id_array[ind_array].ptr);
        accepter(PF);
    }
}

void inst_comp(){
    accepter(BEGIN);
    list_inst();
    accepter(END);
}

int type(){
    if (symbole.ul == INTEGER){
        accepter(INTEGER);
        return INTEGER;
    }
    else{
        accepter(CHAR);
        return CHAR;
    }
}

void list_instp(){
    if(symbole.ul == PV){
        accepter(PV);
        i();
        list_instp();
    }
    else{
        // epsilon
    }
}

void list_inst(){
    i();
    list_instp();
}

void liste_id(){
    l_sz=0;
    accepter(ID);
    liste_idp();
}

void liste_idp(){
    if(symbole.ul == VG){
        ++l_sz;
        accepter(VG);
        accepter(ID);
        liste_idp();
    }
    else{
        ++l_sz;
    }
}


void dcl(){
    dcl_flag = 1;
    int t;
    int i;
    if(symbole.ul == VAR){
        accepter(VAR);
        liste_id();
        accepter(DP);
        t=type();
        for(i=0;i!=l_sz;++i){
            id_array[id_head-i-1].type=t;
            if(t==INTEGER){
                id_array[id_head-i-1].ptr=malloc(sizeof(int));
            }
            else if(t==CHAR){
                id_array[id_head-i-1].ptr=malloc(sizeof(char));
            }
        }
        accepter(PV);
        dcl();
    }
    else{
        dcl_flag = 0;
        // epsilon
    }
}

void p(){
    dcl_flag=1;
    symbole = anal_lex();
    accepter(PROGRAM);
    accepter(ID);
    dcl_flag=0;
    accepter(PV);
    dcl();
    inst_comp();
    accepter(PT);
}

#endif // NONTERMS_H
