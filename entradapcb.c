#include "entradapcb.h"
#include "processo.h"
#include <stdlib.h>
#include <string.h>

EntradaPCB* epcbInit(Processo* processo, unsigned int processoID, unsigned int paiID, unsigned char prioridade)
{
  if(processo == NULL)
    return NULL;
  EntradaPCB* retorno = malloc(sizeof(EntradaPCB));
  if(retorno == NULL)
    return NULL;
  retorno->processo         = processo;
  retorno->processoID       = processoID;
  retorno->paiID            = paiID;
  retorno->prioridade       = prioridade;
  retorno->contadorPrograma = 0;
  retorno->tempoExecucao    = 0;
  retorno->usoCPU           = 0;
  return retorno;
}

EntradaPCB* epcbCriaFilho(EntradaPCB* objeto, int idFilho)
{
  if(objeto == NULL)
    return NULL;
  EntradaPCB* retorno = malloc(sizeof(EntradaPCB));
  if(retorno == NULL)
    return NULL;
  retorno->contadorPrograma   = objeto->contadorPrograma;
  retorno->paiID              = objeto->processoID;
  retorno->prioridade         = objeto->prioridade;
  retorno->processoID         = idFilho;
  retorno->tempoExecucao      = 0;
  retorno->usoCPU             = 0;
  retorno->processo           = malloc(sizeof(Processo));
  if(retorno->processo == NULL)
  {
    free(retorno);
    return NULL;
  }
  retorno->processo->valor    = objeto->processo->valor;
  retorno->processo->comandos = malloc(strlen(objeto->processo->comandos) + 1);
  if(retorno->processo->comandos == NULL)
  {
    free(retorno->processo);
    free(retorno);
    return NULL;
  }
  strcpy(retorno->processo->comandos, objeto->processo->comandos);
  return retorno;
}

void epbcDesaloca(EntradaPCB* objeto)
{
  if(objeto == NULL)
    return;
  prcsDesaloca(objeto->processo);
  free(objeto);
}

bool epcbTrocaImagem(EntradaPCB* objeto, char* novaImagem_tomaPosse)
{
  if(objeto == NULL || novaImagem_tomaPosse == NULL)
    return false;
  free(objeto->processo->comandos);
  objeto->processo->comandos = novaImagem_tomaPosse;
  objeto->contadorPrograma = 0;
  return true;  
}