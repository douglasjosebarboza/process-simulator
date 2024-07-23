#include "processo.h"
#include <stdlib.h>
#include <string.h>

static size_t inicioLinha(const char* texto, size_t numeroLinha) //retorna o char depois do \n da linha anterior
{
  size_t retorno = 0;
  size_t linhaAtual = 0;
  size_t tamTexto = strlen(texto);
  for(; retorno < tamTexto; (linhaAtual += 1, retorno += 1))
  {
    if(linhaAtual == numeroLinha)
      return retorno;
    for(; texto[retorno] != '\n' && texto[retorno] != '\0'; retorno += 1);
  }
  return -1;  
}

static size_t finalLinha(const char* texto, size_t numeroLinha) //retorna o /n ou /0 que termina a linha
{
  size_t retorno = inicioLinha(texto, numeroLinha);
  if(retorno == -1)
    return -1;
  for(; texto[retorno] != '\n' && texto[retorno] != '\0'; retorno += 1);
  return retorno;
}

Processo* prcsInit(char* argumentos)
{
  if(argumentos == NULL)
    return NULL;
  Processo* retorno = malloc(sizeof(Processo));
  if(retorno == NULL)
    return NULL;
  retorno->valor    = 0;
  retorno->comandos = argumentos;
  return retorno;
}

char* prcsLer(Processo* objeto, size_t indiceComando)
{
  if(objeto == NULL)
    return NULL;
  size_t varInicioLinha = inicioLinha(objeto->comandos, indiceComando);
  size_t varFimLinha    = finalLinha(objeto->comandos, indiceComando);
  if(varInicioLinha == -1)
    return NULL;
  char* retorno = malloc(varFimLinha - varInicioLinha + 1);
  if(retorno == NULL)
    return NULL;
  retorno[varFimLinha - varInicioLinha] = '\0';
  for(size_t i = varInicioLinha, j = 0; i < varFimLinha; i += 1, j += 1)
    retorno[j] = objeto->comandos[i];
  return retorno;
}

void prcsDesaloca(Processo* objeto)
{
  if(objeto == NULL)
    return;
  free(objeto->comandos);
  free(objeto);
}