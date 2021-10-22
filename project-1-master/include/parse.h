/*
 * @Author: your name
 * @Date: 2021-09-29 00:49:48
 * @LastEditTime: 2021-10-22 21:38:20
 * @LastEditors: Please set LastEditors
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
	char header_name[4096]; //首部行名称
	char header_value[4096]; //首部行值
} Request_header;

//HTTP Request Header
typedef struct
{
	char http_version[50]; //协议版本
	char http_method[50]; //请求方法
	char http_uri[4096];  //请求资源的uri
	Request_header *headers; //请求首部行
	int header_count;		//请求首部行数目
	
	//是否要持续连接属性:是为1，不是为0
	int isPermanent;
} Request;

Request* parse(char *buffer, int size,int socketFd);

// functions decalred in parser.y
int yyparse();
void set_parsing_options(char *buf, size_t i, Request *request);
