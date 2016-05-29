#include <stdio.h>
#include <stdlib.h>
#include "erros.h"
#include "arq_processos.h"
#include "bcp.h"

arq_processos_t* PROCESSOS_ler(char* arquivo_processos){
    arq_processos_t* novo;
    char* s;
    FILE* arq;
    int i;
    
    novo = malloc(sizeof(arq_processos_t));
    
    arq = fopen(arquivo_processos, "r");
    
    if(!arq){
        fprintf(stderr, "Arquivo %s não encontrado!\n", arquivo_processos);
        exit(ARQ_PROC_N_ENCONTRADO);
    }
    
    s = malloc(sizeof(char) * 10);
    fgets(s, 10, arq);
    
    novo->nProcessos = atoi(s);
    
    //ignorar faixa de prioridades!
    fgets(s, 10, arq);
	novo->faixaPrioridade = s;    
    novo->processos = malloc(sizeof(bcp_t*) * novo->nProcessos);
    
    
    for(i = 0; i < novo->nProcessos; i++){
        novo->processos[i] = BCP_criar(arq);
    }
  
//não da o free pois a faixa de prioridade não está mais sendo ignorada  
//    free(s);
    return novo;
}

void PROCESSOS_imprimir(arq_processos_t* proc){
    int i, j;
    printf("número de processos: %d\n"
	"faixa de prioridade: %s\n", proc->nProcessos, proc->faixaPrioridade);
    
    for(i = 0; i < proc->nProcessos; i++){
        printf("PID: %d Prioridade: %d Entrada: %d nEventos: %d\n",
                proc->processos[i]->pid,
                proc->processos[i]->prioridade,
                proc->processos[i]->entrada,
                proc->processos[i]->nEventos);
        
        for(j = 0; j < proc->processos[i]->nEventos; j++){
            EVENTO_imprimir(proc->processos[i]->eventos[j]);
        }
        
        
    }
    
}
