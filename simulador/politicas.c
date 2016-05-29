#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "politicas.h"
#include "bcp.h"
#include "bcpList.h"

extern bcpList_t *bloqueados;
extern bcpList_t *prontos;
extern bcp_t* executando;

/*
 * Funções DUMMY são aquelas que não fazem nada... 
 * 
 * Existem políticas que não tomam ações em determinados pontos da simulação. Nestes casos
 * usa-se as rotinas DUMMY para não ter que tratar essas casos no loop de simulação.
*/

void DUMMY_tick(struct politica_t *p){
    return;
}

void DUMMY_novo(struct politica_t *p, bcp_t* novoProcesso){
    return;
}

void DUMMY_fim(struct politica_t *p, bcp_t* processoTerminado){
    return;
}

void DUMMY_desbloqueado(struct politica_t* p, bcp_t* processoDesbloqueado){
    return;
}


/*
 * Round-Robin
 * 
 * Os callbacks abaixo implementam a política round-robin para escalonamento de processos
 * 
 */

void RR_tick(struct politica_t *p){
    if(executando){
        //decrementar o tempo restante deste processo
        executando->timeSlice--;
        if(executando->timeSlice <= 0){
            //se o tempo do processo acabou, inserir o processo atual na lista de prontos
            LISTA_BCP_inserir(prontos, executando);
            //remover o processo atual de execução
            executando = NULL;
        }
    }
}

void RR_novoProcesso(struct politica_t *p, bcp_t* novoProcesso){
    //quando um novo processo chega, ele é inserido na fila round robin
    LISTA_BCP_inserir(p->param.rr->fifo, novoProcesso);
}

bcp_t* RR_escalonar(struct politica_t *p){
    bcp_t* ret;
    int nBloqueados = 0;
    
    //Se não há processos na fila round-robin, retornar nenhum
    if(p->param.rr->fifo->tam == 0)
        return NULL;
    
    //testar todos os processos da fila round-robin a partir da posição atual
    while(nBloqueados < p->param.rr->fifo->tam){
    
        //verificar é necessário apontar para o primeiro elemento novamente
        if(p->param.rr->pos >= p->param.rr->fifo->tam){
            p->param.rr->pos = 0;
        }
        ret = p->param.rr->fifo->data[p->param.rr->pos];

        //verificar se o atual da fila round-robin está bloqueado
        if(LISTA_BCP_buscar(bloqueados, ret->pid) != LISTA_N_ENCONTRADO){
            //Se estiver, testar o próximo! 
            nBloqueados++;
            ret = NULL;
        }
        else{
            //retornar o processo para ser executado!
            LISTA_BCP_remover(prontos, ret->pid);
            ret->timeSlice = p->param.rr->quantum;
            break;
        }
        
        p->param.rr->pos++;
    }
    
    p->param.rr->pos++;
    
    return ret;
}

void RR_fimProcesso(struct politica_t *p, bcp_t* processo){
    //Quando um processo termina, removê-lo da fila round-robin
    LISTA_BCP_remover(p->param.rr->fifo, processo->pid);
}

/***********************RANDOM***********************/
void RANDOM_tick(struct politica_t *p){
	p->param.random->numeroAleatorio+=41;
    return;
}

void RANDOM_novo(struct politica_t *p, bcp_t* novoProcesso){
	/*Para deixar mais "Aleatorio"*/
	p->param.random->numeroAleatorio += novoProcesso->pid;
	/*Insere na lista*/
	LISTA_BCP_inserir(p->param.random->lista, novoProcesso);
    return;
}

void RANDOM_fim(struct politica_t *p, bcp_t* processoTerminado){
	/*Para deixar mais "Aleatorio"*/
	p->param.random->numeroAleatorio += processoTerminado->pid;
	/*Remove o processo da lista*/
	LISTA_BCP_remover(p->param.random->lista, processoTerminado->pid);
    return;
}

void RANDOM_desbloqueado(struct politica_t* p, bcp_t* processoDesbloqueado){
	/*Para deixar mais "Aleatorio"*/
	p->param.random->numeroAleatorio += processoDesbloqueado->pid;
	
    return;
}

bcp_t* RANDOM_escalonar(struct politica_t *p){

	/*se não houver processos para serem executados*/
	/*retorna NULL*/
	if( p->param.random->lista->tam == 0 ){
		return NULL;
	}

	/*Se houver, remove  um processo da lista de prontos 
	  e remove da lista de bloqueados*/
	int indice;
	bcp_t * ret = NULL;

	if( prontos->tam )
	{
		indice = p->param.random->numeroAleatorio % prontos->tam;
		ret = prontos->data[indice];
		LISTA_BCP_remover(bloqueados, ret->pid );
		LISTA_BCP_remover(prontos,ret->pid);
	}

	/*se não houverem processos prontos, retorna NULL*/
	return ret;
}


/****************************************************/


/**
 * Criar uma instância da política First Come First Served
 */
////////////////// FCFS /////////////////////////
void FCFS_novoProcesso(struct politica_t *p, bcp_t* novoProcesso)
{
	//insere o novo processo 
	LISTA_BCP_inserir(p->param.fcfs->fifo, novoProcesso);
}

void FCFS_fimProcesso(struct politica_t *p, bcp_t* processo)
{
	//remove o processo
	LISTA_BCP_remover(p->param.fcfs->fifo, processo->pid);
}

bcp_t* FCFS_escalonar(struct politica_t *p)
{
	//Se não tiver ninguem na lista retorna NULL
	bcp_t* ret = NULL;
	int i;
	//vai percorrendo a lista ate chegar no final ou até achar alguem desbloqueado
	for (i=0; i < p->param.fcfs->fifo->tam; i++)
	{
		ret = p->param.fcfs->fifo->data[i];
		//se o processo não estiver bloqueado
		if (LISTA_BCP_buscar(bloqueados, ret->pid) == LISTA_N_ENCONTRADO)
		{
			//remove ele do prontos e retorna ele para ser executado
			LISTA_BCP_remover(prontos, ret->pid);
			return ret;		
		}
		else
		{
			//se não entra no loop de novo e pega o proximo da lista
			ret = NULL; // para se esse for o final da lista não executar ninguem
		}

	}
	return ret;
}

politica_t* POLITICARR_criar(FILE* arqProcessos, char* quantum){
    politica_t* p;
    char* s;
    rr_t* rr;
    
    p = malloc(sizeof(politica_t));
    
    p->politica = POL_RR;
    
    //Ligar os callbacks com as rotinas RR
    p->escalonar = RR_escalonar;
    p->tick = RR_tick;
    p->novoProcesso = RR_novoProcesso;
    p->fimProcesso = RR_fimProcesso;
    p->desbloqueado = DUMMY_desbloqueado;
    
    //Alocar a struct que contém os parâmetros para a política round-robin
    rr = malloc(sizeof(rr_t));
    s = malloc(sizeof(char) * 10);

    if(quantum == NULL)
	{
    fgets(s, 10, arqProcessos);
    }
	else
	{
		strcpy(s, quantum);
	}
    //inicializar a estrutura de dados round-robin
    rr->quantum = atoi(s);
    rr->fifo = LISTA_BCP_criar();
    rr->pos = 0;
    
    //Atualizar a política com os parâmetros do escalonador
    p->param.rr = rr;
    
    free(s);
    
    return p;
    
}

////////////////////////////////FP/////////////////////////////////
void FP_novoProcesso(struct politica_t *p, bcp_t* novoProcesso)
{
	//TODO implementar a lista
	//indice da lista que será inserido o novo processo
	int indice;
	indice = novoProcesso->prioridade-1; //conversão de 0-39 indices e 1-40 prioridade
	//chama a função de inserir correspondente ao novo processo :) 
	p->param.fp->filas[indice]->novoProcesso(p->param.fp->filas[indice], novoProcesso);
}

void FP_fimProcesso(struct politica_t *p, bcp_t* processo)
{
	//armazena o indice correspondente ao algoritmo
	int indice;
	//conversão
	indice = processo->prioridade-1;
	//chama a função de remoção correta
	p->param.fp->filas[indice]->fimProcesso(p->param.fp->filas[indice], processo);
}

bcp_t* FP_escalonar(struct politica_t *p)
{

	int i;
	bcp_t* ret = NULL; 
	//percorre a fila do indice mais baixo (maior prioridade) ate o o mais alto (maior prioridade)
	for (i=0; i<NUM_FILAS; i++)
	{
		//chama o escalonar de cada posição da lista 
		ret = p->param.fp->filas[i]->escalonar(p->param.fp->filas[i]);
		//se encontrar algum processo para ser excutado retorna ele
		if (ret != NULL)
		{
			return ret;
		}
	}
	
	return ret;
}


void FP_tick(struct politica_t *p){
	if (executando)
	{
		p->param.fp->filas[executando->prioridade-1]->tick(p->param.fp->filas[executando->prioridade-1]);
	}  
}

void FP_desbloqueado(struct politica_t* p, bcp_t* processo)
{
		p->param.fp->filas[processo->prioridade-1]->desbloqueado(p->param.fp->filas[processo->prioridade-1], processo);
}


politica_t* POLITICAFP_criar(FILE* arqProcessos){

 politica_t* p;
    char s[25];
    fp_t* fp;
	int i;    

    p = malloc(sizeof(politica_t));
    
    p->politica = POL_FP;
    
    //Ligar os callbacks com as rotinas FP
  
  p->escalonar = FP_escalonar;
    p->tick = FP_tick;
    p->novoProcesso = FP_novoProcesso;
    p->fimProcesso = FP_fimProcesso;
    p->desbloqueado = FP_desbloqueado;
    
    //Alocar a struct que contém os parâmetros para a política FP
    fp = malloc(sizeof(fp_t));



    fgets(s, 20, arqProcessos); //ignora a quantidade de filas
    
	//cria a politica para cada fila	    
	for (i=0; i<NUM_FILAS; i++)
	{

		fp->filas[i] = POLITICA_criar(arqProcessos); //vai passando linha a linha e criando politicas
	}

    //Atualizar a política com os parâmetros do escalonador
    p->param.fp = fp;
    
    
    return p;
}

politica_t* POLITICA_criar(FILE* arqProcessos){ //esse aqui na vdd é o arquivo de experimentos
    char* str;
    
    str = malloc(sizeof(char) * 20);

    fgets(str, 20, arqProcessos);

    
    //*(strstr(str, "\n")) = '\0';
    
    politica_t* p;
    p = malloc(sizeof(politica_t));
    
    if(!strncmp(str, "sjf", 3)){
        p->param.rr = NULL;
        p->politica = POL_SJF;
        p->escalonar = NULL;
        p->tick = DUMMY_tick;
        p->novoProcesso = DUMMY_novo;
        p->fimProcesso = DUMMY_fim;
        p->desbloqueado = DUMMY_desbloqueado;
    }
    
    if(!strncmp(str, "fcfs", 4)){
	fcfs_t *fcfs = (fcfs_t*) malloc(sizeof(fcfs));
	fcfs->fifo = LISTA_BCP_criar();
        p->param.fcfs = fcfs; //ON
        p->politica = POL_FCFS; 
        p->escalonar = FCFS_escalonar; // ON
        p->tick = DUMMY_tick; 
        p->novoProcesso = FCFS_novoProcesso; //ON
        p->fimProcesso = FCFS_fimProcesso; //ON
        p->desbloqueado = DUMMY_desbloqueado;
    }
    
    if(!strncmp(str, "random",6)){
		random_t* random = (random_t*) malloc(sizeof(random_t));
		random->lista = LISTA_BCP_criar();

        p->param.random = random;
        p->politica = POL_RANDOM;
        p->escalonar = RANDOM_escalonar;
        p->tick = RANDOM_tick;
        p->novoProcesso = RANDOM_novo;
        p->fimProcesso = RANDOM_fim;
        p->desbloqueado = RANDOM_desbloqueado;
    }
    
    if(!strncmp(str, "rr", 2)){
        free(p);
	if (!strncmp(str, "rr(", 3))
	{
		char string[20];
		int k = 0;
		while(str[k+3] != ')')
		{
			string[k] = str[k+3];
			k++;
		}
		string[k] = '\0';
	   p = POLITICARR_criar(arqProcessos, string);
	}
	else
	{	
     	   p = POLITICARR_criar(arqProcessos, NULL);
 	}
    }

    if(!strncmp(str, "fp", 2)){
        free(p);
        p = POLITICAFP_criar(arqProcessos);
    }
    
    free(str);
    
    return p;
}

void POLITICA_imprimir(politica_t* politica){
    const char* pol;
    
    if(politica->politica == POL_FCFS)
        pol = "FCFS";
    if(politica->politica == POL_FP)
        pol = "FP";
    if(politica->politica == POL_RANDOM)
        pol = "RANDOM";
    if(politica->politica == POL_RR)
        pol = "RR";
    if(politica->politica == POL_SJF)
        pol = "SJF";

    printf("política de escalonamento: %s\n", pol);
    if(politica->politica == POL_RR)
        printf("\tquantum: %d\n", politica->param.rr->quantum);
    
    return;
}
