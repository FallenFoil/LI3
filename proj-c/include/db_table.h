#ifndef DBTABLE
#define DBTABLE

#include "db_row.h"
#include "database.h"

/**
  \brief Estrutura que guarda os apontadores para as Column's.
  A Table guarda um Array com as colunas.
  Uma Table é criada usando o tableNew.
  A Table pode ser ordenada usando tableSort.
*/
struct Table {
    struct Column* columns; //!< Array de Column's da tabela. O array está indexado pelo id das colunas.
    guint numCol;           //!< Número de colunas da tabela.
};

TTable tNew(const Type colTypes[], guint numCol);
void tDelete(TTable table);

#endif