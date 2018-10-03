#include "db_list.h"

/**
  \brief Inicializa uma lista de Rows.
  \param arr Endereço do array.
  \param len Tamanho do array.
  \return Endereço de uma lista de Rows.
*/
TRowList tListNew(){
    TRowList new = g_new(struct RowList, 1);
    new->array = g_ptr_array_new();
    return new;
}

TRow tListPosition(TRowList list, guint pos){
    if(!list || pos >= list->array->len){
        return NULL;
    } else {
        return g_ptr_array_index(list->array, pos);
    }
}

void tListAdd(TRowList list, TRow row){
    g_ptr_array_add(list->array, (gpointer) row);
}

/**
  \brief Retorna o tamanho da lista de Rows.
  \param list lista de Row's.
  \return Tamanho da lista de Row's.
*/
unsigned int tListLength(TRowList list){
    return list ? list->array->len : 0;
}

void tListSort(TRowList l, gint (*cmpFunc)(gconstpointer, gconstpointer)){
    if(l == 0) return;
    g_ptr_array_sort (l->array, cmpFunc);
}

void tListSortWithData(TRowList l, gint (*cmpFunc)(gconstpointer, gconstpointer, gpointer), gpointer user_data){
    g_ptr_array_sort_with_data (l->array, cmpFunc, user_data);
}
/**
  \brief Apaga a TRowList.
  \param list Endereço da lista de Rows.
*/
void tListDelete(TRowList list){
    if(list) g_ptr_array_free(list->array, TRUE);
}
