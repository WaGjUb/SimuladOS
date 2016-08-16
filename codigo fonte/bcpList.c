#include <stdlib.h>
#include "bcp.h"
#include "bcpList.h"

bcpList_t* LISTA_BCP_criar(){
    bcpList_t* novo;
    
    novo = malloc(sizeof(bcpList_t));
    
    novo->aloc = LISTA_ALOC_INCREMENTO;
    novo->data = malloc(sizeof(bcp_t*) * novo->aloc);
    novo->tam = 0;
    
    return novo;
}

void LISTA_BCP_inserir(bcpList_t* l, bcp_t* x){
    
    if(LISTA_BCP_buscar(l, x->pid) != LISTA_N_ENCONTRADO) //Lista_n_encontrado = -1
        return;
    
    if(l->tam == l->aloc){
        l->aloc += LISTA_ALOC_INCREMENTO;
        l->data = realloc(l->data, l->aloc * sizeof(bcp_t*));
    }
    
    l->data[l->tam] = x;
    l->tam++;
}

void LISTA_BCP_destruir(bcpList_t* l){
    free(l->data);
    free(l);
}

int LISTA_BCP_buscar(bcpList_t* l, int pid){
    int i;
    int ret;
    
    ret = LISTA_N_ENCONTRADO;
    
    for(i = 0; i < l->tam; i++){
        if(l->data[i]->pid == pid){
            ret = i;
            break;
        }
    }
    
    return ret;
}

void LISTA_BCP_remover(bcpList_t* l, int pid){
    int i, k;
    
    i = LISTA_BCP_buscar(l, pid);
    
    if(i == LISTA_N_ENCONTRADO)
        return;
    
    for(k = i; k < l->tam-1; k++)
        l->data[k] = l->data[k+1];
    
    l->tam--;       
}

int LISTA_BCP_vazia(bcpList_t* l){
    return (l->tam == 0) ? 1 : 0;
}
