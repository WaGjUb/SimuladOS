#include <stdio.h>
#include <stdlib.h>
#include "bcp.h"
#include "eventos.h"

void BCP_adicionarEvento(bcp_t* bcp, evento_t* e){
    if(bcp->nEventos >= bcp->alocEventos){
        bcp->alocEventos += EVENTOS_ALOC_INC;
        bcp->eventos = realloc(bcp->eventos,bcp->alocEventos * sizeof(evento_t*));
    }
    
    bcp->eventos[bcp->nEventos] = e;
    
    bcp->nEventos++;
}

evento_t* BCP_proxEvento(bcp_t* bcp){
    evento_t* e;
    e = bcp->eventos[bcp->proxEvento];
    
    bcp->proxEvento++;
    
    return e;
}

bcp_t* BCP_criar(FILE* arqProcesso){
    bcp_t* novo;
    char s[255];
    int i;
    int eventos;
    
    novo = malloc(sizeof(bcp_t));
    
    novo->nEventos = 0;
    novo->eventos = malloc(sizeof(evento_t*) * EVENTOS_ALOC_INC);
    novo->alocEventos = EVENTOS_ALOC_INC;
    
    fgets(s, 255, arqProcesso);    
    novo->pid = atoi(s);

    fgets(s, 255, arqProcesso);    
    novo->prioridade = atoi(s);
    
    fgets(s, 255, arqProcesso);    
    eventos = atoi(s);
    
    fgets(s, 255, arqProcesso);    
    novo->entrada = atol(s);
    
    for(i = 0; i < eventos-1; i++){
        fgets(s, 255, arqProcesso);
        BCP_adicionarEvento(novo, EVENTO_criar(s));
    }
    
    novo->proxEvento = 0;
    novo->tempoExecutado = 0;
    novo->tPrimeiraExec = -1;
    novo->tUltimaExec = -1;
    
    return novo;
}

void BCP_destruir(bcp_t* bcp){
    int i;
    
    for(i = 0; i < bcp->nEventos; i++){
        free(bcp->eventos[i]);
    }
    free(bcp->eventos);
    free(bcp);
}