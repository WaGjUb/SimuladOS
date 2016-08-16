#ifndef LISTA_H
#define	LISTA_H

#define LISTA_N_ENCONTRADO      -1
#define LISTA_ALOC_INCREMENTO   10

typedef struct lista_int_t{
    uint64_t* data;
    int tam;
    int aloc;
} lista_int_t;

lista_int_t* LISTA_INT_criar();
void LISTA_INT_inserir(lista_int_t* l, uint64_t x);
void LISTA_INT_remover(lista_int_t* l, uint64_t x);
uint64_t LISTA_INT_buscar(lista_int_t* l, uint64_t x);
void LISTA_INT_destruir(lista_int_t* l);
uint64_t LISTA_INT_em(lista_int_t* l, int pos);
void LISTA_INT_alterar(lista_int_t* l, int pos, uint64_t x);
void LISTA_INT_imprimir(lista_int_t* l);

#endif	/* LISTA_H */

