#include "db_query.h"

/**
  \brief Inicializa uma query.
  \param colId Id da coluna.
  \param opr Operação de pesquisa.
  \param value Valor a ser comparado.
  \return Endereço da query.
*/
TQuery tNewQuery(guint colId, Operator opr, gconstpointer value){
    TQuery new = g_new(struct Query, 1);
    new->colId = colId;
    new->opr = opr;
    new->value = value;
    return new;
}

/**
  \brief Apaga a query.
  \param query Endereço da query.
*/
void tDeleteQuery(TQuery query){
    g_free(query);
}
