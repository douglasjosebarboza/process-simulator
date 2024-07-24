#ifndef LISTAINT_H
#define LISTAINT_H

#include <sys/types.h>
#include <stdbool.h>

typedef struct
{
  size_t *ponteiro;
  size_t capacidade;
  size_t quantidade;
} ListaInt;

ListaInt *lintInit();
bool lintInsere(ListaInt *lista, size_t valor);
bool lintRemove(ListaInt *lista, size_t indice);
size_t *lintAcessa(ListaInt *lista, size_t indice);
void lintDesaloca(ListaInt *lista);

#endif // LISTAINT_H

/*
Possui os seguintes dados:
  ponteiro
*/