/*
 * @Author: your name
 * @Date: 2021-09-29 00:49:48
 * @LastEditTime: 2021-09-30 23:48:36
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\include\parse.h
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0


//Header field
typedef struct
{
	char header_name[4096];
	char header_value[4096];
} Request_header;

//HTTP Request Header
typedef struct
{
	char http_version[50];
	char http_method[50];
	char http_uri[4096];
	Request_header *headers;
	int header_count;
} Request;

Request* parse(char *buffer, int size,int socketFd);

// functions decalred in parser.y
int yyparse();
void set_parsing_options(char *buf, size_t i, Request *request);
