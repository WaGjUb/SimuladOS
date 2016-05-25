#include "eventos.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

evento_t* EVENTO_criar(char* e){
    evento_t* novo;
    char* tok;
    
    novo = malloc(sizeof(evento_t));
    
    tok = strtok(e, " ");
    
    novo->tempo = atoi(tok);
    
    tok = strtok(NULL, " \n");
    
    if(!strcmp(tok, "BLOQUEIO"))
        novo->evento = EVT_BLOQUEIO;
    
    if(!strcmp(tok, "DESBLOQUEIO"))
        novo->evento = EVT_DESBLOQUEIO;
    
    if(!strcmp(tok, "TERMINO"))
        novo->evento = EVT_TERMINO;
    
    return novo;
}

void EVENTO_imprimir(evento_t* e){
    const char* evt;
    
    if(e->evento == EVT_BLOQUEIO)
        evt = "BLOQUEIO";
    
    if(e->evento == EVT_DESBLOQUEIO)
        evt = "DESBLOQUEIO";
    
    if(e->evento == EVT_TERMINO)
        evt = "TERMINO";
    
    printf("%d %s\n", e->tempo, evt);
        
}
