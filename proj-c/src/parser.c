#include "parser.h"

#define N_USERS		4

#define QST 	tables[0]
#define ANS 	tables[1]

/**
  \brief Converte uma string para o tipo indicado e guarda na memoria

  \param str String a converter
  \param type Tipo para qual a string deve ser covertida

  \return Endereço onde está guardado
*/
static void* convertString (char* str, Type type){
	int* i;
	long* l;
	char* s;
	long* dt;
	int len;
	switch (type) {
	    case TInt:
	    	i = malloc(sizeof(int));
	    	*i = (int) strtol(str, NULL, 10);
	    	return i;

	    case TLong:
	    	l = malloc(sizeof(long));
	    	*l = strtol(str, NULL, 10);
	    	return l;

	    case TString:
	    	len = strlen(str) + 1;
	    	s = malloc(len * sizeof(char));
	    	strcpy(s, str);
	    	return s;

	    case TDate: ;
			int y,m,d;
			sscanf(str, "%d-%d-%d", &y, &m, &d);
			dt = malloc(sizeof(long));
			*dt = (y << 9) | (m << 5) | d;
	    	return dt;

	    default:
	        return 0;
  }

}


/**
  \brief Guarda um atributo numa row

  \param name 	Nome do atributo
  \param type 	Tipo do atributo
  \param col 	Coluna da Row onde vai ser guardado
  \param row 	Row onde vai ser guardado
  \param cur 	Apontador para o node xml onde está o atributo
*/
static void parseAttr(char *name, Type type, int col, TRow row, xmlNodePtr cur){
	xmlChar *attr = xmlGetProp(cur,(xmlChar*) name);
	if(attr){
		tUpdateRow(row, col, convertString((char *)attr, type));
		xmlFree(attr);
	}
}

/**
  \brief Da parse a um ficheiro Users.xml e guarda as informações num Map e Table

	Estrutura das rows na Table:
	Id,    Reputation, DisplayName, AboutMe, Posts
	TLong, TInt,       TString,     TString, TList
	a coluna posts é preenchida na função parsePosts

	As rows podem ser pesquisadas/percorridas ordenadamente
	por Reputation e Posts (tamanho do array, depois de ser preenchida pela parsePosts)

  \param dump_path 	Path para onde está o ficheiro  Users.xml
  \param map 		Map que liga id -> Users

  \return 			Table com informações dos users
*/
TTable parseUsers(char* dump_path, TRowMap map) {

	char docname[256];
	strcpy(docname, dump_path);
	strcat(docname, "/Users.xml");

	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(docname);

	if (doc == NULL ) {
		fprintf(stderr,"%s not parsed successfully. \n", docname);
		return NULL;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"%s is empty.\n", docname);
		xmlFreeDoc(doc);
		return NULL;
	}

	Type types[N_USERS + 1] = {TLong, TInt, TString, TString, TList};
	char* atts[N_USERS] = {"Id", "Reputation", "DisplayName", "AboutMe"};
	TTable table = tNew(types, N_USERS + 1);

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
	    if ((!xmlStrcmp(cur->name, (const xmlChar*)"row"))) {

	    	TRow user = tCreateRow(table);

	    	for(int i = 0; i < N_USERS; i++){
	    		parseAttr(atts[i], types[i], i, user, cur);
			}

			tMapSetRowByCol(map, UId, user);

			tInsertRow(table, user, UReputation);

 	    }
	cur = cur->next;
	}

	tColumnSort(table, UReputation);

	xmlFreeDoc(doc);

	return table;
}



/**
  \brief Estrutura que guarda o id de uma pergunta e uma lista de rows de respostas
  É usada para guardar respostas que estão à espera que a sua pergunta seja encontrada
*/
typedef struct PPair {
	long parentId;		//!< Id da pergunta
    GSList* answers;	//!< Lista de rows de respostas
} *p_pair;

/**
  \brief Cria um ppair com o id de uma pergunta e uma row de uma resposta

  \param parentId	Id da pergunta
  \param answer 	Row da resposta

  \return Apontador para o ppair
*/
static p_pair newPPair(long parentId, TRow answer){
	p_pair pair = malloc(sizeof(struct PPair));
	pair->parentId = parentId;
	GSList* l = g_slist_prepend(NULL, answer);
	pair->answers = l;
	return pair;
}

/**
  \brief Liberta a memoria ocupada por um ppair

  \param pair 	Apontador para o ppair
*/
static void destroyPPair(p_pair pair){
	g_slist_free(pair->answers);
	free(pair);
}

/**
  \brief Adiciona uma resposta a uma lista de ppair.
  Se já existir um ppair com a mesma pergunta, adiciona a resposta a esse ppair, senão cria um novo e adiciona esse À lista

  \param id		Id da pergunta
  \param answer	Row da resposta
  \param list 	Apontador para a lista de ppair

  \return 		Apontador para a lista de ppair
*/
static GSList* addPPair(long id, TRow answer, GSList* list){
	GSList* l;
	p_pair p;
	for(l = list; l; l = l->next){
		p = l->data;
		if(p->parentId == id){
			p->answers = g_slist_prepend(p->answers, answer);
			return list;
		}
	}
	return g_slist_prepend(list, newPPair(id, answer));
}

/**
  \brief Guarda as tags separadas de uma string numa hash table

  \param tags	String de tags de formato ""
  \param t		Hash Table onde são guardadas as tags (valor = chave)
*/
static void hashTags(char* tags, GHashTable* t){
    char *str = g_strdup(tags);
    char *aux = str;
    char buff[32];
    while(g_strrstr(str,"<")){
        sscanf(str,"<%33[^>]",buff);
        char* tag = malloc(strlen(buff) + 1);
        strcpy(tag,buff);

        str += 2 + strlen(tag);
        g_hash_table_add(t, (gpointer) tag);
    }
    g_free(aux);
}


/**
  \brief Vai buscar o atributo Tags e guarda numa HashTable no post

  \param post 		Row da resposta onde vai ser guardado a pergunta
  \param cur 		Apontador para o node xml onde está o atributo
*/
static void parseTagsPost(TRow post, xmlNodePtr cur){
	xmlChar *attr = xmlGetProp(cur,(xmlChar*) "Tags");

	if(attr){
	    GHashTable* tags = g_hash_table_new(g_str_hash, g_str_equal);
		hashTags((char*)attr, tags);
		xmlFree(attr);
		tUpdateRow(post, PTags, tags);
	}
}

/**
  \brief Vai buscar o atributo ParentId que usa para procurar a pergunta correspondente e guarda-a na resposta,
 	guarda também a resposta na lista de respostas da pergunta
 	Se a pergunta não for encontrada, a resposta é guardada numa lista para ser adicionada mais tarde

  \param post 		Row da resposta onde vai ser guardado a pergunta
  \param cur 		Apontador para o node xml onde está o atributo
  \param pending	Lista onde é guardada a resposta se a pergunta não for encontrada
  \param mapPosts 	Map de procura de posts por id
*/
static void parsePair(TRow post, xmlNodePtr cur, GSList *pending, TRowMap mapPosts){
	xmlChar *attr = xmlGetProp(cur,(xmlChar*) "ParentId");

	if(attr){
		//procura a pergunta correspondente
		long parentId = strtol((char*) attr, NULL, 10);
		TRow parent = tMapGetRow(mapPosts, value2ptr(long, parentId));

		if(parent){
			//guarda a pergunta na row da resposta
			tUpdateRow(post, PQuestion, parent);

			//guarda a resposta na row da pergunta
			TRowList answers = (TRowList) tGetData(parent, PAnswers);
			if(!answers) {
				answers = tListNew();
			}
			tListAdd(answers, post);
			tUpdateRow(parent, PAnswers, answers);

		} else {
			//se o id da pergunta for maior, logo ainda não está na tabela, é posta em espera
			pending = addPPair(parentId, post, pending);
		}

		xmlFree(attr);
	}
}


/**
  \brief Vai buscar o atributo OwnerUserId que usa para procurar o user correspondente e guarda-o no post,
 	guarda também o post na lista de posts do user

  \param post 		Row do post onde vai ser guardado o user
  \param cur 		Apontador para o node xml onde está o atributo
  \param tUsers		Tabela de users onde vai ser guardado o post
  \param mapUsers 	Map de procura de users por id
*/
static void parseOwner(TRow post, xmlNodePtr cur, TTable tUsers, TRowMap mapUsers){
	xmlChar* attr = xmlGetProp(cur,(xmlChar*) "OwnerUserId");

	if(attr){
		//procura o owner pelo id e guarda na row
		TRow owner = tMapGetRow(mapUsers, value2ptr(long, strtol((char*) attr, NULL, 10)));
		tUpdateRow(post, POwner, owner);

		if(owner) {

			//guarda o post na row do user
			TRowList arr = (TRowList) tGetData(owner, UPosts);
			if(!arr) {
				arr = tListNew();
				tInsertRow(tUsers, owner, UPosts);
			}
			tListAdd(arr, post);
			tUpdateRow(owner, UPosts, arr);

		}

		xmlFree(attr);
	}
}


/**
  \brief Verifica se uma pergunta corresponde a alguma das respostas na lista de espera, se sim faz as atualizações necessárias (como seria na parsePair)

  \param pending	Lista de respostas com pergunta por atribuir
  \param post		Row da pergunta
*/
static GSList* checkPending(GSList *pending, TRow post){
	long id = *(long*) tGetData(post, PId);

	GSList *l, *a;
	p_pair p;
	for(l = pending; l; l = l->next){ // percorre lista de pares (id da pergunta e lista de respostas)
		p = l->data;
		if(p->parentId == id){

			GSList* answers = (GSList*) tGetData(post, PAnswers);

			for(a = p->answers; a; a = a->next){ // percorre lista de respostas
				TRow answer = a->data;
				//guarda pergunta na resposta
				tUpdateRow(answer, PQuestion, post);
				//guarda resposta na pergunta
				answers = g_slist_prepend(answers, answer);
				tUpdateRow(post, PAnswers, answers);
			}

			destroyPPair(p);
			pending = g_slist_delete_link(pending, l);//pouco eficiente
		}
	}

	return pending;
}


/**
  \brief Guarda o atributo Title numa Row e as palavras que contém num Map

  \param row 	Row onde vai ser guardado
  \param cur 	Apontador para o node xml onde está o atributo
  \param map 	Map onde vão ser guardadas as palavras (palavra -> lista de rows)
*/
static void parseTitle(TRow row, xmlNodePtr cur, TRowMapList map){

	xmlChar *attr = xmlGetProp(cur,(xmlChar*) "Title");
	if(!attr) return;

	int len = strlen((char*) attr);
	char *title = malloc(len +1);
	strcpy(title, (char*) attr);
	xmlFree(attr);

	tUpdateRow(row, PTitle, title);

	int i, j = 0;
	char word[128];
	char c;
    for(i = 0; i <= len; i++) {
 		//se encontrar palavra ou /0 guarda a palavra
    	c = title[i];
        if(c ==' '|| c =='\0') {
            word[j]='\0';
            tMapListAddRowByKey(map, word, row);
            j = 0;    //for next word, init index to 0
        } else {
            word[j] = title[i];
            j++;
        }
    }


}


/**
  \brief Da parse a um ficheiro Posts.xml e guarda as informações em duas tables(perguntas e respostas),
  dois maps(id->posts e palavra->pergunta) e atualiza uma Table de users

	Estrutura das rows nas tables de perguntas e respostas respetivamente:
	PostTypeId, Id,    CreationDate, Owner,    Title,   Tags,     Answers
	TInt,       TLong, TDate,        TPointer, TString, TPointer, TList

	PostTypeId, Id,    CreationDate, Owner,    Score, CommentCount, Question
	TInt,       TLong, TDate,        TPointer, TInt,  TInt,         TPointer

	As rows podem ser pesquisadas/percorridas ordenadamente por CreationDate e Score

  \param dump_path 	Path para onde está o ficheiro  Users.xml
  \param tUsers		Table com informações dos users (criada com parseUsers)
  \param mapPosts 	Map que liga id -> Posts, que vai ser preenchido
  \param mapUsers	Map que liga id -> Users, usado para procura de users
  \param mapWords	Map que liga palavras no titulo -> Perguntas

  \return 			Apontador para array com duas tables com informação das perguntas e respostas
*/
TTable* parsePosts(char* dump_path, TTable tUsers, TRowMap mapPosts, TRowMap mapUsers, TRowMapList mapWords) {

	char docname[256];
	strcpy(docname, dump_path);
	strcat(docname, "/Posts.xml");

	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(docname);

	if (doc == NULL ) {
		fprintf(stderr,"%s not parsed successfully. \n", docname);
		return NULL;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"%s is empty.\n", docname);
		xmlFreeDoc(doc);
		return NULL;
	}

	TTable *tables = g_malloc_n(2, sizeof(TTable));

	//perguntas
	//{"PostTypeId", "Id", "CreationDate", "Owner", "Title", "Tags", "Answers"};
	Type typesQ[7] = {TInt, TLong, TDate, TPointer, TString, TPointer, TList};
	QST = tNew(typesQ, 7);

	//respostas
	//{"PostTypeId", "Id", "CreationDate", "Owner", "Score", "CommentCount", "Question"};
	Type typesA[7] = {TInt, TLong, TDate, TPointer, TInt, TInt, TPointer};
	ANS = tNew(typesA, 7);


	GSList* pending = NULL;

	for (cur = cur->xmlChildrenNode; cur != NULL; cur = cur->next) {

	    if ((!xmlStrcmp(cur->name, (const xmlChar*)"row"))) {

	    	TRow post;

    		xmlChar* attr = xmlGetProp(cur,(xmlChar*) "PostTypeId");
	    	int postType = (int) strtol((char*) attr, NULL, 10);
	    	xmlFree(attr);


	    	if(postType == Question){
	    		post = tCreateRow(QST);

	    		parseTagsPost(post, cur);

	    		//parseAttr("Title", typesQ[PTitle], PTitle, post, cur);
	    		parseTitle(post, cur, mapWords);
	    	}


	    	else if(postType == Answer){
				post = tCreateRow(ANS);

				parsePair(post, cur, pending, mapPosts);//liga resposta e pergunta

				parseAttr("Score", typesA[PScore], PScore, post, cur);
				tInsertRow(ANS, post, PScore);//?

				parseAttr("CommentCount", typesA[PCommentCount], PCommentCount, post, cur);
			}

			else { //posts de outros tipos
				continue;
			}

			int* pType = g_malloc(sizeof(int));
	    	*pType = postType;
			tUpdateRow(post, PPostTypeId, pType);

			parseAttr("Id", typesQ[PId], PId, post, cur);
			tMapSetRowByCol(mapPosts, PId, post);

			parseAttr("CreationDate", typesQ[PCreationDate], PCreationDate, post, cur);
			tInsertRow(tables[postType - 1], post, PCreationDate);

			parseOwner(post, cur, tUsers, mapUsers);

			//se for a pergunta de alguma resposta com pergunta por atribuir, faz as alterações necessárias
			if(pending && postType == Question){
				pending = checkPending(pending, post);
			}
		}
	}

	tColumnSort(QST, PCreationDate);
	tColumnSort(ANS, PCreationDate);
	tColumnSort(ANS, PScore); //query 6?
	//tColumnSort(QST, PAnswers); //query 7?
	tColumnSort(tUsers, UPosts);

	xmlFreeDoc(doc);

	return tables;
}


/**
  \brief Da parse a um ficheiro Tags.xml e guarda os nomes/ids numa hash table

  \param dump_path 	Path para onde está o ficheiro  Tags.xml

  \return 			hash table onde as chaves são Strings com o nome da tag e os valores o id correspondente
*/
GHashTable* parseTags(char* dump_path){

	char docname[256];
	strcpy(docname, dump_path);
	strcat(docname, "/Tags.xml");

	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(docname);

	if (doc == NULL ) {
		fprintf(stderr,"%s not parsed successfully. \n", docname);
		return NULL;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"%s is empty.\n", docname);
		xmlFreeDoc(doc);
		return NULL;
	}

	GHashTable* table = g_hash_table_new (g_str_hash, g_str_equal);
	gpointer id, tagName;
	xmlChar *attr = xmlGetProp(cur,(xmlChar*) "Id");

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
	    if ((!xmlStrcmp(cur->name, (const xmlChar*)"row"))) {

	    	attr = xmlGetProp(cur,(xmlChar*) "Id");
			if(attr){
				id = convertString((char*) attr, TLong);
				xmlFree(attr);
			}

			attr = xmlGetProp(cur,(xmlChar*) "TagName");
			if(attr){
				tagName = convertString((char*) attr, TString);
				xmlFree(attr);
			}

			g_hash_table_insert (table, tagName, id);

 	    }
	cur = cur->next;
	}

	xmlFreeDoc(doc);

	return table;
}
