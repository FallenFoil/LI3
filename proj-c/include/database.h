#ifndef DATABASE
#define DATABASE
#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <string.h>

typedef enum Type {TChar = 0, TInt, TLong, TDouble, TString, TDate, TList, TPointer} Type;
typedef enum Operator {OTrue = 0, OLess, OLessEqual, OEqual, OGreaterEqual, OGreater, OUnequal} Operator;

typedef struct Row* TRow;
typedef struct Table* TTable;
typedef struct RowList* TRowList;
typedef struct Query* TQuery;
typedef struct Map* TRowMap;
typedef struct Map* TRowMapList;

/**
  \brief Retorna o o conteúdo do apontador.
  \param TYPE Tipo da variável.
  \param VALUE Nome da variável.
  \return Conteúdo do apontador.
*/
#define ptr2value(TYPE, VALUE) (*(TYPE*) VALUE)

/**
  \brief Retorna o endereço do valor.
  \param TYPE Tipo da variável.
  \param VALUE Nome da variável.
  \return Endereço do valor.
*/
#define value2ptr(TYPE, VALUE) (&(TYPE[]){VALUE})

TTable tNew(const Type colTypes[], unsigned int numCol);
void tDelete(TTable table);

TRow tCreateRow(TTable table);
void tUpdateRow(TRow row, unsigned int colId, const void* value);
void tInsertRow(TTable table, TRow row, unsigned int colId);
void tInsertMultipleRow(TTable table, TRow row, unsigned int colsId[], unsigned int N);
void tDeleteRow(TRow row);

void tColumnSort(TTable table, unsigned int colId);
void tSort(TTable table);

TQuery tNewQuery(unsigned int colId, Operator opr, const void* value);
void tDeleteQuery(TQuery query);

TRowList tListNew();
TRow tListPosition(TRowList list, unsigned int pos);
void tListAdd(TRowList list, TRow row);
unsigned int tListLength(TRowList list);
void tListSort(TRowList l, int (*cmpFunc)(const void*, const void*));
void tListSortWithData(TRowList l, int (*cmpFunc)(const void*, const void*, void*), void* user_data);
void tListForEach(TRowList list, void (*func)(void*, void*), void *user_data);
void tListDelete(TRowList list);

TRowMap tNewMap(Type type);
int tMapSetRowByKey(TRowMap map, const void* key, TRow row);
int tMapSetRowByCol(TRowMap map, unsigned int colId, TRow row);
TRow tMapGetRow(TRowMap map, const void* key);
void tMapDelete(TRowMap map);

TRowMapList tNewMapList(Type type);
void tMapListAddRowByKey(TRowMapList map, const void* key, TRow row);
void tMapListAddRowByCol(TRowMapList map, unsigned int colId, TRow row);
TRowList tMapListGetRows(TRowMapList map, const void* key);
void tMapListDelete(TRowMapList map);

TRow tGetRow(TTable table, TQuery querys[], unsigned int N, int sort);
TRow tGetRowEquals(TTable table, unsigned int colId, const void* value);
TRowList tGetRows(TTable table, TQuery querys[], unsigned int N, int sort);
unsigned int tCounTRows(TTable table, TQuery querys[], unsigned int N);
void tApplyRows(TTable table, TQuery querys[], unsigned int N, int sort, int (*f)(TRow, const void*), const void* userData);

TRow tFirstRow(TTable table, unsigned int colId);
TRow tLastRow(TTable table, unsigned int colId);
TRow tNextRow(TRow row, unsigned int colId);
TRow tPrevRow(TRow row, unsigned int colId);

const void* tGetData(TRow row, unsigned int colId);
const void* tGetDataEquals(TTable table, unsigned int dataColId, unsigned int searchColId, const void* value);

#endif
