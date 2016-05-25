#ifndef ARQ_EXPERIMENTO_H
#define	ARQ_EXPERIMENTO_H

#include "politicas.h"

typedef struct experimento_t{    
    char* nome_exp;
    char* arq_processos;
    char* arq_saida;
    politica_t* politica;
}experimento_t;

experimento_t* EXPERIMENTO_ler(char* arqExperimento);
void EXPERIMENTO_imprimir(experimento_t* exp);

#endif	/* ARQ_EXPERIMENTO_H */

