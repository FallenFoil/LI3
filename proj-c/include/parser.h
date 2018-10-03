#ifndef PARSER_H
#define PARSER_H

#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


//colunas da tabela dos users
enum { UId = 0, UReputation, UDisplayName, UAboutMe, UPosts};

//colunas da tabela dos posts
//geral
enum { PPostTypeId = 0, PId, PCreationDate, POwner};
//perguntas
enum { PTitle = 4, PTags, PAnswers};
//respostas
enum { PScore = 4, PCommentCount, PQuestion};

//post type id
enum { Question = 1, Answer};

TTable parseUsers(char* dump_path, TRowMap map);
TTable* parsePosts(char* dump_path, TTable tUsers, TRowMap mapPosts, TRowMap mapUsers, TRowMapList mapWords);
GHashTable* parseTags(char* dump_path);

#endif
