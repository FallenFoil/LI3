#include "db_map.h"


static guint char_hash(gconstpointer key){
    return (guint) ptr2value(char, key);
}

static gboolean char_equal(gconstpointer a, gconstpointer b){
    return ptr2value(char, a) == ptr2value(char, b);
}

GHashTable* hashNew(Type type, void (*freeValueFunc)(gpointer)){
    switch (type) {
        case TChar:
            return g_hash_table_new_full(char_hash, char_equal, g_free, freeValueFunc);
        case TInt:
            return g_hash_table_new_full(g_int_hash, g_int_equal, g_free, freeValueFunc);
        case TLong:
            return g_hash_table_new_full(g_int64_hash, g_int64_equal, g_free, freeValueFunc);
        case TDouble:
            return g_hash_table_new_full(g_double_hash, g_double_equal, g_free, freeValueFunc);
        case TString:
            return g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeValueFunc);
        case TDate:
            return g_hash_table_new_full(g_int64_hash, g_int64_equal, g_free, freeValueFunc);
        case TList:
            return g_hash_table_new_full(g_direct_hash, g_direct_equal, g_free, freeValueFunc);
        case TPointer:
            return g_hash_table_new_full(g_direct_hash, g_direct_equal, g_free, freeValueFunc);
        default:
            return g_hash_table_new_full(NULL, NULL, g_free, freeValueFunc);
    }
}

/**
  \brief Duplica a key
  \param key Chave
  \param type Tipo da chave
  \return endereço da nova key
*/
gpointer keydup(gconstpointer key, Type type){
    switch (type) {
        case TChar:
            return g_memdup(key, sizeof(char));
        case TInt:
            return g_memdup(key, sizeof(int));
        case TLong:
            return g_memdup(key, sizeof(long));
        case TDouble:
            return g_memdup(key, sizeof(double));
        case TString:
            return g_strdup(key);
        case TDate:
            return g_memdup(key, sizeof(long));
        case TList:
            return g_memdup(key, sizeof(gpointer));
        case TPointer:
            return g_memdup(key, sizeof(gpointer));
        default:
            return g_memdup(key, sizeof(gpointer));
    }
}

/**
  \brief Inicializa um Map de Row.
  \param type Tipo dos dados da key.
  \return Endereço de um Map de Row's.
*/
TRowMap tNewMap(Type type){
    TRowMap new = g_new(struct Map, 1);
    new->map = hashNew(type, NULL);
    new->type = type;
    return new;
}

/**
  \brief Agrupa a row com outras rows com o mesmo valor.
  \param map Endereço do Map.
  \param key Chave que entiquetada a Row.
  \param row Endereço da row a ser adicionada.
  \return TRUE se já existir uma row com a mesma etiqueta, FALSE se não existir.
  Ao inserir, se já existir Row com a mesma etiqueta, substitui o valor da row.
*/
gboolean tMapSetRowByKey(TRowMap map, gconstpointer key, TRow row){
    gpointer newKey = keydup(key, map->type);
    return g_hash_table_insert(map->map, (gpointer) newKey, row);
}

/**
  \brief Agrupa a row com outras rows com o mesmo valor.
  \param map Endereço do Map.
  \param colId Coluna a agrupar.
  \param row Endereço da row a ser adicionada.
  \return TRUE se já existir uma row com a mesma etiqueta, FALSE se não existir.
  Ao inserir, se já existir Row com a mesma etiqueta, substitui o valor da row.
*/
gboolean tMapSetRowByCol(TRowMap map, guint colId, TRow row){
    gpointer newKey = keydup(row->data[colId], map->type);
    return g_hash_table_insert(map->map, newKey, row);
}

/**
  \brief Retorna a primeira Row com a key.
  \param map Endereço do Map de rows.
  \param key Chave para encontrar as row.
  \return Endereço da Row entiquetada com a Chave. NULL se não encontrar.
*/
TRow tMapGetRow(TRowMap map, gconstpointer key){
    return (TRow) g_hash_table_lookup(map->map, key);
}

/**
  \brief Apaga o Map.
  \param map Map a ser apagado.
*/
void tMapDelete(TRowMap map){
    g_hash_table_destroy(map->map);
    g_free(map);
}

static void mapListFree(gpointer array){
    g_ptr_array_free((GPtrArray*) array, TRUE);
}

/**
  \brief Inicializa um Map de Row's.
  \param type Tipo dos dados da key.
  \return Endereço de um Map de Row's.
*/
TRowMapList tNewMapList(Type type){
    TRowMap new = g_new(struct Map, 1);
    new->map = hashNew(type, mapListFree);
    new->type = type;
    return new;
}

/**
  \brief Agrupa a row com outras rows com a mesma chave.
  \param map Endereço do Map.
  \param key Endereço da chave.
  \param row Endereço da row a ser adicionada.
  Não usar nas colunas de Id.
*/
void tMapListAddRowByKey(TRowMapList map, gconstpointer key, TRow row){
    gpointer newKey = keydup(key, map->type);
    GPtrArray* array = (GPtrArray*) g_hash_table_lookup(map->map, newKey);
    if(!array) {
        array = g_ptr_array_new();
        g_hash_table_insert(map->map, (gpointer) newKey, array);
    }
    g_ptr_array_add(array, (gpointer) row);
}

/**
  \brief Agrupa a row com outras rows com o mesmo valor.
  \param map Endereço do Map.
  \param colId Coluna a agrupar.
  \param row Endereço da row a ser adicionada.
  Não usar nas colunas de Id.
*/
void tMapListAddRowByCol(TRowMapList map, guint colId, TRow row){
    gpointer newKey = keydup(row->data[colId], map->type);
    tMapListAddRowByKey(map, newKey, row);
}

/**
  \brief Insere a row na lista.
  \param row Endereço da Row a ser adicionada à lista.
  \param listPtr Endereço da Row List onde vai ser adiciionado a row.
*/
static void copyGPtrArray(gpointer row, gpointer listPtr){
    TRowList list = (TRowList) listPtr;
    tListAdd(list, (TRow) row);
}

/**
  \brief Retorna as Row's com a mesma key.
  \param map Endereço do Map de rows.
  \param key Chave para encontrar as rows.
  \return Endereço de uma lista de Row's. NULL se não encontrar.
*/
TRowList tMapListGetRows(TRowMapList map, gconstpointer key){
    TRowList new = tListNew();
    GPtrArray* result = (GPtrArray*) g_hash_table_lookup(map->map, key);
    if(!result) {
        return NULL;
    }
    g_ptr_array_foreach(result, copyGPtrArray, new);
    return new;
}

/**
  \brief Apaga o Map List.
  \param map Map List a ser apagado.
*/
void tMapListDelete(TRowMapList map){
    g_hash_table_destroy(map->map);
    g_free(map);
}
