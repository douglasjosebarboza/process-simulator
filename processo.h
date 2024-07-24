#ifndef PROCESSO_H
#define PROCESSO_H
#include <sys/types.h>
#include <stdbool.h>

typedef struct
{
  int valor;
  char *comandos;
} Processo;

Processo *prcsInit(char *argumentos_tomaPosse);
char *prcsLer(Processo *objeto, size_t indiceComando);
void prcsDesaloca(Processo *objeto);

#endif // PROCESSO_H

/*
Possui os seguintes dados:
  comandos
*/