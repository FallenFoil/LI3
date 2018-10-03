#include "db_row.h"


#define OSortRight -1
#define OSortLeft  -2

/**
  \brief Compara dois apontadores e indica qual deles é NULL.
  \param dataA apontador para a informação.
  \param dataB apontador para a informação.
  \return 0 se forem ambos NULL, -1 se o dataA é NULL e 1 se o dataB é NULL.
*/
static gint compareNull(gconstpointer dataA, gconstpointer dataB, Type type){
    gint r = 0;
    if(dataA) {
        r++;
    }
    if(dataB) {
        r--;
    }
    return r;
}

/**
  \brief Compara os valores de dois apontadores.
  \param type Tipo dos valores.
  \param dataA Apontador para a informação.
  \param dataB Apontador para a informação.
  \return 0 se forem ambos iguais, menor que 0 se dataA for menor que dataB e retorna maior que 0 se dataA for maior que dataB.
*/
static gint cmpDif(Type type, gconstpointer dataA, gconstpointer dataB){
    switch (type) {
        case TChar:
            return ptr2value(gchar, dataA) - ptr2value(gchar, dataB);
        case TInt:
            return ptr2value(gint, dataA) - ptr2value(gint, dataB);
        case TLong:
            return ptr2value(glong, dataA) - ptr2value(glong, dataB);
        case TDouble:
            return ptr2value(gdouble, dataA) - ptr2value(gdouble, dataB);
        case TString:
            return strcmp((gchar*) dataA, (gchar*) dataB);
        case TDate:
            return ptr2value(glong, dataA) - ptr2value(glong, dataB);
        case TList:
            return ((TRowList) dataA)->array->len - ((TRowList) dataB)->array->len;
        case TPointer:
            return dataA - dataB;
        default:
            return 0;
    }
}

/**
  \brief Verifica se a operação é verdade quando comparando com 0.
  \param opr Operação a verificar.
  \param r Diferença entre dois valores.
  \return 0 se a operação for verdade, outro valor caso contrário.
*/
static gint cmpOpr(gint opr, gint r){
    switch (opr) {
        case OSortRight:
            return r > 0 ? 1 : -1;
        case OSortLeft:
            return r >= 0 ? 1 : -1;
        case OTrue:
            return 0;
        case OLess:
            return r < 0 ? 0 : 1;
        case OLessEqual:
            return r <= 0 ? 0 : 1;
        case OEqual:
            return r == 0 ? 0 : r;
        case OGreaterEqual:
            return r >= 0 ? 0 : -1;
        case OGreater:
            return r > 0 ? 0 : -1;
        case OUnequal:
            return r != 0 ? 0 : 1;
        default:
            return r;
    }
}

/**
  \brief Compara dois apontadores para Row's.
  \param a Apontador para uma Row já inserida.
  \param b Apontador para uma Row ainda por inserir / Row com valores a comparar.
  \param user_data Apontador para a struct Search com as definições da pesquisa.
  \return 0 se a operação satifazer, menor que 0 se a for menor que b, maior que 0 se a for maior que b.
*/
static gint cmpFunc(gconstpointer a, gconstpointer b, gpointer user_data){
    TSearch* search = (TSearch*) user_data;
    TRow rowA = (TRow) a;
    TRow rowB = (TRow) b;
    gint r = 0;
    gconstpointer dataA = rowA->data[search->column];
    gconstpointer dataB = rowB->data[search->column];
    if(dataA && dataB) {
        r = cmpDif(search->type, dataA, dataB);
        r = cmpOpr(search->operation, r);
    } else {
        if(search->operation == OSortLeft || search->operation == OSortRight || search->operation == OEqual || search->operation == OUnequal){
            r = compareNull(rowA, rowB, search->type);
        } else {
            r = 0;
        }
    }
    return r;
}

/**
  \brief Ordena uma coluna.
  \param table Endereço da tabela a ordenar.
  \param colId Id da coluna a ordenar.
*/
void tColumnSort(TTable table, guint colId){
    TSearch search;
    searchSetup(search, colId, OSortRight, table->columns[colId].type);
    g_sequence_sort(table->columns[colId].indices, cmpFunc, &search);
}

/**
  \brief Preenche o array columns com apontadores para as Column's correspondentes aos Id's dados.
  \param[out] columns Array a preencher com os endereços das colunas.
  \param table Endereço da tabela à procura.
  \param colsId Array com os Id's das colunas.
  \param N Tamanho do array colsId.
*/
static void getColumns(TColumn columns[], TTable table, const guint colsId[], guint N){
    for(int i = 0; i < N; i++) {
        columns[i] = &table->columns[colsId[i]];
    }
}

/**
  \brief Insere na tabela
  \param table Endereço da tabela com a informação.
  \param colsId Array com os Id's a preencher da Row.
  \param values Array com os valores a preencher da Row.
  \param N Tamanho dos arrays colsId e values.
  \return Endereço da row criada.
*/
TRow tInsert(TTable table, const guint colsId[], const gpointer values[], guint N){
    TColumn columns[N];
    TRow row = g_new0(struct Row, 1);
    getColumns(columns, table, colsId, N);
    row->data = (gconstpointer*) g_malloc0_n(table->numCol, sizeof(gconstpointer));
    for(int i = 0; i < N; i++) {
        row->data[colsId[i]] = values[i];
        row->iters[colsId[i]] = g_sequence_append(columns[i]->indices, row);
    }
    return row;
}

/**
  \brief Inicializa uma Row.
  \param table Tabela que a Row irá pertencer.
*/
TRow tCreateRow(TTable table) {
    TRow row = g_new0(struct Row, 1);
    row->iters = g_malloc0_n(sizeof(GSequenceIter*), table->numCol);
    row->data = (gconstpointer*) g_malloc0_n(table->numCol, sizeof(gconstpointer));
    return row;
}

/**
  \brief Altera o valor da Row.
  \param colId Id da coluna a ser alterada na Row.
  \param value Valor a ser alterado na Row.
*/
void tUpdateRow(TRow row, guint colId, gconstpointer value) {
    row->data[colId] = value;
}

/**
  \brief Adiciona a Row aos índices da coluna.
  \param table Endereço da tabela.
  \param row Endereço da row não inserida na tabela.
  \param colId Id da coluna que vai ser indexada.
*/
void tInsertRow(TTable table, TRow row, guint colId) {
    row->iters[colId] = g_sequence_append(table->columns[colId].indices, row);
}

/**
  \brief Adiciona a Row aos índices da coluna.
  \param table Endereço da tabela.
  \param row Endereço da row não inserida na tabela.
  \param colsId Array com os Id's da coluna que vai ser indexada.
  \param N tamanho do array colsId.
*/
void tInsertMultipleRow(TTable table, TRow row, guint colsId[], guint N) {
    for(int i = 0; i < N; i++){
        row->iters[colsId[i]] = g_sequence_append(table->columns[colsId[i]].indices, row);
    }
}

/**
  \brief Apaga uma row.
  \param row Row a ser apagada.
*/
void tDeleteRow(TRow row) {
    g_free(row->iters);
    g_free(row->data);
    g_free(row);
}

/**
  \brief Verifica se cumpre todas as condições.
  \param querys Array com as condições.
  \param N Tamanho do array querys.
  \param types Array com os tipos dos dados.
  \param numCol Número de colunas da tabela.
  \param row Row que se pretende validar
  \return TRUE se cumprir todas as condições. FALSE caso contrário.
*/
static gboolean validate(TQuery querys[],
                         guint N,
                         Type types[],
                         guint numCol,
                         TRow row){
    struct Row rowValue;
    gconstpointer data[numCol];
    rowValue.data = data;
    TSearch searchValue;
    gboolean success = TRUE;
    for(int i = 0; success && i < N; i++){
        searchSetup(searchValue, querys[i]->colId, querys[i]->opr, types[i]);
        rowValue.data[querys[i]->colId] = (gpointer) querys[i]->value;
        success &= (cmpFunc(row, &rowValue, &searchValue) == 0);
    }
    return success;
}

/**
  \brief Itera e aplica a função f a todas as rows dentro de um intervalo que cumpram as condições.
  \param table Endereço da table.
  \param next Função que dado o iterador atual, dá o próximo iterador.
  \param range Array de dois elementos. Na posição 0 o iterador inicial e na posição 1 o iterador final.
  \param querys Array com as condições.
  \param N Tamanho do array querys.
  \param f Função aplicada a todas as rows que cumpram as condições.
  \param userData Parametro passado à função f.
*/
static void iterateValidate(TTable table,
                            GSequenceIter* (*next)(GSequenceIter*),
                            GSequenceIter* range[2],
                            TQuery querys[],
                            guint N,
                            gboolean (*f)(TRow, gconstpointer),
                            gconstpointer userData){
    TRow row;
    guint numCol = table->numCol;
    GSequenceIter* iter = range[0];
    GSequenceIter* endIter = range[1];
    GSequenceIter* oldIter = NULL;
    Type types[N];

    for(gint i = 0; i < N; i++){
        types[i] = table->columns[querys[i]->colId].type;
    }

    while(iter && !g_sequence_iter_is_end(iter) && iter != endIter && iter != oldIter) {
        row = g_sequence_get(iter);
        if(validate(querys, N, types, numCol, row)){
            if(f(row, userData)){
                return;
            }
        }
        oldIter = iter; //Previne loops infinitos por causa dos iteradores do glib
        iter = next(iter);
    }
    return;
}

/**
  \brief Procura o primeiro iterador dentro de um intervalo.
  \param table Endereço da table.
  \param colId Id da coluna a ordenar.
  \param range Array de dois elementos. Na posição 0 o mínimo e na posição 1 o máximo.
  \param equals Array de dois elementos.
                Na posição 0 é TRUE se o mínimo está incluído no intervalo, FALSE caso contrário.
                Na posição 1 é TRUE se o máximo está incluído no intervalo, FALSE caso contrário.
  \param sort >= 0 para crescente, < 0 para decrescente.
  \return Endereço do iterador. NULL se for o iterador inicial.
  A pesquisa é feita na àrvore da coluna colId.
*/
static GSequenceIter* getIterRangeBegin(TTable table,
                                        guint colId,
                                        gconstpointer range[2],
                                        gboolean equals[2],
                                        gint sort){
    TSearch searchIndex;
    struct Row rowIndex;
    gconstpointer data[table->numCol];
    rowIndex.data = data;
    GSequence* indices = table->columns[colId].indices;
    Type type = table->columns[colId].type;
    GSequenceIter* iter;
    if(sort >= 0){ //Crescente
        if(range[0] == NULL){
            return g_sequence_get_begin_iter(indices);
        }
        rowIndex.data[colId] = range[0]; //Mínimo
        if(equals[0]){
            searchSetup(searchIndex, colId, OSortLeft, type);
        } else {
            searchSetup(searchIndex, colId, OSortRight, type);
        }
        iter = g_sequence_search(indices, &rowIndex, cmpFunc, &searchIndex);

    } else { //Decrescente
        if(range[1] == NULL){
            return g_sequence_iter_prev(g_sequence_get_end_iter(indices));
        }
        rowIndex.data[colId] = range[1]; //Máximo
        if(equals[1]){
            searchSetup(searchIndex, colId, OSortRight, type);
        } else {
            searchSetup(searchIndex, colId, OSortLeft, type);
        }
        iter = g_sequence_search(indices, &rowIndex, cmpFunc, &searchIndex);
        iter = g_sequence_iter_is_begin(iter) ? NULL : g_sequence_iter_prev(iter);
    }
    return iter;
}

/**
  \brief Procura o último iterador dentro de um intervalo.
  \param table Endereço da table.
  \param colId Id da coluna a ordenar.
  \param range Array de dois elementos. Na posição 0 o mínimo e na posição 1 o máximo.
  \param equals Array de dois elementos.
                Na posição 0 é TRUE se o mínimo está incluído no intervalo, FALSE caso contrário.
                Na posição 1 é TRUE se o máximo está incluído no intervalo, FALSE caso contrário.
  \param sort >= 0 para crescente, < 0 para decrescente.
  \return Endereço do iterador. NULL se for o iterador inicial.
  A pesquisa é feita na àrvore da coluna colId.
*/
static GSequenceIter* getIterRangeEnd(TTable table,
                                      guint colId,
                                      gconstpointer range[2],
                                      gboolean equals[2],
                                      gint sort){
    TSearch searchIndex;
    struct Row rowIndex;
    gconstpointer data[table->numCol];
    rowIndex.data = data;
    GSequence* indices = table->columns[colId].indices;
    Type type = table->columns[colId].type;
    GSequenceIter* iter;

    if(sort >= 0){ //Crescente
        if(range[1] == NULL){
            return NULL;
        }
        rowIndex.data[colId] = range[1]; //Máximo
        if(equals[1]){
            searchSetup(searchIndex, colId, OSortRight, type);
        } else {
            searchSetup(searchIndex, colId, OSortLeft, type);
        }
        iter = g_sequence_search(indices, &rowIndex, cmpFunc, &searchIndex);

    } else { //Decrescente
        if(range[0] == NULL){
            return NULL;
        }
        rowIndex.data[colId] = range[0]; //Mínimo
        if(equals[0]){
            searchSetup(searchIndex, colId, OSortLeft, type);
        } else {
            searchSetup(searchIndex, colId, OSortRight, type);
        }
        iter = g_sequence_search(indices, &rowIndex, cmpFunc, &searchIndex);
        iter = g_sequence_iter_is_begin(iter) ? NULL : g_sequence_iter_prev(iter);
    }
    return iter;
}

/**
  \brief Aplica a função f a todas as rows dentro de um intervalo que cumpram as condições.
  \param table Endereço da table.
  \param colId Id da coluna a ordenar.
  \param range Array de dois elementos. Na posição 0 o mínimo e na posição 1 o máximo.
  \param equals Array de dois elementos.
                Na posição 0 é TRUE se o mínimo está incluído no intervalo, FALSE caso contrário.
                Na posição 1 é TRUE se o máximo está incluído no intervalo, FALSE caso contrário.
  \param querys Array com as condições.
  \param N Tamanho do array querys.
  \param sort >= 0 para crescente, < 0 para decrescente.
  \param f Função aplicada a todas as rows que cumpram as condições.
  \param userData Parametro passado à função f.

  A pesquisa é feita na àrvore da coluna colId.
  Procura o o primeiro e último iterador dentro de um intervalo.
  Aplica a função iterateValidate que itera pelas rows que estão nesse intervalo.
*/
static void tApplyRowsRange(TTable table,
                            guint colId,
                            gconstpointer range[2],
                            gboolean equals[2],
                            TQuery querys[],
                            guint N,
                            gint sort,
                            gboolean (*f)(TRow, gconstpointer),
                            gconstpointer userData){
    GSequenceIter* (*next)(GSequenceIter*);
    GSequenceIter* rangeIter[2];

    if(g_sequence_is_empty(table->columns[colId].indices)) return; // Os índices estão vazios.
    if(sort >= 0){
        next = g_sequence_iter_next;
    } else {
        next = g_sequence_iter_prev;
    }

    rangeIter[0] = getIterRangeBegin(table, colId, range, equals, sort);
    rangeIter[1] = getIterRangeEnd(table, colId, range, equals, sort);

    iterateValidate(table, next, rangeIter, querys, N, f, userData);
}

/**
  \brief Atualiza o valor do OLD se OLD OPR NEW for verdade ou OLD == NULL
  \param TYPE   Tipo dos dados a comparar.
  \param OLD    Variável com o valor antigo.
  \param NEW    Variável com o valor.
  \param OPR    Operação binária de comparação com 0.
*/
#define updateRange(TYPE, OLD, NEW, OPR) \
    if(!OLD || (NEW && cmpDif(TYPE, OLD, NEW) OPR 0)) OLD = NEW

/**
  \brief Atualiza o valor do elemeto de indice 0 do array NEW se NEW > OLD
  \param TYPE   Tipo dos dados a comparar.
  \param OLD    Variável com o valor atual.
  \param NEW    Variável com o novo valor.
*/
#define updateRangeMIN(TYPE, OLD, NEW) updateRange(TYPE, OLD[0], NEW, <)

/**
  \brief Atualiza o valor do elemeto de indice 1 do array NEW se NEW < OLD
  \param TYPE   Tipo dos dados a comparar.
  \param OLD    Variável com o valor atual.
  \param NEW    Variável com o novo valor.
*/
#define updateRangeMAX(TYPE, OLD, NEW) updateRange(TYPE, OLD[1], NEW, >)

/**
  \brief Aplica a função f a todas as rows que cumpram as condições.
  \param table Endereço da tabela.
  \param querys Array com as condições.
  \param N Número de elementos do array querys.
  \param sort 0 para não ordenar (menos eficiente), >0 para ordenar por ordem crescente, <0 para ordenar por ordem decrescente.
  \param f Função aplicada a cada Row que cumpra as condições.
  \param userData Endereço de uma estrutura que é passada para a função f.
  \return TRUE se encontrar uma row que cumpra as condições, FALSE se não encontrar.
  A primeira coluna das querys é o indexador, por isso necessita de estar ordenada.
  As rows podem ser ordenadas pela primeira coluna das querys.
*/
void tApplyRows(TTable table, TQuery querys[], guint N, gint sort, gboolean (*f)(TRow, gconstpointer), gconstpointer userData) {
    guint indexColId = querys[0]->colId;
    Type type = table->columns[indexColId].type;
    gconstpointer range[2] = {NULL, NULL}; // range[0]  -> minimo. NULL = -Infinito
                                           // range[1]  -> máximo. NULL = +Infinito
    gboolean equals[2] = {FALSE, FALSE};   // equals[0] -> TRUE se >=, FALSE se >.
                                           // equals[1] -> TRUE se <=, FALSE se <.
    gboolean moreQueries = FALSE;          // TRUE se precisa de analisar mais querys.
    gint i = 0;

    if(N == 0 || querys == NULL){ // Não tem querys.
        return;
    }
    //Tem querys para analisar.
    do {
        switch (querys[i]->opr) {
            case OTrue:
                moreQueries = FALSE;
                break;

            case OLess:
                updateRangeMAX(type, range, querys[i]->value);
                i++; // Próxima query.
                moreQueries = TRUE;
                break;

            case OLessEqual:
                updateRangeMAX(type, range, querys[i]->value);
                equals[1] = TRUE;
                i++; // Próxima query.
                moreQueries = TRUE;
                break;

            case OEqual:
                updateRangeMIN(type, range, querys[i]->value);
                updateRangeMAX(type, range, querys[i]->value);
                equals[0] = TRUE;
                equals[1] = TRUE;
                i++; // Próxima query.
                moreQueries = FALSE;
                break;

            case OGreaterEqual:
                updateRangeMIN(type, range, querys[i]->value);
                equals[0] = TRUE;
                i++; // Próxima query.
                moreQueries = TRUE;
                break;

            case OGreater:
                updateRangeMIN(type, range, querys[i]->value);
                i++; // Próxima query.
                moreQueries = TRUE;
                break;

            case OUnequal:
                moreQueries = FALSE;
                break;

            default:
                return;
        }
    } while(i < N && moreQueries && querys[i]->colId == indexColId);
    tApplyRowsRange(table, indexColId, range, equals, querys + i, N - i, sort, f, userData);
}


static gboolean tGetRowFunc(TRow row, gconstpointer userData){
    ptr2value(TRow, userData) = row;
    return TRUE;
}
/**
  \brief Retorna a primeira row que encontrar que cumpre as condições.
  \param table Endereço da tabela.
  \param querys Array com as condições.
  \param N Número de elementos do array querys.
  \param sort 0 para não ordenar (menos eficiente), >0 para ordenar por ordem crescente, <0 para ordenar por ordem decrescente.
  \return Endereço da row encontrada ou NULL se não encontrar.
  A primeira coluna das querys é o indexador.
  As rows podem ser ordenadas pela primeira coluna das querys.
*/
TRow tGetRow(TTable table, TQuery querys[], guint N, gint sort) {
    TRow row = 0;
    tApplyRows(table, querys, N, sort, tGetRowFunc, &row);
    return row;
}

static gboolean tGetRowsFunc(TRow row, gconstpointer userData){
    g_ptr_array_add((GPtrArray*) userData, (gpointer) row);
    return FALSE;
}
/**
  \brief Retorna a primeira row que encontrar que cumpre as condições.
  \param table Endereço da tabela.
  \param querys Array com as condições.
  \param N Número de elementos do array querys.
  \param sort 0 para não ordenar (menos eficiente), >0 para ordenar por ordem crescente, <0 para ordenar por ordem decrescente.
  \return Endereço da row encontrada ou NULL se não encontrar.
  A primeira coluna das querys é o indexador.
  As rows podem ser ordenadas pela primeira coluna das querys.
*/
TRowList tGetRows(TTable table, TQuery querys[], guint N, gint sort) {
    TRowList list = tListNew();
    tApplyRows(table, querys, N, sort, tGetRowsFunc, list->array);
    return list;
}


TRow tFirstRow(TTable table, guint colId){
    GSequenceIter* iter = g_sequence_get_begin_iter(table->columns[colId].indices);
    iter = g_sequence_iter_next(iter);
    if(g_sequence_iter_is_end(iter)){
        return NULL;
    }
    return g_sequence_get(iter);
}

TRow tLastRow(TTable table, guint colId){
    GSequenceIter* iter = g_sequence_get_end_iter(table->columns[colId].indices);
    iter = g_sequence_iter_prev(iter);
    if(g_sequence_iter_is_begin(iter)){
        return NULL;
    }
    return g_sequence_get(iter);
}

TRow tNextRow(TRow row, guint colId){
    GSequenceIter* nextIter = g_sequence_iter_next(row->iters[colId]);
    if(g_sequence_iter_is_end(nextIter)){
        return NULL;
    }
    return g_sequence_get(nextIter);
}

TRow tPrevRow(TRow row, guint colId){
    GSequenceIter* prevIter = g_sequence_iter_prev(row->iters[colId]);
    if(g_sequence_iter_is_begin(prevIter)){
        return NULL;
    }
    return g_sequence_get(prevIter);
}

/**
  \brief Obtêm a informação de uma coluna da Row.
  \param row Row com a informação a ser obtida.
  \param colId Id da coluna a que se pretende obter a informação.
  \return Apontador para a data, NULL se a row for NULL.
  Usado normalmente com a macro ptr2value para converter o apontador para a informação guardada.
*/
gconstpointer tGetData(TRow row, guint colId){
    return row ? row->data[colId] : 0;
}
