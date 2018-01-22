
/*Regles sémantique implémenté */
// declaration
// type checking
// k


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "nonterms.h"


#define MAX_ID_LENGTH 32
#define LEX_ERROR -2
#define TYPE_ERROR -3
#define VOID_TYPE -4

// Definition et code de UL
// keywords
#define PROGRAM 0
#define VAR 1
#define INTEGER 2
#define CHAR 3
#define BEGIN 4
#define END 5
#define IF 6
#define THEN 7
#define ELSE 8
#define WHILE 9
#define DO 10
#define READ 11
#define READLN 12
#define WRITE 13
#define WRITELN 14

#define ID 15
#define PV 16
#define PT 17
#define DP 18
#define VG 19
#define OPAFF 20
#define OPMUL 21
#define OPREL 22
#define OPADD 23
#define NB 24
#define PO 25
#define PF 26

// Table de mots clés
const char * const keywords[] = {"program","var","integer","char","begin","end","if","then",
                  "else","while","do","read","readln","write","writeln"}; /*la position est le code d'UL*/

typedef struct symb_t {
    int ul;
    int att;
}symb_t;

typedef struct idef_t{
    char id[MAX_ID_LENGTH];
    int type;
    void *ptr;
}idef_t;


    /*globals*/

    FILE *fs, *fd;
    symb_t symbole;
    idef_t *id_array;
    int id_size;
    int id_head;
    int etat = 0 ;
    int dcl_flag; // utilisé par analyse sémantique pour determiner si declaration mal placée
    int new_id_flag;
    int l_sz; //used by liste_id() to track the number of declared variables.
    //


// Attributes
    enum oprel {egal,diff,inf_egale,sup_egale,sup_strict,inf_strict};
    enum opadd {sum,subs,ou};
    enum opmul {mul,division,mod,et};

    const char * const oprel_2str[]={"==","<>","<=",">=",">","<"};
    const char * const opadd_2str[]={"+","-","||"};
    const char * const opmul_2str[]={"*","/","%","&&"};

// Analyse lexicale

// unilex_id()

int unilex_id(const char *ch){
    int i = 0;
    while((i != 15) && (strcmp(ch,keywords[i]) != 0)){
        ++i;
    }
    return i; // if not in keywords than i==15 (code of ID)
}

// ranger_id() : tested +1

int ranger_id(int code_ul, const char *ch){
    int i = 0;
    new_id_flag = 0 ;
    if (code_ul != ID){return -1;} // ch is keyword
    else{
        while((i < id_head) && (strcmp(ch,id_array[i].id) != 0)){
            ++i;
        }
        if(i==id_head){ // le id ne figure pas dans id_array
            new_id_flag = 1 ;
            if(++id_head>id_size){
                id_size*=2;
                id_array = realloc(id_array, id_size * sizeof(idef_t));
            }
            strcpy(id_array[i].id, ch);
        }
        return i;
    }
}

// debug();
void debug_analex(int etat){
    printf("erreur dans etat = %d \n",etat);
}
// Automate à état fini
symb_t anal_lex(){
    etat = 0 ;
    char c = '\0';
    char ch[MAX_ID_LENGTH]; // the exceeding characters will be ignored by the lexer

    symb_t result;
    int n = 0; //points to head of buffer string
    while(1){
        switch(etat){
            case 0 :
                c = getc(fs);
                if((c == ' ') || (c == '\n') || (c =='\t'));
                else if(isalpha(c)){
                    etat = 1;
                    ch[n++] = c;
                }
                else if(isdigit(c)){
                    etat = 3;
                    ch[n++] = c;
                }
                else if( c == '('){etat = 5; }
                else if( c == '='){etat = 10;}
                else if( c == '<'){etat = 13;}
                else if( c == '>'){etat = 17;}
                else if( c == '+'){etat = 20;}
                else if( c == '-'){etat = 21;}
                else if( c == '|'){etat = 22;}
                else if( c == '*'){etat = 25;}
                else if( c == '/'){etat = 26;}
                else if( c == '%'){etat = 28;}
                else if( c == ';'){etat = 31;}
                else if( c == '.'){etat = 32;}
                else if( c == ','){etat = 33;}
                else if( c == ')'){etat = 34;}
                else if( c == ':'){etat = 35;}
                else if( c == EOF ){
                    etat = 38;
                }
                else{ // autre
                    /* Missing error treatement */
                    debug_analex(etat);
                    etat = 9;
                }
                break;

            case 1 :
                c = getc(fs);
                if(isalnum(c)){
                    if(n < MAX_ID_LENGTH)
                        ch[n++]=c;
                }
                else{
                    etat = 2;
                }
                break;
            case 2 :
                ungetc(c,fs);
                ch[n]='\0';
                result.ul  = unilex_id(ch);
                result.att = ranger_id(result.ul, ch);
                return result;

            case 3 :
                c = getc(fs);
                if(isdigit(c)){
                    if(n < MAX_ID_LENGTH)
                        ch[n++]=c;
                }
                else{
                    etat = 4;
                }
                break;
            case 4 :
                ungetc(c,fs);
                ch[n]='\0';
                result.ul = NB;
                result.att = atoi(ch);
                return result;
            case 5 :
                c = getc(fs);
                if( c == '*'){
                    etat = 6;
                }
                else{
                    etat = 8 ;
                }
                break;
            case 6 :
                c = getc(fs);
                if( c == '*'){
                    etat = 7 ;
                }
                break;
            case 7 :
                c = getc(fs);
                if( c == ')'){
                    etat = 0;
                }
                else{
                    etat = 6;
                }
                break;
            case 8 :
                ungetc(c,fs);
                result.ul = PO ;
                return result;
            case 9 :
                debug_analex(etat);
                result.ul = -1;
                return result ;
                /* Missing error treatement*/
                break;

            // OPREL
            case 10 :
                c = getc(fs);
                if(c == '='){
                    etat = 11 ;
                }
                else{
                    etat = 12 ;
                }
                break;
            case 11 :
                result.ul = OPREL;
                result.att=egal;
                return result;
            case 12 :
                debug_analex(etat);
                result.ul = LEX_ERROR ;
                return result;
                /* traitment d'erreur */
            case 13 :
                c = getc(fs);
                if (c == '>'){
                    etat = 15;
                }
                else if(c == '='){
                    etat = 16;
                }
                else {
                    etat = 14;
                }
                break;
            case 14 :
                ungetc(c,fs);
                result.ul = OPREL;
                result.att= inf_strict;
                return result;
            case 15 :
                result.ul =OPREL;
                result.att=diff;
                return result;
            case 16 :
                result.ul =OPREL;
                result.att=inf_egale;
                return result;
            case 17 :
                c = getc(fs);
                if (c == '='){
                    etat = 18;
                }
                else{
                    etat = 19;
                }
                break;
            case 18 :
                result.ul = OPREL;
                result.att=sup_egale;
                return result;
            case 19 :
                ungetc(c,fs);
                result.ul = OPREL;
                result.att=sup_strict;
                return result;

            // OPADD
            case 20 :
                result.ul = OPADD;
                result.att=sum;
                return result;
            case 21 :
                result.ul = OPADD;
                result.att=subs;
                return result;
            case 22 :
                c = getc(fs);
                if( c == '|'){
                    etat = 23;
                }
                else{
                    etat = 24;
                }
                break;
            case 23 :
                result.ul = OPADD;
                result.att=ou;
                return result;

            case 24 :
                /* error treatement */
                result.ul = LEX_ERROR ;
                debug_analex(etat);
                return result;
            // OPMUL
            case 25 :
                result.ul = OPMUL;
                result.att=mul;
                return result ;
            case 26 :
                result.ul = OPMUL;
                result.att=division;
                return result ;
            case 27 :
                result.ul = OPMUL;
                result.att=mod;
                return result ;
            case 28 :
                c = getc(fs);
                if( c == '%'){
                    etat = 29;
                }
                else{
                    etat = 30;
                }
                break;
            case 29 :
                result.ul = OPMUL;
                result.att=et;
                return result ;
            case 30 :
                /* error treatement */
                result.ul = LEX_ERROR ;
                debug_analex(etat);
                return result;

            // PONCT
            case 31 :
                result.ul = PV;
                return result ;
            case 32 :
                result.ul = PT;
                return result ;
            case 33 :
                result.ul = VG;
                return result ;
            case 34 :
                result.ul = PF;
                return result ;
            case 35 :
                c = getc(fs);
                if( c == '='){
                    etat = 37;
                }
                else{
                    etat = 36;
                }
            break;
            case 36 :
                ungetc(c,fs);
                result.ul = DP;
                return result ;
            case 37 :
                result.ul = OPAFF;
                return result ;

            // EOF
            case 38 :
                result.ul = EOF ;
                return result;
        }
    }
}
// fin analex

// Analyse synatxique
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

// Procédurs non terminaux
/* Missing error treatments */
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
    printf("in exprp() \n");
    if(symbole.ul == OPREL){
        att=symbole.att;
        accepter(OPREL);
        return expr_simple();
    }
    else{
        return VOID_TYPE;
        fprintf(fd,"%s",oprel_2str[att]);
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
        accepter(IF);
        expr();
        accepter(THEN);
        fprintf(fd,"%s\n","allersifaux\n");
        i();
        accepter(ELSE);
        fprintf(fd,"%s\n","etiq sortie\n");
        i();
    }
    else if(symbole.ul == WHILE){
        accepter(WHILE);
        expr();
        accepter(DO);
        i();
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

//    /** Content of id_array */

    for(i=0;i<id_head;++i){
        printf("id_array[%d].id = %s\n", i, id_array[i].id);
        printf("id_array[%d].type= %d\n",i,id_array[i].type);
    }

//    symbole=anal_lex();
//    while(symbole.ul != EOF){
//        printf("UL == %d\n",symbole.ul);
//        symbole=anal_lex();
//    }

    fclose(fs);

    for(i=0;i!=id_head;++i){
        free(id_array[id_head-i-1].ptr);
    }
    free(id_array);
    //
    return 0;
}
