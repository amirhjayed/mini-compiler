#ifndef ANAL_LEX_H
#define ANAL_LEX_H

#include "structs.h"
#include "globals.h"

extern int traduction_flag;
extern int f_line;
extern int f_column;

enum uni_lex {PROGRAM, VAR, INTEGER, CHAR, BEGIN, END, IF, THEN, ELSE, WHILE, DO,
             READ, READLN, WRITE, WRITELN, ID, PV, PT, DP, VG, OPAFF,
             OPMUL, OPREL, OPADD, NB, PO, PF, EPSILON};

int unilex_id(const char *ch){
    int i = 0;
    while((i != 15) && (strcmp(ch,ul_words[i]) != 0)){
        ++i;
    }
    return i; // if not in ul_words than i==15 (code of ID)
}

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
                f_column++ ;
                if(c == ' ');
                else if(c == '\t'){
                    f_column+=7; // \t is 8 columns
                }
                else if(c == '\n'){
                    f_line++;
                    f_column=0;
                }
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
                    etat = 9;
                }
                break;

            case 1 :
                c = getc(fs);
                f_column++;
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
                f_column--;
                ch[n]='\0';
                result.ul  = unilex_id(ch);
                result.att = ranger_id(result.ul, ch);
                return result;

            case 3 :
                c = getc(fs);
                f_column++;
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
                f_column--;
                ch[n]='\0';
                result.ul = NB;
                result.att = atoi(ch);
                    return result;
            case 5 :
                c = getc(fs);
                f_column++;
                if( c == '*'){
                    etat = 6;
                }
                else{
                    etat = 8 ;
                }
                break;
            case 6 :
                c = getc(fs);
                f_column++;
                if( c == '*'){
                    etat = 7 ;
                }
                break;
            case 7 :
                c = getc(fs);
                f_column++;
                if( c == ')'){
                    etat = 0;
                }
                else{
                    etat = 6;
                }
                break;
            case 8 :
                ungetc(c,fs);
                f_column--;
                result.ul = PO ;
                return result;
            case 9 :
                result.ul = LEX_ERROR;
                return result ;
                /* Missing error treatement */
                break;

            // OPREL
            case 10 :
                c = getc(fs);
                f_column++;
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
                result.ul = LEX_ERROR ;
                return result;
                /* traitment d'erreur */
            case 13 :
                c = getc(fs);
                f_column++;
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
                f_column--;
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
                f_column++;
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
                f_column--;
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
                f_column++;
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
                f_column++;
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
                f_column++;
                if( c == '='){
                    etat = 37;
                }
                else{
                    etat = 36;
                }
            break;
            case 36 :
                ungetc(c,fs);
                f_column--;
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

#endif // ANAL_LEX_H
