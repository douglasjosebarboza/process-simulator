#ifndef TABELAPCB_H
#define TABELAPCB_H

#include "entradapcb.h"
#include <stdbool.h>

typedef struct {
    EntradaPCB** ponteiro;
    size_t capacidade;
    size_t quantidade;
} TabelaPCB;

TabelaPCB* pcbInit();                                             //Pode gerar erro de SO
bool pcbInsere(TabelaPCB* tabela, EntradaPCB* entrada_tomaPosse);
bool pcbRemove(TabelaPCB* tabela, size_t indice);                 //Remove a entrada da lista e desaloca ela.
EntradaPCB* pcbAcessa(TabelaPCB* tabela, size_t indice);          //Acessa a lista internamente
void pcbDesaloca(TabelaPCB* tabela);

#endif //TABELAPCB_H

/*
Possui os seguintes dados:
  ponteiro e seus pertencentes (as entradas nela presentes).
*/