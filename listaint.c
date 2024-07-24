#include "listaint.h"
#include <stdlib.h>

static bool lintDobraCapacidade(ListaInt *lista)
{
    size_t novaCapacidade = lista->capacidade * 2;
    size_t *novoPonteiro = realloc(lista->ponteiro, sizeof(size_t) * novaCapacidade);
    if (novoPonteiro == NULL)
        return false;
    lista->ponteiro = novoPonteiro;
    lista->capacidade = novaCapacidade;
    return true;
}

ListaInt *lintInit()
{
    ListaInt *retorno = malloc(sizeof(ListaInt));
    if (retorno == NULL)
        return NULL;
    retorno->ponteiro = malloc(sizeof(size_t) * 10);
    if (retorno->ponteiro == NULL)
    {
        free(retorno);
        return NULL;
    }
    retorno->capacidade = 10;
    retorno->quantidade = 0;
    return retorno;
}

bool lintInsere(ListaInt *lista, size_t valor)
{
    if (lista == NULL)
        return false;
    if (lista->capacidade <= lista->quantidade + 1)
        if (lintDobraCapacidade(lista) == false)
            return false;
    lista->ponteiro[lista->quantidade] = valor;
    lista->quantidade += 1;
    return true;
}

bool lintRemove(ListaInt *lista, size_t indice)
{
    if (lista == NULL || indice < 0 || indice >= lista->quantidade)
        return false;
    for (; indice < lista->quantidade - 1; indice += 1)
        lista->ponteiro[indice] = lista->ponteiro[indice + 1];
    lista->quantidade -= 1;
    return true;
}

size_t *lintAcessa(ListaInt *lista, size_t indice)
{
    return (lista == NULL || indice < 0 || indice >= lista->quantidade) ? NULL : lista->ponteiro + indice;
}

void lintDesaloca(ListaInt *lista)
{
    if (lista == NULL)
        return;
    free(lista->ponteiro);
    free(lista);
}