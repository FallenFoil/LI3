#include "interface.h"
#include "parser.h"
#include "database.h"

#define USERS 	0
#define POSTS   1
#define QST 	1
#define ANS 	2


struct TCD_community {
	TTable tables[3];
	TRowMap maps[2];
	TRowMapList mapWords;
	GHashTable* tags;
};


TAD_community init() {
	TAD_community com = g_new(struct TCD_community, 1);
	return com;
}

TAD_community load(TAD_community com, char* dump_path){

	com->maps[USERS] = tNewMap(TLong);
	com->maps[POSTS] = tNewMap(TLong);
	com->mapWords = tNewMapList(TString);

	com->tables[USERS] = parseUsers(dump_path, com->maps[USERS]);
	TTable *tables = parsePosts(dump_path, com->tables[USERS], com->maps[POSTS], com->maps[USERS], com->mapWords);
	com->tables[QST] = tables[0];
	com->tables[ANS] = tables[1];

	com->tags = parseTags(dump_path);
	malloc_trim(0);
	return com;
}

TTable getTable(TAD_community com, int t){
	return com->tables[t];
}

//os intervalos de tempo sao em dias
//posts = perguntas e respostas
//CI = cronologia inversa (da mais recente para a mais antiga)


//1- recebe id post, devolve titulo e nome do autor (se for resposta, da pergunta correspondente)
STR_pair info_from_post(TAD_community com, long id){

	TRow post = tMapGetRow(com->maps[POSTS], value2ptr(long, id));

	if(*(int*) tGetData(post, PPostTypeId) == Answer){
		post = (TRow) tGetData(post, PQuestion);
	}

	char* title = (char*) tGetData(post, PTitle);

	post = (TRow) tGetData(post, POwner);
	char* name = (char*) tGetData(post, UDisplayName);

	return create_str_pair(title, name);
}


//2- top N utilizadores com mais posts
LONG_list top_most_active(TAD_community com, int N){

	TRow user = tLastRow(com->tables[USERS], UPosts);

	LONG_list list = create_list(N);

	for(int i = 0; i < N && user; i++){
		set_list(list, i, *(long*) tGetData(user, UId));
		user = tPrevRow(user, UPosts);
	}
	set_list(list, N, 0);

	return list;
}



static long dateToLong(Date date){
	int y,m,d;
	y = get_year(date);
	m = get_month(date);
	d = get_day(date);
	return (y << 9) | (m << 5) | d;
}

static gboolean counTRows(TRow row, gconstpointer userData){
    ptr2value(glong, userData)++;
    return FALSE;
}

//3- recebe intervalo de tempo, devolve numero de perguntas e respostas
LONG_pair total_posts(TAD_community com, Date begin, Date end){

	glong qst = 0;
	glong ans = 0;

	TQuery beginQ  = tNewQuery(PCreationDate, OGreaterEqual, value2ptr(glong, dateToLong(begin)));
	TQuery endQ    = tNewQuery(PCreationDate, OLessEqual, value2ptr(glong, dateToLong(end)));

	tApplyRows(com->tables[QST], (TQuery[]) {beginQ, endQ}, 2, 0, counTRows, &qst);
	tApplyRows(com->tables[ANS], (TQuery[]) {beginQ, endQ}, 2, 0, counTRows, &ans);

	tDeleteQuery(beginQ);
	tDeleteQuery(endQ);

	return create_long_pair(qst, ans);
}



//recebe array de posts e um N, devolve uma lista com os ids dos primeiros N posts do array
LONG_list rowListToIdList(TRowList arr, int N){
	LONG_list list = create_list(N);

    for (int i = 0; i < N; i++) {
    	set_list(list, i,  *(long*) tGetData(tListPosition(arr, i), PId));
    }

    set_list(list, N, 0);
	return list;
}

typedef struct tagCheck {
	TRowList questions;
	char *tag;
}* Check;

static gboolean getQstWithTag(TRow row, gconstpointer userData){
	Check listTag = (Check) userData;
	if (g_hash_table_contains ((GHashTable*) tGetData(row, PTags), listTag->tag))
		tListAdd(listTag->questions, row);
	return FALSE;
}

//4- recebe intervalo de tempo, devolve ids das perguntas com determinada tag CI
LONG_list questions_with_tag(TAD_community com, char* tag, Date begin, Date end){

	TQuery beginQ  = tNewQuery(PCreationDate, OGreaterEqual, value2ptr(long, dateToLong(begin)));
	TQuery endQ 	= tNewQuery(PCreationDate, OLessEqual, value2ptr(long, dateToLong(end)));

	Check listTag = malloc(sizeof(Check));
	listTag->questions = tListNew();
	listTag->tag = tag;
	//fazer funçoes disto
	tApplyRows(com->tables[QST], (TQuery[]) {beginQ, endQ}, 2, -1, getQstWithTag, listTag); //percorre por ordem CI de data
	LONG_list list = rowListToIdList(listTag->questions, tListLength(listTag->questions));

	tListDelete(listTag->questions);

	g_free(listTag);

	tDeleteQuery(beginQ);
	tDeleteQuery(endQ);

	return list;
}



static gint compareDates(gconstpointer a, gconstpointer b) {

    long dateA = *(long*) tGetData(*(TRow*) a, PCreationDate);
    long dateB = *(long*) tGetData(*(TRow*) b, PCreationDate);

    return dateB - dateA; // ordem crescente inversa (mais recente para mais antigo)
}

//5- recebe id utilizador, devolve aboutMe e ids dos ultimos 10 posts CI
USER get_user_info(TAD_community com, long id){

	TRow user = tMapGetRow(com->maps[USERS], value2ptr(long, id));

	char* bio = (char*) tGetData(user, UAboutMe);
	long history[10];

	TRowList posts = (TRowList) tGetData(user, UPosts);

	if(posts){
		tListSort(posts, compareDates);
		int len = tListLength(posts);
		for(int i = 0; i < 10 && i < len; i++){
			history[i] = *(long*) tGetData(tListPosition(posts, i), PId);
		}
	}

	USER info = create_user(bio, history);
	return info;
}



static gint compareScores(gconstpointer a, gconstpointer b) {

    int scoreA = *(int*) tGetData(*(TRow*) a, PScore);
    int scoreB = *(int*) tGetData(*(TRow*) b, PScore);

    return scoreB - scoreA; //ordem crescente de score
}

//percorrer pelo top primeiro se intervalo for muito grande?
//6- recebe intervalo de tempo, devolve ids das N respostas com mais votos(score) por ordem decrescente
LONG_list most_voted_answers(TAD_community com, int N, Date begin, Date end){

	TQuery beginQ  = tNewQuery(PCreationDate, OGreaterEqual, value2ptr(long, dateToLong(begin)));
	TQuery endQ    = tNewQuery(PCreationDate, OLessEqual, value2ptr(long, dateToLong(end)));

	TRowList list = tGetRows(com->tables[ANS], (TQuery[]) {beginQ, endQ}, 2, 0);

	tDeleteQuery(beginQ);
	tDeleteQuery(endQ);

	tListSort(list, compareScores);

	int n = tListLength(list);
	if(N < n) n = N;

	return rowListToIdList(list, n);
}


typedef struct interval{
	long begin;
	long end;
} *inter;


static gint compareAnswers(gconstpointer a, gconstpointer b, gpointer in) {

	inter interval = (inter) in;
	int i, ansA = 0, ansB = 0;
	long date;
	TRowList listA = (TRowList) tGetData(*(TRow*) a, PAnswers);
	if(listA){
		int sizeA = tListLength(listA);
		for (i = 0; i < sizeA; i++){
			date = *(long*) tGetData(tListPosition(listA, i), PCreationDate);
			if(date >= interval->begin && date <= interval->end)
				ansA++;
		}
	}

	TRowList listB = (TRowList) tGetData(*(TRow*) b, PAnswers);
	if(listB){
		int sizeB = tListLength(listB);
		for (i = 0; i < sizeB; i++){
			date = *(long*) tGetData(tListPosition(listB, i), PCreationDate);
			if(date >= interval->begin && date <= interval->end)
				ansB++;
		}
	}

    return ansB - ansA; //ordem decrescente numero de respostas
}

//melhorar como a 6?
//7- recebe intervalo de tempo, devolve ids das N perguntas com mais respostas por ordem decrescente do numero de respostas
LONG_list most_answered_questions(TAD_community com, int N, Date begin, Date end){


	long beginD = dateToLong(begin);
	long endD = dateToLong(end);
	TQuery beginQ  = tNewQuery(PCreationDate, OGreaterEqual, value2ptr(long, beginD));
	TQuery endQ    = tNewQuery(PCreationDate, OLessEqual, value2ptr(long, endD));

	TRowList list = tGetRows(com->tables[QST], (TQuery[]) {beginQ, endQ}, 2, 0);

	tDeleteQuery(beginQ);
	tDeleteQuery(endQ);

	inter in = g_new(struct interval, 1);
	in->begin = beginD;
	in->end = endD;
	tListSortWithData(list, compareAnswers, in);
	g_free(in);

	int n = tListLength(list);
	if(N < n) n = N;

	return rowListToIdList(list, n);
}



//8- recebe palavra, devolve ids de N perguntas cujos titulos a contenham CI
LONG_list contains_word(TAD_community com, char* word, int N){

	TRowList rowList = tMapListGetRows(com->mapWords, word);

	tListSort(rowList, compareDates);

	int n = tListLength(rowList);
	if(N < n) n = N;

	return rowListToIdList(rowList, n);
}



//9- recebe ids de dois users, devolve ids ultimas N perguntas CI em que os dois participaram (podem participar tanto na p como r)
LONG_list both_participated(TAD_community com, long id1, long id2, int N){

	TRowList posts1 = (TRowList) tGetData(tMapGetRow(com->maps[USERS], value2ptr(long, id1)), UPosts);
	TRowList posts2 = (TRowList) tGetData(tMapGetRow(com->maps[USERS], value2ptr(long, id2)), UPosts);

	if(!posts1 || !posts2) return NULL;
	long arr[N];

	TRowList qst1 = tListNew();
	TRowList qst2 = tListNew();

	int i, j ,n;
	TRow post;

	//passsa as perguntas dos posts1 e 2 para qst1 e 2 e ordena por data CI
	int size1 = tListLength(posts1);
	for (i = 0; i < size1; i++){
		post = tListPosition(posts1, i);
		if(*(int*) tGetData((TRow) post, PPostTypeId) == Answer)
			post = (TRow) tGetData(post, PQuestion);
		if(post) tListAdd(qst1, post);
	}
	tListSort(qst1, compareDates);

	int size2 = tListLength(posts2);
	for (i = 0; i < size2; i++){
		post = tListPosition(posts2, i);
		if(*(int*) tGetData((TRow) post, PPostTypeId) == Answer)
			post = (TRow) tGetData(post, PQuestion);
		if(post) tListAdd(qst2, post);
	}
	tListSort(qst2, compareDates);

	i = 0, j = 0, n = 0;
    while (i < tListLength(qst1) && j < tListLength(qst2) && n < N) {

		long date1 = *(long*) tGetData(tListPosition(qst1, i), PCreationDate);
		long date2 = *(long*) tGetData(tListPosition(qst2, j), PCreationDate);

        if (date1 == date2){
        	long date = date1, pId1, pId2;
        	int ii, jj;
        	for(ii = i; date == date1 && ii < tListLength(qst1); ii++){
        		for(jj = j; date == date1 && jj < tListLength(qst2); jj++){
        			pId1 = *(long*) tGetData(tListPosition(qst1, ii), PId);
    				pId2 = *(long*) tGetData(tListPosition(qst2, jj), PId);
    				if (pId1 == pId2){
			        	arr[n] = pId1;
						n++;
			        }
        			date = *(long*) tGetData(tListPosition(qst2, jj), PCreationDate);
        		}
        		date = *(long*) tGetData(tListPosition(qst1, ii), PCreationDate);
        	}
        	i = ii;
        	j = jj;
        }
        else if (date1 > date2){
            i++;
        }
        else{
 			j++;
 		}
    }

    tListDelete(qst1);
    tListDelete(qst2);

    LONG_list list = create_list(n);

    //guarda so um id de cada post/evita repetidos
    int k = 0;
    int rep;
    for (i = 0; i < n; i++) {
    	rep = 0;
    	for (int j = 0; j < k; j++) {
    		if(arr[i] == get_list(list, j)){
    			rep = 1;
    			break;
    		}
    	}
    	if(!rep){
    		set_list(list, k, arr[i]);
    		k++;
    	}
    }
    set_list(list, k, 0);

	return list;
}



//10- recebe id pergunta, devolve id resposta com melhor: (Scr × 0.45) + (Rep × 0.25) + (Score × 0.2) + (Comt × 0.1)
long better_answer(TAD_community com, long id){

	int scr;
	int rep = 0; //owner pode não existir
	int cmt;
	int total;
	int best = 0;
	TRow bestAns = NULL;
	TRow owner;

	TRow question = tMapGetRow(com->maps[POSTS], value2ptr(long, id));

	TRowList ans = (TRowList) tGetData(question, PAnswers);
	int len = tListLength(ans);
	for(int i = 0; i < len; i++){
		TRow answer = tListPosition(ans, i);

		scr = *(int*) tGetData(answer, PScore);
		cmt = *(int*) tGetData(answer, PCommentCount);
		owner = (TRow) tGetData(answer, POwner);
		if(owner) rep = *(int*) tGetData(owner, UReputation);

		total = scr * 0.65 + rep * 0.25 + cmt * 0.1;

		if (total > best){
			best = total;
			bestAns = answer;
		}
	}

	if(bestAns)
		return *(long*) tGetData(bestAns, PId);
	else
		return 0;
}



struct TagCount{
	char* tag;
	int count;
};

static int compareTagsCount(gconstpointer a, gconstpointer b){
 	struct TagCount* tagA = *(struct TagCount**) a;
	struct TagCount* tagB = *(struct TagCount**) b;
	return tagB->count - tagA->count;
}

//11- recebe intervalo de tempo, devolve N tags mais usadas pelos N users com melhor reputacao ordem decrescente nr de vezes tag foi usada
LONG_list most_used_best_rep(TAD_community com, int N, Date begin, Date end) {
	//lista das perguntas no intervalo de tempo
	TQuery beginQ  = tNewQuery(PCreationDate, OGreaterEqual, value2ptr(long, dateToLong(begin)));
	TQuery endQ    = tNewQuery(PCreationDate, OLessEqual, value2ptr(long, dateToLong(end)));

	TRowList questions = tGetRows(com->tables[QST], (TQuery[]) {beginQ, endQ}, 2, 0);

	tDeleteQuery(beginQ);
	tDeleteQuery(endQ);

	GHashTable *topRep = g_hash_table_new(g_int64_hash, g_int64_equal);
	TRow user = tLastRow(com->tables[USERS], UPosts);

	for(int i = 0; i < N && user; i++){
		g_hash_table_add(topRep, (gpointer) tGetData(user, UId));
		user = tPrevRow(user, UPosts);
	}

	GHashTable *tagCount = g_hash_table_new (g_str_hash, g_str_equal);
	GHashTableIter iter;
	gpointer key, value;
	int *count;

	int n = tListLength(questions);
	//preenche hash table com contagem das tags
	for (int i = 0; i < n; i++) {

		//vai buscar a hash table das tags do post
		gpointer owner = (gpointer) tGetData((gpointer) tListPosition(questions, i), POwner);
		glong* ownerId = (glong*) tGetData((gpointer) owner, UId);
		if(ownerId && g_hash_table_contains(topRep, ownerId)){
			gpointer question = (GHashTable*) tGetData(tListPosition(questions, i), PTags);
			g_hash_table_iter_init (&iter, question);
			while (g_hash_table_iter_next (&iter, &key, &value)) { //percorre
				//incrementa o contador da tag
				count = g_hash_table_lookup (tagCount, key);

				if(count == NULL) {
					count = malloc(sizeof(int));
					*count = 0;
				}
				(*count)++;

			    g_hash_table_insert (tagCount, key, count);
			}
		}
    }

    n = g_hash_table_size (tagCount);
	if(N < n) n = N;

	g_hash_table_iter_init (&iter, tagCount);
	GPtrArray* sorTList = g_ptr_array_new_with_free_func(g_free);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		struct TagCount* tagCounter = g_new(struct TagCount, 1);
		tagCounter->tag = key;
		tagCounter->count = *(int*) value;
		g_ptr_array_add(sorTList, tagCounter);
	}

	g_ptr_array_sort(sorTList, compareTagsCount);

    LONG_list list = create_list(n);

    GHashTable *tagsId = com->tags;
	for(int i = 0; i < n; i++){
		char* key = (*(struct TagCount*) g_ptr_array_index(sorTList, i)).tag;
		int tagId = *(int*) g_hash_table_lookup (tagsId, key);
		set_list(list, i, tagId);
	}
	set_list(list, n, 0);

	g_ptr_array_free(sorTList, TRUE);

	return list;
}

TAD_community clean(TAD_community com){
	TRow row, nextRow;

	// Limpa a tabela dos Users
	nextRow = tFirstRow(com->tables[USERS], UReputation);
	while((row = nextRow)){
		g_free((gpointer) tGetData(row, UId));
		g_free((gpointer) tGetData(row, UReputation));
		g_free((gpointer) tGetData(row, UDisplayName));
		g_free((gpointer) tGetData(row, UAboutMe));
		tListDelete((TRowList) tGetData(row, UPosts));
		nextRow = tNextRow(row, UReputation);
		tDeleteRow(row); // Apaga a row
	}
	tDelete(com->tables[USERS]);

	// Limpa a tabela das Questions
	nextRow = tFirstRow(com->tables[QST], PCreationDate);
	while((row = nextRow)){
		g_free((gpointer) tGetData(row, PPostTypeId));
		g_free((gpointer) tGetData(row, PId));
		g_free((gpointer) tGetData(row, PCreationDate));
		g_free((gpointer) tGetData(row, PTitle));
		tListDelete((TRowList) tGetData(row, PAnswers));
		nextRow = tNextRow(row, PCreationDate);
		tDeleteRow(row); // Apaga a row
	}
	tDelete(com->tables[QST]);

	// Limpa a tabela das Answers
	nextRow = tFirstRow(com->tables[ANS], PCreationDate);
	while((row = nextRow)){
		g_free((gpointer) tGetData(row, PPostTypeId));
		g_free((gpointer) tGetData(row, PId));
		g_free((gpointer) tGetData(row, PCreationDate));
		g_free((gpointer) tGetData(row, PScore));
		g_free((gpointer) tGetData(row, PCommentCount));
		nextRow = tNextRow(row, PCreationDate);
		tDeleteRow(row); // Apaga a row
	}
	tDelete(com->tables[ANS]);

	// Limpa o Map Users
	tMapDelete(com->maps[USERS]);
	//Limpa o Map Posts
	tMapDelete(com->maps[POSTS]);
	//Limpa o Map mapWords;
	tMapListDelete(com->mapWords);
	//Limpa p Map tags
	g_hash_table_destroy(com->tags);
	malloc_trim(0);

	return com;
}
