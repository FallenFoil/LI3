#include "db_table.h"

/**
  \brief Inicializa uma tabela.
  \param colTypes Array com os tipos das colunas.
  \param numCol Tamanho do arFray colTypes e o número de colunas.
  \return Endereço da struct Table.
*/
TTable tNew(const Type colTypes[], guint numCol){
    TTable table = (TTable) g_new(struct Table, 1);
    TColumn columns = (TColumn) g_new(struct Column, numCol);
    table->columns = columns;
    table->numCol = numCol;
    for(int i = 0; i < numCol; i++) {
        columns[i].indices = g_sequence_new(NULL);
        columns[i].type = colTypes[i];
    }
    return table;
}

/**
  \brief Apaga uma tabela.
  \param table Endereço da tabela.
*/
void tDelete(TTable table){
    for(int i = 0; i < table->numCol; i++){
        g_sequence_free(table->columns[i].indices);
    }
    g_free(table->columns);
    g_free(table);
}
