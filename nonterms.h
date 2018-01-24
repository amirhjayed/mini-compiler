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
#include "reprise_erreur.h"

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
extern int deb;
extern int eof_flag;

extern int f_line;
extern int f_column;

// Analyse synatxique

int creer_etiq(){
    return etiq_ctr++;
}


int accepter(int ul,int nt){
    int ret_att = NO_ATT;
    int rep_value;
    if(ul == symbole.ul){
        if(symbole.ul == ID){
            ret_att=symbole.att;
            if((new_id_flag)&&(!dcl_flag)){
                traduction_flag = 0;
                printf("WARNING (%d, %d): '%s' undeclared.\n",f_line,f_column,id_array[id_head-1].id);
            }
            if((!new_id_flag)&&(dcl_flag)){
                traduction_flag = 0;
                printf("WARNING (%d, %d): '%s' redeclared.\n",f_line,f_column,id_array[id_head-1].id);
            }
        }
        else if(symbole.ul == NB){
            ret_att=symbole.att;
        }
        symbole = anal_lex();
        return ret_att;
    }
    else{
        traduction_flag = 0;
        rep_value=reprise_erreur(ul,symbole.ul,nt);
        if(rep_value != EOF){
            printf("Analyse continue apres trouve: '%s'.\n",ul_words[rep_value]);
        }
        else{
            if(eof_flag!=1){
                eof_flag=1;
                printf("ERROR: unexpected EOF\n");
            }
        }
        return SYNTAX_ERROR;
    }
}

int facteur(){
    int type=VOID_TYPE;
    int att;
    if(symbole.ul == ID){
        att=accepter(ID,FACTEUR);
        type=id_array[att].type;
        fprintf(fd,"valeurd %p\n",id_array[att].ptr);
    }
    else if(symbole.ul == NB){
        att=accepter(NB,FACTEUR);
        fprintf(fd,"empiler %d\n",att);
        type=INTEGER;
    }
    else if(symbole.ul == PO){
        symbole=anal_lex();
        type=expr();
        accepter(PF,FACTEUR);
    }
    else{
        traduction_flag=0;
        printf("SYNTAX ERROR (%d, %d): facteur manquant \n",f_line, f_column);
        while ((symbole.ul != EOF)){
            if(symbole.ul == OPMUL){
                break;
            }
            else if(symbole.ul == OPADD){
                break;
            }
            else if(symbole.ul == OPREL){
                break;
            }
            else if(symbole.ul == THEN){
                break;
            }
            else if(symbole.ul == ELSE){
                break;
            }
            else if(symbole.ul == DO){
                break;
            }
            else if(symbole.ul == PF){
                break;
            }
            else
                symbole = anal_lex();
        }
        if(symbole.ul!=EOF)
            printf("Analyse continue apres trouve '%s'.\n",ul_words[symbole.ul]);
    }
    return type;
}

int termep(){
    int t1,t2;
    int att ;
    if (symbole.ul == OPMUL){
        att=symbole.att;
        symbole=anal_lex();
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
        symbole = anal_lex();
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
        symbole=anal_lex();
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
       ind_array=accepter(ID,I);
       fprintf(fd,"valeurg %p\n",id_array[ind_array].ptr);
       accepter(OPAFF,I);
       t1=expr_simple();
       t2=id_array[ind_array].type;
       if(t1!=t2){
            traduction_flag = 0;
            printf("WARNING (%d, %d): type mismatch %s and expr_simple()\n",f_line,f_column,id_array[ind_array].id);
       }
       fprintf(fd,":=\n");
    }
    else if(symbole.ul == IF){
        int sortie,sinon;
        sinon=creer_etiq();
        sortie=creer_etiq();
        accepter(IF,I);
        if(expr()==TYPE_ERROR){
            traduction_flag=0;
            printf("WARNING (%d, %d): type mismatch in expr\n",f_line,f_column);
        }
        fprintf(fd,"allersifaux %d \n",sinon);
        accepter(THEN,I);
        deb=1;
        i();
        deb=0;
        fprintf(fd,"allerà %d \n",sortie);
        accepter(ELSE,I);
        fprintf(fd,"etiq %d : \n",sinon);
        i();
        fprintf(fd,"etiq %d : \n",sortie);
    }
    else if(symbole.ul == WHILE){
        int test,sortie;
        test=creer_etiq();
        sortie=creer_etiq();
        symbole=anal_lex();
        fprintf(fd,"etiq %d\n",test);
        if(expr()==TYPE_ERROR){
            traduction_flag=0;
            printf("WARNING (%d, %d): type mismatch in expr\n",f_line,f_column);
        }
        fprintf(fd,"allersifaux %d\n",sortie);
        accepter(DO,I);
        i();
        fprintf(fd,"allerà %d\n",test);
        fprintf(fd,"etiq %d\n",sortie);
    }
    else if(symbole.ul == READ){
        symbole=anal_lex();
        if(accepter(PO,I)==SYNTAX_ERROR){return;}
        ind_array=accepter(ID,I);
        fprintf(fd,"read valeurg %p\n",id_array[ind_array].ptr);
        if(accepter(PF,I)==SYNTAX_ERROR){return;};
    }
    else if(symbole.ul == READLN){
        symbole = anal_lex();
        if(accepter(PO,I)==SYNTAX_ERROR){return;};
        ind_array=accepter(ID,I);
        fprintf(fd,"readln valeurg %p\n",id_array[ind_array].ptr);
        if(accepter(PF,I)==SYNTAX_ERROR){return;};
    }
    else if(symbole.ul == WRITE){
        symbole = anal_lex();
        if(accepter(PO,I)==SYNTAX_ERROR){return;};
        ind_array=accepter(ID,I);
        fprintf(fd,"write valeurd %p\n",id_array[ind_array].ptr);
        if(accepter(PF,I)==SYNTAX_ERROR){return;};
    }
    else if(symbole.ul == WRITELN){
        symbole = anal_lex();
        if(accepter(PO,I)==SYNTAX_ERROR){return;};
        ind_array=accepter(ID,I);
        fprintf(fd,"writeln valeurd %p\n",id_array[ind_array].ptr);
        if(accepter(PF,I)==SYNTAX_ERROR){return;};
    }
}

void inst_comp(){
    deb=1;
    accepter(BEGIN,INST_COMP);
    deb=0;
    list_inst();
    accepter(END,INST_COMP);
}

int type(){
    if (symbole.ul == INTEGER){
        accepter(INTEGER,TYPE);
        return INTEGER;
    }
    else{
        accepter(CHAR,TYPE);
        return CHAR;
    }
}

void list_instp(){
    if(symbole.ul == PV){
        symbole = anal_lex();
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
    accepter(ID,LISTE_ID);
    liste_idp();
}

void liste_idp(){
    if(symbole.ul == VG){
        ++l_sz;
        symbole = anal_lex();
        accepter(ID,LISTE_IDP);
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
        symbole=anal_lex();
        liste_id();
        if(accepter(DP,DCL)==SYNTAX_ERROR){dcl_flag=0;return;}
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
        accepter(PV,DCL);
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
    deb=1;
    accepter(PROGRAM,P);
    deb=0;
    accepter(ID,P);
    dcl_flag=0;
    accepter(PV,P);
    dcl();
    inst_comp();
    accepter(PT,P);
}

#endif // NONTERMS_H
