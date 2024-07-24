#include "entradapcb.h"
#include "processomanager.h"
#include "listaint.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <math.h>

static char *leTudo(FILE *arquivo)
{
  ListaInt *lista = lintInit();
  if (lista == NULL)
    return NULL;
  for (int c = fgetc(arquivo); c != EOF; c = fgetc(arquivo))
  {
    if (lintInsere(lista, c) == false)
    {
      lintDesaloca(lista);
      return NULL;
    }
  }
  if (lintInsere(lista, '\0') == false)
  {
    lintDesaloca(lista);
    return NULL;
  }
  char *buffer = malloc(lista->quantidade);
  if (buffer == NULL)
  {
    lintDesaloca(lista);
    return NULL;
  }
  char caractere;
  for (size_t i = 0; i < lista->quantidade; i += 1)
  {
    if (lintAcessa(lista, i) == NULL)
    {
      free(buffer);
      lintDesaloca(lista);
      return NULL;
    }
    caractere = (char)*lintAcessa(lista, i);
    buffer[i] = caractere;
  }
  lintDesaloca(lista);
  return buffer;
}

ProcessoManager *prcsmngInit()
{
  ProcessoManager *objeto = malloc(sizeof(ProcessoManager));
  if (objeto == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar memoria para o gerenciador de processos\n");
    return NULL;
  }
  objeto->tabelaPCB = pcbInit();
  if (objeto->tabelaPCB == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar memoria para o gerenciador de processos\n");
    free(objeto);
    return NULL;
  }
  objeto->estadoPronto = lintInit();
  if (objeto->estadoPronto == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar a memoria para o gerenciador de processos\n");
    pcbDesaloca(objeto->tabelaPCB);
    free(objeto);
    return NULL;
  }
  objeto->estadoBloqueado = lintInit();
  if (objeto->estadoBloqueado == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar a memoria para o gerenciador de processos\n");
    pcbDesaloca(objeto->tabelaPCB);
    lintDesaloca(objeto->estadoPronto);
    free(objeto);
    return NULL;
  }
  objeto->tempo = 0;
  objeto->proximoId = 1;
  objeto->estadoExecutando = 0;
  objeto->CPU = malloc(sizeof(CPU));
  if (objeto->CPU == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar a memoria para o gerenciador de processos\n");
    pcbDesaloca(objeto->tabelaPCB);
    lintDesaloca(objeto->estadoPronto);
    lintDesaloca(objeto->estadoBloqueado);
    free(objeto);
    return NULL;
  }
  FILE *arquivoInit = fopen("init", "r");
  if (arquivoInit == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao abrir o arquivo \"init\"\n");
    pcbDesaloca(objeto->tabelaPCB);
    lintDesaloca(objeto->estadoPronto);
    lintDesaloca(objeto->estadoBloqueado);
    free(objeto->CPU);
    free(objeto);
    return NULL;
  }
  char *stringInit = leTudo(arquivoInit);
  if (stringInit == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar a memoria para o gerenciador de processos\n");
    pcbDesaloca(objeto->tabelaPCB);
    lintDesaloca(objeto->estadoPronto);
    lintDesaloca(objeto->estadoBloqueado);
    free(objeto->CPU);
    free(objeto);
    fclose(arquivoInit);
    return NULL;
  }
  Processo *processo0 = prcsInit(stringInit);
  if (processo0 == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar a memoria para o gerenciador de processos\n");
    pcbDesaloca(objeto->tabelaPCB);
    lintDesaloca(objeto->estadoPronto);
    lintDesaloca(objeto->estadoBloqueado);
    free(objeto->CPU);
    free(objeto);
    fclose(arquivoInit);
    free(stringInit);
    return NULL;
  }
  EntradaPCB *entrada0 = epcbInit(processo0, 0, 0);
  if (entrada0 == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao alocar a memoria para o gerenciador de processos\n");
    pcbDesaloca(objeto->tabelaPCB);
    lintDesaloca(objeto->estadoPronto);
    lintDesaloca(objeto->estadoBloqueado);
    free(objeto->CPU);
    free(objeto);
    fclose(arquivoInit);
    prcsDesaloca(processo0);
    return NULL;
  }
  objeto->CPU->processo = processo0;
  objeto->CPU->tempoEntrada = 0;
  objeto->CPU->contadorProgramaAtual = 0;
  if (pcbInsere(objeto->tabelaPCB, entrada0) == false)
  {
    prcsmngDesaloca(objeto);
    fclose(arquivoInit);
    epbcDesaloca(entrada0);
    return NULL;
  }
  return objeto;
}

static size_t prcsmngEscalonador(ProcessoManager *objeto)
{
  srand(time(NULL));
  return ((size_t)rand()) % objeto->estadoPronto->quantidade;
}

bool prcsmngTrocaContexto(ProcessoManager *objeto)
{
  if (objeto->estadoPronto->quantidade == 0)
    return false;
  CPU *cpu = objeto->CPU;
  EntradaPCB *executandoAntigo = pcbAcessa(objeto->tabelaPCB, objeto->estadoExecutando);
  if (executandoAntigo == NULL)
    return false;
  executandoAntigo->contadorPrograma = cpu->contadorProgramaAtual;
  executandoAntigo->usoCPU += objeto->tempo - cpu->tempoEntrada;
  if (lintInsere(objeto->estadoBloqueado, objeto->estadoExecutando) == false)
    return false;
  size_t indiceNoEstadoPronto = prcsmngEscalonador(objeto);
  if (lintAcessa(objeto->estadoPronto, indiceNoEstadoPronto) == NULL)
    return false;
  size_t novoIndicePCB = *lintAcessa(objeto->estadoPronto, indiceNoEstadoPronto);
  if (pcbAcessa(objeto->tabelaPCB, novoIndicePCB) == NULL)
    return NULL;
  cpu->contadorProgramaAtual = pcbAcessa(objeto->tabelaPCB, novoIndicePCB)->contadorPrograma;
  cpu->processo = pcbAcessa(objeto->tabelaPCB, novoIndicePCB)->processo;
  cpu->tempoEntrada = objeto->tempo;
  objeto->estadoExecutando = novoIndicePCB;
  if (lintRemove(objeto->estadoPronto, indiceNoEstadoPronto) == false)
    return false;
  return true;
}

static bool eNumerico(char *string)
{
  if (string == NULL)
    return false;
  for (size_t i = 0; i < strlen(string); i += 1)
    if ((string[i] < '0' || string[i] > '9') && string[i] != '-')
      return false;
  return true;
}

static int processaS(ProcessoManager *objeto, char *argumentos)
{
  if (eNumerico(argumentos) == false)
  {
    printf("FALHA EM EXECUCAO: Argumento nao numerico passado para instrucao S\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  objeto->CPU->processo->valor = atoi(argumentos);
  objeto->CPU->contadorProgramaAtual += 1;
  return PRCSMNG_SUCESSO_EXECUCAO;
}

static int processaA(ProcessoManager *objeto, char *argumentos)
{
  if (eNumerico(argumentos) == false)
  {
    printf("FALHA EM EXECUCAO: Argumento nao numerico passado para instrucao A\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  objeto->CPU->processo->valor += atoi(argumentos);
  objeto->CPU->contadorProgramaAtual += 1;
  return PRCSMNG_SUCESSO_EXECUCAO;
}

static int processaD(ProcessoManager *objeto, char *argumentos)
{
  if (eNumerico(argumentos) == false)
  {
    printf("FALHA EM EXECUCAO: Argumento nao numerico passado para instrucao D\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  objeto->CPU->processo->valor -= atoi(argumentos);
  objeto->CPU->contadorProgramaAtual += 1;
  return PRCSMNG_SUCESSO_EXECUCAO;
}

static int processaB(ProcessoManager *objeto, char *argumentos)
{
  objeto->CPU->contadorProgramaAtual += 1;
  if (prcsmngTrocaContexto(objeto))
    return PRCSMNG_SUCESSO_EXECUCAO;
  printf("FALHA EM EXECUCAO: Troca de contexto falhou\n");
  return PRCSMNG_FALHA_EXECUCAO;
}

static int processaE(ProcessoManager *objeto, char *argumentos)
{
  if (pcbRemove(objeto->tabelaPCB, objeto->estadoExecutando) == false)
  {
    printf("FALHA EM EXECUCAO: Erro ao remover entrada da PCB\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  if (objeto->estadoPronto->quantidade == 0)
    return PRCSMNG_CODIGO_SAIDA;
  else
  {
    for (size_t i = 0; i < objeto->estadoPronto->quantidade; i += 1)
      if ((*lintAcessa(objeto->estadoPronto, i)) > objeto->estadoExecutando)
        *lintAcessa(objeto->estadoPronto, i) -= 1;
    for (size_t i = 0; i < objeto->estadoBloqueado->quantidade; i += 1)
      if ((*lintAcessa(objeto->estadoBloqueado, i)) > objeto->estadoExecutando)
        *lintAcessa(objeto->estadoBloqueado, i) -= 1;
  }
  size_t indiceNoPronto = prcsmngEscalonador(objeto);
  if (lintAcessa(objeto->estadoPronto, indiceNoPronto) == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao acessar lista de estados prontos\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  size_t indiceNaPCB = *lintAcessa(objeto->estadoPronto, indiceNoPronto);
  if (pcbAcessa(objeto->tabelaPCB, indiceNaPCB)->processo == NULL)
  {
    printf("FALHA EM EXECUCAO: Erro ao acessar tabela PCB\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  objeto->CPU->processo = pcbAcessa(objeto->tabelaPCB, indiceNaPCB)->processo;
  objeto->CPU->tempoEntrada = objeto->tempo;
  objeto->CPU->contadorProgramaAtual = pcbAcessa(objeto->tabelaPCB, indiceNaPCB)->contadorPrograma;
  objeto->estadoExecutando = indiceNaPCB;
  if (lintRemove(objeto->estadoPronto, indiceNoPronto) == false)
  {
    printf("FALHA EM EXECUCAO: Erro ao remover antigo processo da lista de prontos\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  return PRCSMNG_SUCESSO_EXECUCAO;
}

static int processaF(ProcessoManager *objeto, char *argumentos)
{
  if (eNumerico(argumentos) == false)
  {
    printf("FALHA EM EXECUCAO: Argumento nao numerico passado para instrucao F\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  if (prcsmngNovoProcesso(objeto, atoi(argumentos)) == false)
  {
    printf("FALHA EM EXECUCAO: Erro ao criar novo processo\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  return PRCSMNG_SUCESSO_EXECUCAO;
}

static int processaR(ProcessoManager *objeto, char *argumentos)
{
  if (argumentos == NULL)
  {
    printf("FALHA EM EXECUCAO: Ausencia de argumento para a instrucao R\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  FILE *arquivo = fopen(argumentos, "r");
  if (arquivo == NULL)
  {
    printf("FALHA EM EXECUCAO: Nome de arquivo nao encontrado ao executar a instrucao R\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
  char *novosComandos = leTudo(arquivo);
  if (novosComandos == NULL)
  {
    printf("FALHA EM EXECUCAO: Falha ao alocar memoria\n");
    fclose(arquivo);
    return PRCSMNG_FALHA_EXECUCAO;
  }
  objeto->CPU->contadorProgramaAtual = 0;
  free(objeto->CPU->processo->comandos);
  objeto->CPU->processo->comandos = novosComandos;
  fclose(arquivo);
  return PRCSMNG_SUCESSO_EXECUCAO;
}

static int processaArgumentoProcessoSimulado(ProcessoManager *objeto, char argumento, char *restoLinha)
{
  switch (argumento)
  {
  case 'S':
    return processaS(objeto, restoLinha);
  case 'A':
    return processaA(objeto, restoLinha);
  case 'D':
    return processaD(objeto, restoLinha);
  case 'B':
    return processaB(objeto, restoLinha);
  case 'E':
    return processaE(objeto, restoLinha);
  case 'F':
    return processaF(objeto, restoLinha);
  case 'R':
    return processaR(objeto, restoLinha);
  default:
    printf("FALHA EM EXECUCAO: instrucao nao existente escrita em arquivo\n");
    return PRCSMNG_FALHA_EXECUCAO;
  }
}

int prcsmngExecuta(ProcessoManager *prcsmng, char argumento)
{
  if (argumento == 'Q')
  {
    Processo *processoAtual = prcsmng->CPU->processo;
    char *linha = prcsLer(processoAtual, prcsmng->CPU->contadorProgramaAtual);
    if (linha == NULL)
    {
      printf("FALHA EM EXECUCAO: Tentativa de ler linha invalida de processo.\n");
      return PRCSMNG_FALHA_EXECUCAO;
    }
    char argumentoProcesso = linha[0];
    char *restoLinha = ((strlen(linha) - 1) > 0) ? linha + 2 : NULL;
    int retorno = processaArgumentoProcessoSimulado(prcsmng, argumentoProcesso, restoLinha);
    prcsmng->tempo += 1;
    free(linha);
    return retorno;
  }
  else if (argumento == 'U')
  {
    if (
        lintAcessa(prcsmng->estadoBloqueado, 0) == NULL ||
        lintInsere(prcsmng->estadoPronto, *lintAcessa(prcsmng->estadoBloqueado, 0)) == false ||
        lintRemove(prcsmng->estadoBloqueado, 0) == false)
    {
      printf("FALHA EM EXECUCAO: Erro ao manusear listas\n");
      return PRCSMNG_FALHA_EXECUCAO;
    }
    return PRCSMNG_SUCESSO_EXECUCAO;
  }
  else if (argumento == 'P')
  {
    if (prcsmngNovoReporter(prcsmng) == false)
    {
      printf("FALHA EM EXECUCAO: Erro no manuseio de processos para a criacao do Reporter\n");
      return PRCSMNG_FALHA_EXECUCAO;
    }
    return PRCSMNG_SUCESSO_EXECUCAO;
  }
  else if (argumento == 'T')
  {
    if (prcsmngNovoReporter(prcsmng) == false)
    {
      printf("FALHA EM EXECUCAO: Erro no manuseio de processos para a criacao do Reporter\n");
      return PRCSMNG_FALHA_EXECUCAO;
    }
    return PRCSMNG_CODIGO_SAIDA;
  }
  else
    return PRCSMNG_SUCESSO_EXECUCAO;
}

bool prcsmngNovoProcesso(ProcessoManager *prcsmng, int pulosNoPai)
{
  CPU *cpu = prcsmng->CPU;
  EntradaPCB *entradaA = pcbAcessa(prcsmng->tabelaPCB, prcsmng->estadoExecutando);
  if (entradaA == NULL)
    return false;
  char *comandos = malloc(strlen(cpu->processo->comandos) + 1);
  if (comandos == NULL)
    return false;
  strcpy(comandos, cpu->processo->comandos);
  Processo *processoN = prcsInit(comandos);
  if (processoN == NULL)
  {
    free(comandos);
    return false;
  }
  processoN->valor = cpu->processo->valor;
  EntradaPCB *entradaN = malloc(sizeof(EntradaPCB));
  if (entradaN == NULL)
  {
    prcsDesaloca(processoN);
    return false;
  }
  entradaN->contadorPrograma = cpu->contadorProgramaAtual + 1;
  entradaN->paiID = entradaA->processoID;
  entradaN->prioridade = entradaA->prioridade;
  entradaN->processo = processoN;
  entradaN->processoID = prcsmng->proximoId;
  entradaN->tempoExecucao = 0;
  entradaN->usoCPU = 0;
  prcsmng->proximoId += 1;
  prcsmng->CPU->contadorProgramaAtual += pulosNoPai + 1;
  if (pcbInsere(prcsmng->tabelaPCB, entradaN) == false)
  {
    epbcDesaloca(entradaN);
    return false;
  }
  if (lintInsere(prcsmng->estadoPronto, prcsmng->tabelaPCB->quantidade - 1) == false)
    return false;
  return true;
}

static void codigoReporter(ProcessoManager *prcsmng)
{
  printf("********************\n");
  printf("Estado do sistema\n");
  printf("********************\n");
  printf("Tempo atual: %d\n", prcsmng->tempo);
  printf("Processo executando:\n");
  printf(
      "-> pid(%u) ppid(%u) valor(%d) prioridade(%d) tempoInicio(%d) CPUAteOMomento(%u)\n",
      pcbAcessa(prcsmng->tabelaPCB, prcsmng->estadoExecutando)->processoID,
      pcbAcessa(prcsmng->tabelaPCB, prcsmng->estadoExecutando)->paiID,
      prcsmng->CPU->processo->valor,
      pcbAcessa(prcsmng->tabelaPCB, prcsmng->estadoExecutando)->prioridade,
      prcsmng->CPU->tempoEntrada,
      pcbAcessa(prcsmng->tabelaPCB, prcsmng->estadoExecutando)->usoCPU + prcsmng->tempo - prcsmng->CPU->tempoEntrada);
  printf("Fila de processos bloqueados: [%ld] Processos no total\n", prcsmng->estadoBloqueado->quantidade);
  for (size_t i = 0; i < prcsmng->estadoBloqueado->quantidade; i += 1)
  {
    size_t *indice = lintAcessa(prcsmng->estadoBloqueado, i);
    if (indice == NULL)
      break;
    printf(
        "-> pid(%u) ppid(%u) valor(%d) prioridade(%d) CPUAteOMomento(%u)\n",
        pcbAcessa(prcsmng->tabelaPCB, *indice)->processoID,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->paiID,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->processo->valor,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->prioridade,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->usoCPU);
  }
  printf("Fila de processos prontos: [%ld] Processos no total\n", prcsmng->estadoPronto->quantidade);
  for (size_t i = 0; i < prcsmng->estadoPronto->quantidade; i += 1)
  {
    size_t *indice = lintAcessa(prcsmng->estadoPronto, i);
    if (indice == NULL)
      break;
    printf(
        "-> pid(%u) ppid(%u) valor(%d) prioridade(%d) CPUAteOMomento(%u)\n",
        pcbAcessa(prcsmng->tabelaPCB, *indice)->processoID,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->paiID,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->processo->valor,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->prioridade,
        pcbAcessa(prcsmng->tabelaPCB, *indice)->usoCPU);
  }
  printf("********************\n");
  exit(EXIT_SUCCESS);
}

bool prcsmngNovoReporter(ProcessoManager *prcsmng)
{
  pid_t pidFilho;
  int statusSaida;
  pidFilho = fork();
  if (pidFilho == -1)
    return false;
  else if (pidFilho == 0)
    codigoReporter(prcsmng);
  else
    waitpid(-1, &statusSaida, WUNTRACED);
  return true;
}

void prcsmngDesaloca(ProcessoManager *objeto)
{
  if (objeto == NULL)
    return;
  cpuDesaloca(objeto->CPU);
  lintDesaloca(objeto->estadoBloqueado);
  lintDesaloca(objeto->estadoPronto);
  pcbDesaloca(objeto->tabelaPCB);
  free(objeto);
}