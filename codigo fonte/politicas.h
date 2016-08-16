#ifndef POLITICAS_H
#define	POLITICAS_H
#define NUM_FILAS 40 
#include <stdio.h>

#include "bcp.h"
#include "bcpList.h"

typedef enum POLITICA_ESC{
    POL_RR,
    POL_RANDOM,
    POL_FCFS,
    POL_SJF,
    POL_FP
} POLITICA_ESC;

typedef struct rr_t rr_t;
typedef struct fp_t fp_t;
typedef struct fcfs_t fcfs_t;
typedef struct random_t random_t;
typedef struct sjf_t sjf_t;

typedef struct politica_t{
    POLITICA_ESC politica;
    union{
        rr_t* rr;
        fp_t* fp;
	fcfs_t* fcfs;	
	random_t* random;
	sjf_t* sjf;
    }param;
    bcp_t* (*escalonar)(struct politica_t*);
    void (*tick)(struct politica_t*);
    void (*novoProcesso)(struct politica_t*, bcp_t*);
    void (*fimProcesso)(struct politica_t*, bcp_t*);
    void (*desbloqueado)(struct politica_t*, bcp_t*);
}politica_t;

typedef struct rr_t{
    int quantum;
    bcpList_t* fifo;
    int pos;
}rr_t;

typedef struct fp_t{
    politica_t *filas[NUM_FILAS]; 
}fp_t;

typedef struct fcfs_t{
	bcpList_t* fifo;
}fcfs_t;

typedef struct random_t{
	bcpList_t * lista;
	unsigned int numeroAleatorio;
}random_t;

typedef struct sjf_t{
	bcpList_t* lista;
}sjf_t;


politica_t* POLITICA_criar(FILE* arqProcessos);
void POLITICA_imprimir(politica_t* politica);
#endif	/* POLITICAS_H */
