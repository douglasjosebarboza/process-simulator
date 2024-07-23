#ifndef PROCESSO_H
#define PROCESSO_H
#include <sys/types.h>
#include <stdbool.h>

typedef struct
{
  int   valor;
  char* comandos;
} Processo;

Processo* prcsInit(char* argumentos_tomaPosse);         //Pode gerar erro por SO
char* prcsLer(Processo* objeto, size_t indiceComando);  //Pode retornar nulo, string alocada com comando ou dar erro de SO
void prcsDesaloca(Processo* objeto);

#endif //PROCESSO_H

/*
Possui os seguintes dados:
  comandos
*/