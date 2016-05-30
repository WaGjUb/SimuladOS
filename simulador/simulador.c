#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "erros.h"
#include <stdint.h>

#include "arq_experimento.h"
#include "arq_processos.h"
#include "bcpList.h"
#include "intList.h"

//#define SIM_DEBUG 
//#define ALL_TICKS

//Estrutura contendo informações sobre o experimento
experimento_t *experimento = NULL;
//Estrutura contendo os processos sendo executados
arq_processos_t *processos = NULL;
//Lista de processos prontos
bcpList_t *prontos = NULL;
//Lista de processos bloqueados
bcpList_t *bloqueados = NULL;
//Lista de processos que ainda não foram instanciados
bcpList_t *novos = NULL;
//Ponteiro para o processo atual
bcp_t* executando = NULL;
//Relógio do sistema
uint64_t relogio;
float TME = 0;	
long unsigned int ticks;
unsigned int qtdProcessosConcluidos=0;

int main(int argc, char** argv) {
    
    int i;
    
    if(argc != 2){
        fprintf(stderr, "uso: %s arquivo_de_experimento\n", argv[0]);
        exit(ARGUMENTOS_INVALIDOS);
    }
    
    //Ler arquivo de experimento
    experimento = EXPERIMENTO_ler(argv[1]);
    
#ifdef SIM_DEBUG    
    EXPERIMENTO_imprimir(experimento);
#endif
    
    //Tirar \n do nome do arquivo de processos
    *(strstr(experimento->arq_processos, "\n")) = '\0';
    
    //Ler arquivo de processos
    processos = PROCESSOS_ler(experimento->arq_processos);
    
#ifndef SIM_DEBUG
    PROCESSOS_imprimir(processos);
#endif
    
    //Criar as filas de processos
    prontos = LISTA_BCP_criar();
    bloqueados = LISTA_BCP_criar();
    novos = LISTA_BCP_criar();
    
    //Inserir todos os processos do arquivo de processos na lista de novos
    for(i = 0; i < processos->nProcessos; i++){
        LISTA_BCP_inserir(novos, processos->processos[i]);
    }
   /// até aqui houve apenas a inserção na lista de novos .... OBS todos os processos são inseridos 
    relogio = 0;
    
#ifdef SIM_DEBUG
    int prontos_ant, bloqueados_ant, novos_ant, executando_ant;
#endif
    //variaveis de para impressão 
    uint64_t trocas_de_contexto = 0; //quantas vezes houve chaveamento de processo?
    uint64_t tempo_ocioso = 0; // ver o que é
    
#ifdef SIM_DEBUG    
    prontos_ant = bloqueados_ant = novos_ant = 0;
    executando_ant = -1;
#endif
    
    //Executar a simulação enquanto as listas não são vazias ou há pelo menos um processo executando, (SÓ PARA QUANDO ESVAZIA TUDO)
    while(!LISTA_BCP_vazia(prontos) 
            || !LISTA_BCP_vazia(bloqueados) 
            || !LISTA_BCP_vazia(novos)
            || executando != NULL){
        
#ifdef SIM_DEBUG
        if((prontos_ant != prontos->tam) || (bloqueados_ant != bloqueados->tam) || (novos_ant != novos->tam) || ( executando ? executando_ant != executando->pid : executando_ant != -1)){
        
            prontos_ant = prontos->tam;
            bloqueados_ant = bloqueados->tam;
            novos_ant = novos->tam;
            executando_ant = executando ? executando->pid : -1;
            
            printf("%ld: prontos: %d, bloqueados: %d, novos: %d, executando = %d\n", relogio, prontos->tam, bloqueados->tam, novos->tam, executando ? executando->pid : -1);
            
            //if porquinho.. mas como é pra debug tem desculpa :)
            if(experimento->politica->politica == POL_RR){
                printf("tamfifo: %d\n", experimento->politica->param.rr->fifo->tam);
            }
        }
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

       //ver o que essa variavel faz 
        evento_t* e;
 
       
        //Isto é pra evitar o laço que somente faz relogio++ (compensa!)
        //entra no if apenas quando o programa está ocioso ou quando é inicializado
        if((bloqueados->tam == 0) && (prontos->tam == 0) && (novos->tam > 0) && (!executando)){
			//se a entrada do primeiro da lista maior ou igual que 0
             if(novos->data[0]->entrada >= 0){
    	
				//calcula o tempo ocioso e o TME pula o relogio para o valor da entrada do processo 
                tempo_ocioso += novos->data[0]->entrada - relogio;
				TME += prontos->tam*(novos->data[0]->entrada - relogio);
				uint64_t oldRelogio = relogio;
                relogio = novos->data[0]->entrada;

				/*quantos ticks o simulador pulou*/
				ticks = relogio - oldRelogio;
            }
        }
        
        //verificar se algum processo novo chegou neste momento
        //se a entrada do novo é a do relogio atual
        if(novos->data[0]->entrada == relogio){
			//é inserido no prontos
            LISTA_BCP_inserir(prontos, novos->data[0]);
			//add o novo processo na lista da politica correta
            experimento->politica->novoProcesso(experimento->politica, novos->data[0]);
			//remove do novos o primeiro elemento da lista
            LISTA_BCP_remover(novos, novos->data[0]->pid);
        }
        
        //Se há um processo executando...
        if(executando){
            //contabilizar mais um tempo executado no processo ATUAL
            executando->tempoExecutado++;
            
            //verificar se é hora de um evento!
            if(executando->eventos[executando->proxEvento]->tempo == executando->tempoExecutado){
                
                //É um evento de término?, se sim finaliza o processo 
                if(executando->eventos[executando->proxEvento]->evento == EVT_TERMINO){
                    printf("O processo PID = %d terminou no tempo %ld!\n", executando->pid, relogio);
                    //invocar callback para o final de processo.
                    experimento->politica->fimProcesso(experimento->politica, executando);
                    
                    //salvar o momento da última execução
                    executando->tUltimaExec = relogio;
                    //Destruir o BCP do processo.
                    BCP_destruir(executando);
                    
                    //retirar o processo de execução
                    executando = NULL;
					qtdProcessosConcluidos++;
                }
                else{
                    //Se não for evento de término, é um evento de bloqueio
                    executando->proxEvento++;
                    //O próximo evento é um desbloqueio! 
                    executando->tempoBloqueio = executando->eventos[executando->proxEvento]->tempo + 1;
                    //Apontar para o próximo evento
                    executando->proxEvento++;
                    
                    //Bloquear processo.
                    LISTA_BCP_inserir(bloqueados, executando);

                    //retirar processo de execução
                    executando = NULL;
                }
            }
            
        }
        

		#ifdef ALL_TICKS
		/*chama o callback na quantidade de ticks que aconteceram*/
		for( i=0; i< ticks; i++ )
		{
			//Executar o que tiver que ser executado a cada tick do relógio (callback)
			experimento->politica->tick(experimento->politica);
		}
	   	#else
		//Chama o tick menos vezes
		experimento->politica->tick(experimento->politica);
		#endif
       
        //Atualizar o tempo de bloqueio de todos os processos bloqueados
        for(i = 0; i < bloqueados->tam; i++){
            bloqueados->data[i]->tempoBloqueio--;

            //se um processo deve ser desbloqueado
            if(bloqueados->data[i]->tempoBloqueio <= 0){
                bcp_t* p;
                p = bloqueados->data[i];
                //insere na fila de prontos
                LISTA_BCP_inserir(prontos, bloqueados->data[i]);
                //retira da fila de bloqueados
                LISTA_BCP_remover(bloqueados, bloqueados->data[i]->pid);
                //Invocar callback de desbloqueio de processo
                experimento->politica->desbloqueado(experimento->politica, p);
            }

        }

        //Se não há processo executando...
        if(!executando){
            //Invocar o callback de escalonamento para escolher um processo para ocupar a CPU!
            executando = experimento->politica->escalonar(experimento->politica);
            if(executando != NULL){
                trocas_de_contexto++;
                
                //Dica: provavelmente aqui é um ponto que é usado para computar o TME deste processo.
                
                if(executando->tPrimeiraExec == -1){
					executando->tPrimeiraExec = relogio+1;
                }
                
            }
            else{
                tempo_ocioso++;
            }
        }        
	//soma o tme todos que estão na lista de prontos desse ciclo
        TME += prontos->tam;
        relogio++;
    }
    
//    float TME = 0;
    float acum = 0;
	float vazao = processos->nProcessos / (relogio/1000);
    
    //Calcular TME! (ver definição nos slides!)
	TME = (float) TME/processos->nProcessos;  
    printf("Trocas de Contexto: %ld\n", trocas_de_contexto);
    printf("Tempo ocioso: %ld\n", tempo_ocioso);
    printf("Tempo médio de espera: %.2f\n", TME);
    printf("Vazão : %.5f\n", vazao);
	
    
    return (EXIT_SUCCESS);
}

