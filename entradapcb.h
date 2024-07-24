
#ifndef ENTRADAPCB_H
#define ENTRADAPCB_H
#include "processo.h"
#include <sys/types.h>

typedef struct
{
  Processo *processo;
  unsigned int processoID;
  unsigned int paiID;
  unsigned char prioridade;
  unsigned int tempoExecucao;
  unsigned int usoCPU;
  size_t contadorPrograma;
} EntradaPCB;

EntradaPCB *epcbInit(Processo *processo_tomaPosse, unsigned int processoID, unsigned int paiID);
EntradaPCB *epcbCriaFilho(EntradaPCB *objeto, int idFilho);
void epbcDesaloca(EntradaPCB *objeto);
bool epcbTrocaImagem(EntradaPCB *objeto, char *novaImagem_tomaPosse);

#endif // ENTRADAPCB_H

/*
Possui os seguintes dados:
  processo e pertencentes a ele.
*/