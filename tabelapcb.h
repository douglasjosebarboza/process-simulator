#ifndef TABELAPCB_H
#define TABELAPCB_H

#include "entradapcb.h"
#include <stdbool.h>

typedef struct
{
  EntradaPCB **ponteiro;
  size_t capacidade;
  size_t quantidade;
} TabelaPCB;

TabelaPCB *pcbInit();
bool pcbInsere(TabelaPCB *tabela, EntradaPCB *entrada_tomaPosse);
bool pcbRemove(TabelaPCB *tabela, size_t indice);
EntradaPCB *pcbAcessa(TabelaPCB *tabela, size_t indice);
void pcbDesaloca(TabelaPCB *tabela);

#endif // TABELAPCB_H

/*
Possui os seguintes dados:
  ponteiro e seus pertencentes (as entradas nela presentes).
*/