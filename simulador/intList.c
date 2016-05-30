#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "intList.h"

lista_int_t* LISTA_INT_criar(){
    lista_int_t* novo;
    
    novo = malloc(sizeof(lista_int_t));
    
    novo->aloc = LISTA_ALOC_INCREMENTO;
    novo->data = malloc(sizeof(uint64_t) * novo->aloc);
    novo->tam = 0;
    
    return novo;
}

void LISTA_INT_inserir(lista_int_t* l, uint64_t x){
    
    if(l->tam == l->aloc){
        l->aloc += LISTA_ALOC_INCREMENTO;
        l->data = realloc(l->data, l->aloc * sizeof(uint64_t));
    }
    
    l->data[l->tam] = x;
    l->tam++;
}

void LISTA_INT_remover(lista_int_t* l, uint64_t x){
    int i, k;
    
    i = LISTA_INT_buscar(l, x);
    
    if(i == LISTA_N_ENCONTRADO)
        return;
    
    for(k = i; k < l->tam-1; k++)
        l->data[k] = l->data[k+1];
    
    l->tam--;   
}

uint64_t LISTA_INT_buscar(lista_int_t* l, uint64_t x){
    int i;
    int ret;
    
    ret = LISTA_N_ENCONTRADO;
    
    for(i = 0; i < l->tam; i++){
        if(l->data[i] == x){
            ret = i;
            break;
        }
    }
    
    return ret;
}

void LISTA_INT_destruir(lista_int_t* l){
    free(l->data);
    free(l);
}

uint64_t LISTA_INT_em(lista_int_t* l, int pos){
    if(pos >= 0 && pos < l->tam)
        return l->data[pos];
    
    return -1;
}

void LISTA_INT_alterar(lista_int_t* l, int pos, uint64_t x){
    if(pos >= 0 && pos < l->tam)
        l->data[pos] = x;
}

void LISTA_INT_imprimir(lista_int_t* l){
    int i;
    
    for(i = 0; i <l->tam; i++){
        printf("%d : %lu\n", i, l->data[i]);
    }
}
