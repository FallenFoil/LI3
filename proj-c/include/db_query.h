#ifndef DBQUERY
#define DBQUERY

#include "database.h"

/**
  \brief Estrutura que guarda condições de pesquisa.
*/
struct Query {
  guint colId;        //<! Número da coluna
  Operator opr;       //<! Operação
  gconstpointer value;//<! Valor a ser comparado
};

TQuery tNewQuery(guint colId, Operator opr, gconstpointer value);
void tDeleteQuery(TQuery query);

#endif