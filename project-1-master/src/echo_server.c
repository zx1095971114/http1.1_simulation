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
#include <time.h>
#include "log.h"
#include "send_code.h"
#include "file_motion.h"
#include "my_segment.h"
#include "my_bool.h"

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
    Info info;
    info.code = 0;
    info.fileLength = 0;
    info.IP = "127.0.0.1";
    info.method = "GET";
    info.path = "/var/www/html";
    info.seriousness = "error";
    info.version = "HTTP/1.1";

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

        //计时器，判断是否超时
        time_t start_time = time(&start_time);

        if ((client_sock == -1))
        {
            close(sock);
            fprintf(stderr, "Error accepting connection.\n");
            return EXIT_FAILURE;
        }


        readret = 0;
        //用isPermanent判断是否要持久连接
        int isPermanent = 1;
        //每次用之前清理buf的缓存
        memset(buf, 0, sizeof(buf));
        while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
        {
            time_t end_time = time(&end_time);
            if(end_time - start_time >= 5){
                info.code = 400;
                send_408(client_sock, info);
                break;
            }

            //分割报文
            int file_num = divide(buf);
            for(int i = 1; i <= file_num; i++){
                char path[50] = "/home/project-1-master/pipelining/recall";
                char file_content[4096] = {0};
                char sfile_num[5] = {0};
                sprintf(sfile_num, "%d", i);
                strcat(path, sfile_num);
                read_file(path, file_content, 4096);
                rm_file(path);

                //显示解析信息
                printf("file_contet: %s\n", file_content);
                Request *request = parse(file_content, strlen(file_content), client_sock);
                if(request != NULL){
                    printf("Http Method %s\n",request->http_method);
                    printf("Http Version %s\n",request->http_version);
                    printf("Http Uri %s\n",request->http_uri);
                    for(int index = 0;index < request->header_count;index++){
                        printf("Request Header\n");
                    printf("Header name: %s\nHeader Value: %s\n",request->headers[index].header_name,request->headers[index].header_value);
                    }
                    //判断是否要持续连接
                    isPermanent = request->isPermanent;

                    //修改info状态
                    info.method = request->http_method;
                    char path[100] = {0};
                    strcpy(path, "/var/www/html");
                    strcat(path, request->http_uri);
                    info.path = path;
                    info.version = request->http_version;
                }

                if(isPermanent == 0){
                    break;
                }

                //根据解析做出相应的响应
                if(get_status(request) == 200){
                    info.code = 200;
                    if(COMPARE_GET(request->http_method)){
                        printf("发送开始\n");
                        send_html(request->http_uri, client_sock, info);
                        printf("发送完成\n");
                        printf("isPermanent:%d\n",request->isPermanent);
                    }
                    else if(COMPARE_HEAD(request->http_method)){
                        info.method = "HEAD";
                        send_HEAD(client_sock, info);
                    }
                    else{
                        info.method = "POST";
                        send_echo(client_sock, buf, info);
                    }
                    free(request->headers);
                    free(request);
                }
                else if(get_status(request) == 501){
                    info.code = 501;
                    send_501(client_sock, info);
                    free(request->headers);
                    free(request);
                }
                else if(get_status(request) == 505){
                    info.code = 505;
                    send_505(client_sock, info);
                    free(request->headers);
                    free(request);
                }
                else{
                    info.code = 400;
                    send_400(client_sock, info);
                }
                
            }   
            break;
        } 

        if (readret == -1)
        {
            close_socket(client_sock);
            close_socket(sock);
            fprintf(stderr, "Error reading from client socket.\n");
            return EXIT_FAILURE;
        }

        if(isPermanent == 0){
            break;
        }
        
        
        if (close_socket(client_sock))
        {
            close_socket(sock);
            fprintf(stderr, "Error closing client socket.\n");
            return EXIT_FAILURE;
        }
        
        printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
    

    close_socket(sock);

    return EXIT_SUCCESS;
}


// //显示解析信息
//             Request *request = parse(buf, strlen(buf), client_sock);
//             if(request != NULL){
//                 printf("Http Method %s\n",request->http_method);
//                 printf("Http Version %s\n",request->http_version);
//                 printf("Http Uri %s\n",request->http_uri);
//                 for(int index = 0;index < request->header_count;index++){
//                     printf("Request Header\n");
//                 printf("Header name: %s\nHeader Value: %s\n",request->headers[index].header_name,request->headers[index].header_value);
//                 }
//                 //判断是否要持续连接
//                 isPermanent = request->isPermanent;

//                 //修改info状态
//                 info.method = request->http_method;
//                 char path[100] = {0};
//                 strcpy(path, "/var/www/html");
//                 strcat(path, request->http_uri);
//                 info.path = path;
//                 info.version = request->http_version;
//             }

//             if(isPermanent == 0){
//                 break;
//             }

//             //根据解析做出相应的响应
//             if(get_status(request) == 200){
//                 info.code = 200;
//                 if(COMPARE_GET(request->http_method)){
//                     printf("发送开始\n");
//                     send_html(request->http_uri, client_sock, info);
//                     printf("发送完成\n");
//                     printf("isPermanent:%d\n",request->isPermanent);
//                 }
//                 else if(COMPARE_HEAD(request->http_method)){
//                     send_200_head(client_sock, info);
//                 }
//                 else{
//                     send_echo(client_sock, buf, info);
//                 }
//                 free(request->headers);
//                 free(request);
//             }
//             else if(get_status(request) == 501){
//                 info.code = 501;
//                 send_501(client_sock, info);
//                 free(request->headers);
//                 free(request);
//             }
//             else if(get_status(request) == 505){
//                 info.code = 505;
//                 send_505(client_sock, info);
//                 free(request->headers);
//                 free(request);
//             }
//             else{
//                 info.code = 400;
//                 send_400(client_sock, info);
//             }
            
//             memset(buf, 0, BUF_SIZE);