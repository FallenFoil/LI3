#ifndef DBMAP
#define DBMAP

#include "database.h"
#include "db_row.h"

/**
  \brief Estrutura que guarda um map de row's.
*/
struct Map {
  GHashTable* map; //!< Map de Row's.
  Type type;       //!< Tipo da key.
};

GHashTable* hashNew(Type type, void (*freeValueFunc)(gpointer));
gpointer keydup(gconstpointer key, Type type);
TRowMap tNewMap(Type type);
gboolean tMapSetRowByKey(TRowMap map, gconstpointer key, TRow row);
gboolean tMapSetRowByCol(TRowMap map, guint colId, TRow row);
TRow tMapGetRow(TRowMap map, gconstpointer key);
void tMapDelete(TRowMap map);
TRowMapList tNewMapList(Type type);
void tMapListAddRowByKey(TRowMapList map, gconstpointer key, TRow row);
void tMapListAddRowByCol(TRowMapList map, guint colId, TRow row);
TRowList tMapListGetRows(TRowMapList map, gconstpointer key);
void tMapListDelete(TRowMapList map);

#endif