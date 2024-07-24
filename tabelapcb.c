#include "tabelapcb.h"
#include "entradapcb.h"
#include <stdbool.h>
#include <stdlib.h>

static bool pcbDobraCapacidade(TabelaPCB *tabela)
{
    size_t novaCapacidade = tabela->capacidade * 2;
    EntradaPCB **novoPonteiro = realloc(tabela->ponteiro, sizeof(EntradaPCB *) * novaCapacidade);
    if (novoPonteiro == NULL)
        return false;
    tabela->ponteiro = novoPonteiro;
    tabela->capacidade = novaCapacidade;
    return true;
}

TabelaPCB *pcbInit()
{
    TabelaPCB *retorno = malloc(sizeof(TabelaPCB));
    if (retorno == NULL)
        return NULL;
    retorno->ponteiro = malloc(sizeof(EntradaPCB *) * 10);
    if (retorno->ponteiro == NULL)
    {
        free(retorno);
        return NULL;
    }
    retorno->capacidade = 10;
    retorno->quantidade = 0;
    return retorno;
}

bool pcbInsere(TabelaPCB *tabela, EntradaPCB *entrada)
{
    if (tabela == NULL || entrada == NULL)
        return NULL;
    if (tabela->capacidade <= tabela->quantidade + 1)
        if (pcbDobraCapacidade(tabela) == false)
            return false;
    tabela->ponteiro[tabela->quantidade] = entrada;
    tabela->quantidade += 1;
    return true;
}

bool pcbRemove(TabelaPCB *tabela, size_t indice)
{
    if (tabela == NULL || indice < 0 || indice >= tabela->quantidade)
        return false;
    epbcDesaloca(tabela->ponteiro[indice]);
    for (; indice < tabela->quantidade - 1; indice += 1)
        tabela->ponteiro[indice] = tabela->ponteiro[indice + 1];
    tabela->quantidade -= 1;
    return true;
}

EntradaPCB *pcbAcessa(TabelaPCB *tabela, size_t indice)
{
    return (tabela == NULL || indice < 0 || indice >= tabela->quantidade) ? NULL : tabela->ponteiro[indice];
}

void pcbDesaloca(TabelaPCB *tabela)
{
    if (tabela == NULL)
        return;
    for (size_t i = 0; i < tabela->quantidade; i += 1)
        epbcDesaloca(pcbAcessa(tabela, i));
    free(tabela->ponteiro);
    free(tabela);
}