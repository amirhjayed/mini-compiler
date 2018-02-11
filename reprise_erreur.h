#ifndef REPRISE_ERREUR_H
#define REPRISE_ERREUR_H

#include "structs.h"
#include "globals.h"
#include "anal_lex.h"

int reprise_erreur(int ul,int s_ul,int nt){
    if(eof_flag!=1){
        if(s_ul!=LEX_ERROR){
            printf("SYNTAX ERROR (%d, %d): Expected '%s' but found '%s'.\n",f_line,f_column,ul_words[ul],ul_words[s_ul]);
        }
        else{
            printf("LEXICAL ERROR (%d, %d).\n",f_line,f_column);
        }
    }
    while (symbole.ul != EOF){
        switch(nt){
        case P:
            if(symbole.ul == PROGRAM){
                symbole=anal_lex();
                return PROGRAM;
            }
            break;
        case DCL:
            if(symbole.ul == BEGIN){
                return BEGIN;
            }
            break;
        case LISTE_ID:
            if(symbole.ul == DP){
                return DP;
            }
            break;
        case LISTE_IDP:
            if(symbole.ul == DP){
                return DP;
            }
            break;
        case LIST_INSTP:
            if(symbole.ul == END){
                return END;
            }
            if(symbole.ul == ID){
                return ID;
            }
            if(symbole.ul == IF){
                return IF;
            }
            if(symbole.ul == WHILE){
                return WHILE;
            }
            if(symbole.ul == READ){
                return READ;
            }
            if(symbole.ul == READLN){
                return READLN;
            }
            if(symbole.ul == WRITE){
                return WRITE;
            }
            if(symbole.ul == WRITELN){
                return WRITELN;
            }
            break;
        case TYPE:
            if(symbole.ul == PV){
                return PV;
            }
            break;
        case INST_COMP:
            if(deb == 1){
                if(symbole.ul == BEGIN){
                    symbole=anal_lex();
                    return BEGIN;
                }
            }
            else{
                if(symbole.ul == PT){
                    return PT;
                }
            }
            break;
        case I:
            if(deb==1){
                if(symbole.ul == ELSE){
                    return ELSE;
                }
            }
            else{
                if(symbole.ul == PV){
                    return PV;
                }
                else if(symbole.ul == END){
                    return END;
                }
            }
            break;
        case EXPRP:
            if(symbole.ul == THEN){
                return THEN;
            }
            if(symbole.ul == DO){
                return DO;
            }
            if(symbole.ul == PF){
                return PF;
            }
            break;
        case EXPR_SIMPLEP:
            if(symbole.ul == OPREL){
                return OPREL;
            }
            if(symbole.ul == THEN){
                return THEN;
            }
            if(symbole.ul == DO){
                return DO;
            }
            if(symbole.ul == PF){
                return PF;
            }
            break;
        case TERMEP:
            if(symbole.ul == OPADD){
                return OPADD;
            }
            if(symbole.ul == OPREL){
                return OPREL;
            }
            if(symbole.ul == THEN){
                return THEN;
            }
            if(symbole.ul == DO){
                return DO;
            }
            if(symbole.ul == PF){
                return PF;
            }
            break;
        case FACTEUR:
            if(symbole.ul == OPMUL){
                return OPMUL;
            }
            else if(symbole.ul == OPADD){
                return OPADD;
            }
            if(symbole.ul == OPREL){
                return OPREL;
            }
            if(symbole.ul == THEN){
                return THEN;
            }
            if(symbole.ul == DO){
                return DO;
            }
            if(symbole.ul == PF){
                return PF;
            }
            else if(symbole.ul == ELSE){
                return ELSE;
            }
            break;
        }
        symbole=anal_lex();
    }
    return EOF;
}
#endif // REPRISE_ERREUR_H
