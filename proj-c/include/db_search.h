#ifndef DBSEARCH
#define DBSEARCH
/**
  \brief Estrutura que guarda informação sobre as pesquisas.
  Esta estrutura de dados é normalmente passada para a função cmpFunc como user_data.
*/
typedef struct Search {
    guint column;         //!< Id da coluna a pesquisar.
    Operator operation;   //!< Operação de pesquisa.
    Type type;            //!< Tipo do dado.
} TSearch;

/**
  \brief Inicializa uma variável struct Search.
  \param SEARCH Nome da variável do tipo struct Search.
  \param COLUMN Id da coluna.
  \param OPERATION Operação.
  \param TYPE Tipo dos dados.
*/
#define searchSetup(SEARCH, COLUMN, OPERATION, TYPE) \
  (SEARCH).column = COLUMN; \
  (SEARCH).operation = OPERATION; \
  (SEARCH).type = TYPE

#endif