#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_ID_LENGTH 32
#define LEX_ERROR -2
#define TYPE_ERROR -3
#define VOID_TYPE -4

const char * const ul_words[] = {"program","var","integer","char","begin","end","if","then",
                  "else","while","do","read","readln","write","writeln","ID", "PV", "PT", "DP",
                    "VG", "OPAFF","OPMUL", "OPREL", "OPADD", "NB", "PO", "PF"};


                  /*la position est le code d'UL*/

enum oprel {egal,diff,inf_egale,sup_egale,sup_strict,inf_strict};
enum opadd {sum,subs,ou};
enum opmul {mul,division,mod,et};

typedef struct symb_t {
    int ul;
    int att;
}symb_t;

typedef struct idef_t{
    char id[MAX_ID_LENGTH];
    int type;
    void *ptr;
}idef_t;


#endif // STRUCTS_H
