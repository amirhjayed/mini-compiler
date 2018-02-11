TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

HEADERS += \
    nonterms.h \
    anal_lex.h \
    globals.h \
    structs.h \
    reprise_erreur.h
