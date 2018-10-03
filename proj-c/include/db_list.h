#ifndef DBLIST
#define DBLIST

#include "database.h"

struct RowList {
    GPtrArray* array;
};

TRowList tListNew();
TRow tListPosition(TRowList list, guint pos);
void tListAdd(TRowList list, TRow row);
unsigned int tListLength(TRowList list);
void tListSort(TRowList l, gint (*cmpFunc)(gconstpointer, gconstpointer));
void tListSortWithData(TRowList l, gint (*cmpFunc)(gconstpointer, gconstpointer, gpointer), gpointer user_data);
void tListDelete(TRowList list);

#endif