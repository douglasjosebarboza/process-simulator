#ifndef PROCESSOMANAGER_H
#define PROCESSOMANAGER_H
#include "cpu.h"
#include "tabelapcb.h"
#include "listaint.h"
#include <stdbool.h>

#define PRCSMNG_SUCESSO_EXECUCAO    1
#define PRCSMNG_FALHA_EXECUCAO      0
#define PRCSMNG_CODIGO_SAIDA        -1

typedef struct {
  int         tempo;
  int         proximoId;
  CPU*        CPU;
  TabelaPCB*  tabelaPCB;
  ListaInt*   estadoPronto;
  size_t      estadoExecutando; 
  ListaInt*   estadoBloqueado;
} ProcessoManager;
 
ProcessoManager* prcsmngInit();
bool prcsmngNovoProcesso(ProcessoManager* prcsmng, int pulosNoPai);
bool prcsmngNovoReporter(ProcessoManager* prcsmng);
bool prcsmngTrocaContexto(ProcessoManager* prcsmng);
int prcsmngExecuta(ProcessoManager* prcsmng, char argumento);
void prcsmngDesaloca(ProcessoManager* prcsmng);

#endif //PROCESSOMANAGER_H

/*
Possui os seguintes dados:
  todos os ponteiros 
*/
