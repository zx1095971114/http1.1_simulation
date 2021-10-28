/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: 实现了GET,HEAD方法，对POST请求echo返回，实现pipelining，实现8000 *
*              客户端的并发访问                                                *
*                                                                             *
* Authors: 周翔，吴家旺                                                        *
* Former Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                  *
*                 Wolf Richter <wolf@cs.cmu.edu>                              *
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
#include "debug.h"
#include <assert.h>

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
    int sock;
    ssize_t readret;
    struct sockaddr_in addr;
    
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

    fd_set readFdSet; //读事件的socket集合，用于select的监听时要用副本，因为select会改变该集合
    int maxfd; //readFdSet中有的所有socket

    //初始化socket结构体
    FD_ZERO(&readFdSet);

    //将监听的socket加入集合
    FD_SET(sock, &readFdSet);
    maxfd = sock;


    while (1)
    {
        //将readFdSet复制一份
        fd_set tempFdSet = readFdSet;

        //程序将阻塞在此处，当有事件发生，select才会返回值
        int infds = select(maxfd + 1, &tempFdSet, NULL, NULL, NULL);

        //select返回失败的情况
        if(infds <= 0){
            printf("select() failed.\n");
            break;
        }


        //此时infds > 0,infds代表发生了事件的socket数目
        //遍历整个集合来看哪个socket发生了事件
        for(int eventFd = 0; eventFd <= maxfd; eventFd++){
            //该位置的socket无事件发生
            if(FD_ISSET(eventFd, &tempFdSet) <= 0){
                continue;
            }

            //发生事件的是服务器的监听socket，表示有新用户连接，将其加入位图中
            if(eventFd == sock){
                struct sockaddr_in cli_addr;
                socklen_t cli_size = sizeof(cli_addr);
                int client_sock = accept(sock, (struct sockaddr *) &cli_addr, &cli_size);

                if(client_sock < 0){
                    printf("accect() failed.\n");
                    continue;
                }

                FD_SET(client_sock, &readFdSet);

                if(maxfd < client_sock){
                    maxfd = client_sock;
                }

                continue;
            }

            //其他事件发生在客户端socket，发生的事件可能是有数据被发送过来或其主动断开连接
            else{
                BUG_PRINTF("eventfd: %d\n\n", eventFd);
                char buf[BUF_SIZE];
                memset(buf, 0, sizeof(buf));

                //计时器，判断是否超时
                time_t start_time = time(&start_time);

                readret = 0;
                //用isPermanent判断是否要持久连接
                int isPermanent = 1;
                while((readret = recv(eventFd, buf, BUF_SIZE, 0)) >= 1)
                {
                    //客户端断开连接或发生错误
                    if(readret <= 0){
                        //关闭客户端
                        close(eventFd);
                        //将客户端socket移除
                        FD_CLR(eventFd, &readFdSet);
                        //重新计算maxfd的值
                        if(eventFd == maxfd){
                            for(int i = maxfd; i > 0; i--){
                                if(FD_ISSET(i, &readFdSet)){
                                    maxfd = i;
                                    break;
                                }
                            }
                        }
                    }

                    time_t end_time = time(&end_time);
                    if(end_time - start_time >= 5){
                        info.code = 400;
                        send_408(eventFd, info);
                        break;
                    }

                    //分割报文
                    BUG_POINT(5);
                    Segment_queue* queue = (Segment_queue*) malloc(sizeof(Segment_queue));
                    init_Segment_queue(queue);
                    int file_num = divide(buf, queue);
                    BUG_POINT(6);
                    BUG_PRINTF("file_num: %d\n", file_num);
                    for(int i = 1; i <= file_num; i++){
                        BUG_POINT(7);
                        char segment[4096];
                        memset(segment, 0, sizeof(segment));
                        assert(pop(queue, segment) != FALSE);

                        BUG_POINT(9);
                        Request *request = parse(segment, sizeof(segment), eventFd);
                        if(request != NULL){
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
                        BUG_POINT(1);
                        if(get_status(request) == 200){
                            BUG_POINT(2);
                            info.code = 200;
                            if(COMPARE_GET(request->http_method)){
                                BUG_POINT(3);
                                send_html(request->http_uri, eventFd, info);
                                BUG_POINT(4);
                            }
                            else if(COMPARE_HEAD(request->http_method)){
                                info.method = "HEAD";
                                send_HEAD(eventFd, info);
                            }
                            else{
                                info.method = "POST";
                                send_echo(eventFd, buf, info);
                            }
                            free(request->headers);
                            free(request);
                        }
                        else if(get_status(request) == 501){
                            info.code = 501;
                            send_501(eventFd, info);
                            free(request->headers);
                            free(request);
                        }
                        else if(get_status(request) == 505){
                            info.code = 505;
                            send_505(eventFd, info);
                            free(request->headers);
                            free(request);
                        }
                        else{
                            info.code = 400;
                            send_400(eventFd, info);
                        }
                        
                    }
                    destroy_Segment_queue(queue);
                    free(queue);   
                    break;
                } 

                if (readret == -1)
                {
                    close_socket(eventFd);
                    close_socket(sock);
                    fprintf(stderr, "Error reading from client socket.\n");
                    return EXIT_FAILURE;
                }

                if(isPermanent == 0){
                    break;
                }
                
                
                if (close_socket(eventFd))
                {
                    close_socket(sock);
                    fprintf(stderr, "Error closing client socket.\n");
                    return EXIT_FAILURE;
                }

                //从集合中将处理好的客户端socket移除
                FD_CLR(eventFd, &readFdSet);

                //重新计算maxfd的值
                if(eventFd == maxfd){
                    for(int i = maxfd; i > 0; i--){
                        if(FD_ISSET(i, &readFdSet)){
                            maxfd = i;
                            break;
                        }
                    }
                }

            }
        }

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







// cli_size = sizeof(cli_addr);
//         client_sock = accept(sock, (struct sockaddr *) &cli_addr,
//                                     &cli_size);

//         //计时器，判断是否超时
//         time_t start_time = time(&start_time);

//         if ((client_sock == -1))
//         {
//             close(sock);
//             fprintf(stderr, "Error accepting connection.\n");
//             return EXIT_FAILURE;
//         }


//         readret = 0;
//         //用isPermanent判断是否要持久连接
//         int isPermanent = 1;
//         while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
//         {
//             time_t end_time = time(&end_time);
//             if(end_time - start_time >= 5){
//                 info.code = 400;
//                 send_408(client_sock, info);
//                 break;
//             }

//             //分割报文
//             BUG_POINT(1);
//             int file_num = divide(buf);
//             BUG_POINT(2);
//             for(int i = 1; i <= file_num; i++){
//                 char path[50] = "/home/project-1-master/pipelining/recall";
//                 char file_content[4096] = {0};
//                 char sfile_num[5] = {0};
//                 sprintf(sfile_num, "%d", i);
//                 strcat(path, sfile_num);

//                 //打开文件   
//                 int fd_in = open(path, 0);
//                 int readRet = read(fd_in, file_content, 4096);

//                 //显示解析信息
//                 printf("file_contet: %s\n", file_content);
//                 parse(file_content, readRet, fd_in);

//                 close(fd_in);
//                 rm_file(path);

//                 Request *request = parse(file_content, strlen(file_content), client_sock);
//                 if(request != NULL){
//                     printf("Http Method %s\n",request->http_method);
//                     printf("Http Version %s\n",request->http_version);
//                     printf("Http Uri %s\n",request->http_uri);
//                     for(int index = 0;index < request->header_count;index++){
//                         printf("Request Header\n");
//                     printf("Header name: %s\nHeader Value: %s\n",request->headers[index].header_name,request->headers[index].header_value);
//                     }
//                     //判断是否要持续连接
//                     isPermanent = request->isPermanent;

//                     //修改info状态
//                     info.method = request->http_method;
//                     char path[100] = {0};
//                     strcpy(path, "/var/www/html");
//                     strcat(path, request->http_uri);
//                     info.path = path;
//                     info.version = request->http_version;
//                 }

//                 if(isPermanent == 0){
//                     break;
//                 }

//                 //根据解析做出相应的响应
//                 if(get_status(request) == 200){
//                     info.code = 200;
//                     if(COMPARE_GET(request->http_method)){
//                         printf("发送开始\n");
//                         send_html(request->http_uri, client_sock, info);
//                         printf("发送完成\n");
//                         printf("isPermanent:%d\n",request->isPermanent);
//                     }
//                     else if(COMPARE_HEAD(request->http_method)){
//                         info.method = "HEAD";
//                         send_HEAD(client_sock, info);
//                     }
//                     else{
//                         info.method = "POST";
//                         send_echo(client_sock, buf, info);
//                     }
//                     free(request->headers);
//                     free(request);
//                 }
//                 else if(get_status(request) == 501){
//                     info.code = 501;
//                     send_501(client_sock, info);
//                     free(request->headers);
//                     free(request);
//                 }
//                 else if(get_status(request) == 505){
//                     info.code = 505;
//                     send_505(client_sock, info);
//                     free(request->headers);
//                     free(request);
//                 }
//                 else{
//                     info.code = 400;
//                     send_400(client_sock, info);
//                 }
                
//             }   
//             break;
//         } 

//         if (readret == -1)
//         {
//             close_socket(client_sock);
//             close_socket(sock);
//             fprintf(stderr, "Error reading from client socket.\n");
//             return EXIT_FAILURE;
//         }

//         if(isPermanent == 0){
//             break;
//         }
        
        
//         if (close_socket(client_sock))
//         {
//             close_socket(sock);
//             fprintf(stderr, "Error closing client socket.\n");
//             return EXIT_FAILURE;
//         }
        