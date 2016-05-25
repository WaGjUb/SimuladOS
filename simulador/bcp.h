#ifndef BCP_H
#define	BCP_H

#include <stdint.h>
#include <stdio.h>
#include "eventos.h"

#define EVENTOS_ALOC_INC    20

typedef struct bcp_t{
    int pid;
    uint64_t tempoExecutado;
    int prioridade;
    uint64_t entrada;
    uint64_t tempoBloqueio;
    uint64_t timeSlice;
    evento_t** eventos;
    int nEventos;
    int alocEventos;
    int proxEvento;
    uint64_t tPrimeiraExec;
    uint64_t tUltimaExec;
}bcp_t;

bcp_t* BCP_criar(FILE* arqProcesso);
evento_t* BCP_proxEvento(bcp_t* bcp);
void BCP_destruir(bcp_t* bcp);

#endif	/* BCP_H */

