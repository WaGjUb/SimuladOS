#ifndef BCPLIST_H
#define	BCPLIST_H

#include "bcp.h"

#define LISTA_N_ENCONTRADO      -1
#define LISTA_ALOC_INCREMENTO   10

typedef struct bcpList_t{
    bcp_t** data;
    int tam;
    int aloc;
} bcpList_t;

bcpList_t* LISTA_BCP_criar();
void LISTA_BCP_inserir(bcpList_t* l, bcp_t* x);
void LISTA_BCP_destruir(bcpList_t* l);
int LISTA_BCP_buscar(bcpList_t* l, int pid);
void LISTA_BCP_remover(bcpList_t* l, int pid);
int LISTA_BCP_vazia(bcpList_t* l);

#endif	/* BCPLIST_H */
