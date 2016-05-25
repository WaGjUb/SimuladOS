CC=gcc
DEBUG=-ggdb
CFLAGS=
LDFLAGS=
OPT=-O0

all: lib simulador link_simulador

lib: eventos.c politicas.c arq_processos.c arq_experimento.c bcp.c bcpList.c intList.c
	$(CC) eventos.c -c $(OPT) $(CFLAGS) $(DEBUG)
	$(CC) politicas.c -c $(OPT) $(CFLAGS) $(DEBUG)
	$(CC) arq_processos.c -c $(OPT) $(CFLAGS) $(DEBUG)
	$(CC) arq_experimento.c -c $(OPT) $(CFLAGS) $(DEBUG)
	$(CC) bcp.c -c $(OPT) $(CFLAGS) $(DEBUG)
	$(CC) bcpList.c -c $(OPT) $(CFLAGS) $(DEBUG)
	$(CC) intList.c -c $(OPT) $(CFLAGS) $(DEBUG)
	
simulador: simulador.c
	$(CC) simulador.c -c $(OPT) $(CFLAGS) $(DEBUG)

link_simulador:
	$(CC) simulador.o intList.c eventos.o politicas.o arq_processos.o arq_experimento.o bcp.o bcpList.o -o simulador $(LDFLAGS)
	
clean:
	rm -f *.o
	rm -f simulador
