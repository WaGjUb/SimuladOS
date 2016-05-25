#ifndef ARQ_PROCESSOS_H
#define	ARQ_PROCESSOS_H

#include "bcp.h"

typedef struct arq_processos_t{
    int nProcessos;
    bcp_t** processos;
}arq_processos_t;

arq_processos_t* PROCESSOS_ler(char* arquivo_processos);
void PROCESSOS_imprimir(arq_processos_t* proc);

#endif	/* ARQ_PROCESSOS_H */

