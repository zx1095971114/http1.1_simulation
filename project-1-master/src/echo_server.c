/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: This file contains the C source code for an echo server.  The  *
*              server runs on a hard-coded port and simply write back anything*
*              sent to it by connected clients.  It does not support          *
*              concurrent clients.                                            *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "parse.h"

#define ECHO_PORT 9999
#define BUF_SIZE 4096
#define COMPARE_GET(ptr) (ptr[0] == 'G' && ptr[1] == 'E' && ptr[2] == 'T')
#define COMPARE_HEAD(ptr) (ptr[0] == 'H' && ptr[1] == 'E' && ptr[2] == 'A' && ptr[3] == 'D')
#define COMPARE_POST(ptr) (ptr[0] == 'P' && ptr[1] == 'O' && ptr[2] == 'S' && ptr[3] == 'T')


int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

//发送消息
void send_400(int addr_cli)  //发送错误页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 404 NOT FOUND\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"Content-Length: ");
	sprintf(buff+strlen(buff),"%d",0);
	strcat(buff,"\n\r");
	strcat(buff,"Content-Type:text/html;charset=utf-8\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"404 NOT FOUND");  //发送给客户端的数据，用于显示
	send(addr_cli,buff,strlen(buff),0);
}

void send_200(int cli_sock, char* get_from_cli)  //发送echo的页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 200 OK\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,get_from_cli);  
	send(cli_sock,buff,strlen(buff),0);
}

void send_501(int cli_sock, char* get_from_cli)  //发送echo的页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 501 Not Implemented\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"501 Not Implemented");  
	send(cli_sock,buff,strlen(buff),0);
    
}


//判断报文格式以决定要采取何种回应措施
//返回值为对应的状态码
int get_status(Request *request){
    if(request == NULL){
        return 400;
    }
    char* method = request->http_method;

    if(COMPARE_GET(method) || COMPARE_HEAD(method) || COMPARE_POST(method)){
        return 200;
    }
    else{
        return 501;
    }
}

int main(int argc, char* argv[])
{
    int sock, client_sock;
    ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];

    fprintf(stdout, "----- Echo Server -----\n");
    
    /* all networked programs must create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }
    printf("1\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    printf("2\n");
    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }

    printf("3\n");
    if (listen(sock, 5))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }
    printf("4\n");
    /* finally, loop waiting for input and then write it back */
    while (1)
    {
        cli_size = sizeof(cli_addr);
        client_sock = accept(sock, (struct sockaddr *) &cli_addr,
                                    &cli_size);
        printf("5\n");
        if ((client_sock == -1))
        {
            printf("7\n");
            close(sock);
            fprintf(stderr, "Error accepting connection.\n");
            return EXIT_FAILURE;
        }
        printf("6\n");
        readret = 0;

        while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
        {
            //显示解析信息
            printf("2\n");
            Request *request = parse(buf, strlen(buf), client_sock);
            if(request != NULL){
                printf("Http Method %s\n",request->http_method);
                printf("Http Version %s\n",request->http_version);
                printf("Http Uri %s\n",request->http_uri);
                for(int index = 0;index < request->header_count;index++){
                    printf("Request Header\n");
                    printf("Header name %s Header Value %s\n",request->headers[index].header_name,request->headers[index].header_value);
                }
            }

            //根据解析做出相应的响应
            printf("1===================");
            printf("%d\n",get_status(request));
            printf("1===================");
            free(request->headers);
            printf("2===================");
            free(request);
            printf("1===================");

            if (send(client_sock, buf, readret, 0) != readret)
            {
                close_socket(client_sock);
                close_socket(sock);
                fprintf(stderr, "Error sending to client.\n");
                return EXIT_FAILURE;
            }
            memset(buf, 0, BUF_SIZE);
        }
        printf("2==================="); 

        if (readret == -1)
        {
            printf("3===================");
            close_socket(client_sock);
            close_socket(sock);
            fprintf(stderr, "Error reading from client socket.\n");
            return EXIT_FAILURE;
        }

        if (close_socket(client_sock))
        {
            printf("4===================");
            close_socket(sock);
            fprintf(stderr, "Error closing client socket.\n");
            return EXIT_FAILURE;
        }
    }

    close_socket(sock);

    return EXIT_SUCCESS;
}
