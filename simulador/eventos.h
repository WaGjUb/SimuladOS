#ifndef EVENTOS_H
#define	EVENTOS_H

typedef enum EVENTO{
    EVT_BLOQUEIO,
    EVT_DESBLOQUEIO,
    EVT_TERMINO
}EVENTO;

typedef struct evento_t{
    int tempo;
    EVENTO evento;
}evento_t;

evento_t* EVENTO_criar(char* e);
void EVENTO_imprimir(evento_t* e);

extern unsigned long int tempo_restante;

#endif	/* EVENTOS_H */

