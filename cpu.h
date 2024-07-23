
#ifndef CPU_H
#define CPU_H
#include "processo.h"
#include <sys/types.h>

typedef struct
{
  Processo* processo;
  int       tempoEntrada;
  size_t    contadorProgramaAtual;
} CPU;

void cpuDesaloca(CPU* objeto);

#endif //CPU_H

/*
Possui os seguintes dados:
  Nenhum
*/