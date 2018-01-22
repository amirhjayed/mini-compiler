#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "nonterms.h"
#include "anal_lex.h"
#include "structs.h"
#include "globals.h"

const char * const oprel_2str[]={"==","<>","<=",">=",">","<"};
const char * const opadd_2str[]={"+","-","||"};
const char * const opmul_2str[]={"*","/","%","&&"};


/* Globals */
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

// Analyse synatxique

int creer_etiq(){
    return etiq_ctr++;
}

int accepter(int ul){
    int ret_att;
    if(ul == symbole.ul){
        printf("accepted : %d \n",ul);
        if(symbole.ul == ID){
            ret_att=symbole.att;
            symbole = anal_lex();
            if((new_id_flag)&&(!dcl_flag)){ // if misplaced declaration
                printf("Warning : misplaced declaration of %s\n",id_array[id_head-1].id);
            }
            return ret_att;
        }
        symbole = anal_lex();
    }
    else{
        printf("erreur : expected %d but found %d\n",ul,symbole.ul);
    }
    return -1;
}
int facteur(){
    printf("in facteur() \n");
    int type=VOID_TYPE;
    int ind_array;
    if(symbole.ul == ID){
        ind_array=accepter(ID);
        type=id_array[ind_array].type;
        fprintf(fd,"valeurd %p\n",id_array[ind_array].ptr);
    }
    if(symbole.ul == NB){
        fprintf(fd,"empiler %d\n",symbole.att);
        accepter(NB);
        type=INTEGER;
    }
    if(symbole.ul == PO){
        accepter(PO);
        type=expr_simple();
        accepter(PF);
    }
    return type;
}

int termep(){
    printf("in termep() \n");
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
    printf("in terme() \n");
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
    printf("in expr_simplep() \n");
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
    printf("in expr_simple() \n");
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
    printf("in exprp() \n");
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
    printf("in expr() \n");
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
    printf("in i() \n");
    if(symbole.ul == ID){
       ind_array=accepter(ID);
       fprintf(fd,"valeurg %p\n",id_array[ind_array].ptr);
       accepter(OPAFF);
       t1=expr_simple();
       // typechecking
       t2=id_array[ind_array].type;
       if(t1!=t2){
            printf("WARNING: type mismatch %s and expression\n",id_array[ind_array].id);
       }
       //
       fprintf(fd,":=\n");
    }

    // No type checking needed
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
    printf("in inst_comp() \n");
    accepter(BEGIN);
    list_inst();
    accepter(END);
}

int type(){
    printf("in type() \n");
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
    printf("in liste_instp() \n");
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
    printf("in liste_inst() \n");
    i();
    list_instp();
}

int liste_id(){
    printf("in liste_id() \n");
    l_sz=0;
    accepter(ID);
    return(liste_idp());
}

int liste_idp(){
    printf("in liste_idp() \n");
    if(symbole.ul == VG){
        ++l_sz;
        accepter(VG);
        accepter(ID);
        liste_idp();
    }
    else{
        return(++l_sz);
        // epsilon
    }
}


void dcl(){
    printf("in dcl() \n");
    dcl_flag = 1;
    int t;
    int i;
    int l_sz=0;
    if(symbole.ul == VAR){
        accepter(VAR);
        l_sz=liste_id();
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
    printf("in p() \n");
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

int main(int argc, char** argv){
    etiq_ctr=0;
    etat=0;
    int i = 0 ;
    if(argc != 3){
        printf("arguments manquants\n");
        return -1;
    }
    fs = fopen(argv[1], "r");
    fd = fopen(argv[2], "w+");
    id_size = 5 ;
    id_head = 0 ;
    id_array = malloc(id_size * sizeof(idef_t));

    if(fs){ //
        p();
    }
    else {
        printf("file openned incorrectly\n");
    }

    fclose(fs);

    for(i=0;i!=id_head;++i){
        free(id_array[id_head-i-1].ptr);
    }
    free(id_array);
    //
    return 0;
}
