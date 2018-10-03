#ifndef DBROW
#define DBROW

#include "db_query.h"
#include "db_search.h"
#include "db_list.h"
#include "database.h"
#include "db_table.h"

/**
  \brief Estrutura que guarda a informação de uma Row.
  A Row guarda num array os apontadores para as informações que se pretende guardar da row.
  O array data é indexado pelos id's das colunas.
  Se o apontador for nulo é considerado que não existe informação a ser guardada para aquela coluna.
  As rows podem ser alteradas usando o tableUpdateRow.
  As rows podem ser inseridas na Table usando tableInserTRow.
*/
struct Row {
    GSequenceIter** iters;  //!< Array de apontadores para o iterador da ávore.
    gconstpointer* data;    //!< Array de apontadores para a da Row.
                            //!< O array está indexado pelos id's das colunas.
};

/**
  \brief Estrutura que guarda o seu tipo e o apontador para uma árvore AVL.
  A Column guarda numa árvore AVL das Row's e o seu tipo de dados.
  A árvore AVL pode estar ordenada pelos valores da coluna a que se refere.
  As colunas são inicializadas pela tableNew.
  Os índices são populados usando a função tableInserTRow e tableInsert.
  Os índices podem ser ordenados usando o tableColumnSort.
*/
typedef struct Column {
    GSequence* indices; //!< Apontador para a árvore AVL de apontadores para as Row's.
                        //!< Os apontadores para as Row's podem estar ordenados pelos valores desta coluna.
    Type type;          //!< Tipo dos dados da coluna.
}* TColumn;

void tColumnSort(TTable table, guint colId);
TRow tInsert(TTable table, const guint colsId[], const gpointer values[], guint N);
TRow tCreateRow(TTable table) ;
void tUpdateRow(TRow row, guint colId, gconstpointer value);
void tInsertRow(TTable table, TRow row, guint colId);
void tInsertMultipleRow(TTable table, TRow row, guint colsId[], guint N);
void tDeleteRow(TRow row);
void tApplyRows(TTable table, TQuery querys[], guint N, gint sort, gboolean (*f)(TRow, gconstpointer), gconstpointer userData);
TRow tGetRow(TTable table, TQuery querys[], guint N, gint sort);
TRowList tGetRows(TTable table, TQuery querys[], guint N, gint sort);
TRow tFirstRow(TTable table, guint colId);
TRow tLastRow(TTable table, guint colId);
TRow tNextRow(TRow row, guint colId);
TRow tPrevRow(TRow row, guint colId);
gconstpointer tGetData(TRow row, guint colId);

#endif