#include <stdio.h>
#include <stdlib.h>
#include "arq_experimento.h"
#include "politicas.h"
#include "erros.h"

#define NOME_EXP_TAM_MAX        255
#define NOME_ARQ_PROC_TAM_MAX   255
#define NOME_ARQ_SAIDA_TAM_MAX  255

experimento_t* EXPERIMENTO_ler(char* arqExperimento){
    FILE* arq;
    experimento_t* novo;
    
    arq = fopen(arqExperimento, "r");
    
    if(!arq){
        fprintf(stderr, "Arquivo %s não encontrado!\n", arqExperimento);
        exit(ARQ_EXP_N_ENCONTRADO);
    }
    
    novo = malloc(sizeof(experimento_t));
    
    novo->nome_exp = malloc(NOME_EXP_TAM_MAX * sizeof(char));
    fgets(novo->nome_exp, NOME_EXP_TAM_MAX, arq);
    
    novo->arq_processos = malloc(NOME_ARQ_PROC_TAM_MAX * sizeof(char));
    fgets(novo->arq_processos, NOME_ARQ_PROC_TAM_MAX, arq);
 
    novo->arq_saida = malloc(NOME_ARQ_SAIDA_TAM_MAX * sizeof(char));
    fgets(novo->arq_saida, NOME_ARQ_SAIDA_TAM_MAX, arq);    
    
    novo->politica = POLITICA_criar(arq);
    
    fclose(arq);
    
    return novo;
}

void EXPERIMENTO_imprimir(experimento_t* exp){
    printf("Arquivo de experimento %s\n", exp->nome_exp);
    printf("Arquivo de processos %s\n", exp->arq_processos);
    printf("Arquivo de saida %s\n", exp->arq_saida);
    POLITICA_imprimir(exp->politica);
}
