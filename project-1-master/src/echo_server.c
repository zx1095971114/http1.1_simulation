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
#include <fcntl.h>
#include "parse.h"

#define ECHO_PORT 9999
#define BUF_SIZE 4096
#define COMPARE_GET(ptr) (ptr[0] == 'G' && ptr[1] == 'E' && ptr[2] == 'T')
#define COMPARE_HEAD(ptr) (ptr[0] == 'H' && ptr[1] == 'E' && ptr[2] == 'A' && ptr[3] == 'D')
#define COMPARE_POST(ptr) (ptr[0] == 'P' && ptr[1] == 'O' && ptr[2] == 'S' && ptr[3] == 'T')
#define COMPARE_VERSION(ptr) (ptr[0] == 'H' && ptr[1] == 'T' && ptr[2] == 'T' && ptr[3] == 'P' && ptr[4] == '/' && ptr[5] == '1' && ptr[6] == '.' && ptr[7] == '1' )

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

//发送响应头部
void send_400(int cli_sock, char* get_from_cli)  //发送400 BAD REQUEST页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 400 BAD REQUEST\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"400 Bad Request\n");  //发送给客户端的数据，用于显示
	send(cli_sock,buff,strlen(buff),0);
}

void send_404(int cli_sock)  //发送404 NOT FOUND页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 404 NOT FOUND\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"404 NOT FOUND\n");  //发送给客户端的数据，用于显示
	send(cli_sock,buff,strlen(buff),0);
}

void send_408(int cli_sock)  //发送408 REQUEST TIMEOUT页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 408 REQUEST TIMEOUT\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"408 REQUEST TIMEOUT\n");  //发送给客户端的数据，用于显示
	send(cli_sock,buff,strlen(buff),0);
}

void send_200_head(int cli_sock)  //发送echo的页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 200 OK\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	send(cli_sock,buff,strlen(buff),0);
}


void send_501(int cli_sock)  //发送501 Not Implemented的页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 501 Not Implemented\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"501 Not Implemented\n");  
	send(cli_sock,buff,strlen(buff),0);
    
}

void send_505(int cli_sock)  //发送505 HTTP Version Not Supported的页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 505 HTTP Version Not Supported\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"505 HTTP Version Not Supported\n");  
	send(cli_sock,buff,strlen(buff),0);
    
}

/*发送html页面
 *PS:未找到资源发送404 NOT FOUND
 */
void send_html(char* http_uri, int cli_sock){
    char pathname[128] = "/var/www/html";
    strcat(pathname, http_uri);
    printf("获得的文件路径:");
    printf(pathname);
    printf("\n");
    int fd = open(pathname,O_RDONLY);
    if(fd == -1){
        send_404(cli_sock);
    }
    else{
        send_200_head(cli_sock);
        while (1){
            char buff[128] = {0};
            int cont = read(fd, buff, 127);
            printf(buff);
            printf("\nend\n");
            if(cont <= 0){
                printf("here1\n");
                close(fd);
                break;
            }
            printf("here2\n");
            send(cli_sock, buff, strlen(buff), 0);
        }
        printf("here\n");
    }
}

//发送echo页面
void send_echo(int cli_sock, char* get_from_cli){
    char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 200 OK\n\r");
	strcat(buff,"Server:http/1.1\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
    strcat(buff,get_from_cli);
	send(cli_sock,buff,strlen(buff),0);
}

//判断报文格式以决定要采取何种回应措施
//返回值为对应的状态码
int get_status(Request *request){
    if(request == NULL){
        
        return 400;
    }
    char* method = request->http_method;
    char* version = request->http_version;
    if(!COMPARE_VERSION(version)){
        return 505;
    }
    else if(COMPARE_GET(method) || COMPARE_HEAD(method) || COMPARE_POST(method)){
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


    addr.sin_family = AF_INET;
    addr.sin_port = htons(ECHO_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;


    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
    {
        close_socket(sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }


    if (listen(sock, 5))
    {
        close_socket(sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

    /* finally, loop waiting for input and then write it back */
    while (1)
    {
        cli_size = sizeof(cli_addr);
        client_sock = accept(sock, (struct sockaddr *) &cli_addr,
                                    &cli_size);

        if ((client_sock == -1))
        {
            close(sock);
            fprintf(stderr, "Error accepting connection.\n");
            return EXIT_FAILURE;
        }
        readret = 0;

        while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
        {
            //显示解析信息
            Request *request = parse(buf, strlen(buf), client_sock);
            if(request != NULL){
                printf("Http Method %s\n",request->http_method);
                printf("Http Version %s\n",request->http_version);
                printf("Http Uri %s\n",request->http_uri);
                for(int index = 0;index < request->header_count;index++){
                    printf("Request Header\n");
                    printf("Header name: %s\nHeader Value: %s\n",request->headers[index].header_name,request->headers[index].header_value);
                }
            }

            //根据解析做出相应的响应
            if(get_status(request) == 200){
                if(COMPARE_GET(request->http_method)){
                    printf("发送开始\n");
                    send_html(request->http_uri, client_sock);
                    printf("发送完成\n");
                }
                else if(COMPARE_HEAD(request->http_method)){
                    send_200_head(client_sock);
                }
                else{
                    send_echo(client_sock, buf);
                }
                free(request->headers);
                free(request);
            }
            else if(get_status(request) == 501){
                send_501(client_sock);
                free(request->headers);
                free(request);
            }
            else if(get_status(request) == 505){
                send_505(client_sock);
                free(request->headers);
                free(request);
            }
            else{
                send_400(client_sock,buf);
            }
            
            memset(buf, 0, BUF_SIZE);
            break;
        } 

        if (readret == -1)
        {
            close_socket(client_sock);
            close_socket(sock);
            fprintf(stderr, "Error reading from client socket.\n");
            return EXIT_FAILURE;
        }

        if (close_socket(client_sock))
        {
            close_socket(sock);
            fprintf(stderr, "Error closing client socket.\n");
            return EXIT_FAILURE;
        }
    }

    close_socket(sock);

    return EXIT_SUCCESS;
}
