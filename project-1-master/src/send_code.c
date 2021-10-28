/*
 * @Author: your name
 * @Date: 2021-10-09 00:18:48
 * @LastEditTime: 2021-10-23 18:08:18
 * @LastEditors: Please set LastEditors
 * @Description: 发送状态码对应的报文
 * @FilePath: \project-1-master\src\send_code.c
 */
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

//发送响应头部
void send_400(int cli_sock, Info info)  //发送400 BAD REQUEST页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 400 BAD REQUEST\r\n");
	strcat(buff,"Server: http/1.1\r\n");
	strcat(buff,"\r\n");   //空行标识数据部分和头部分开
	strcat(buff,"400 Bad Request\n");  //发送给客户端的数据，用于显示
    errorLog(info);
	send(cli_sock,buff,strlen(buff),0);
    
}

void send_404(int cli_sock, Info info)  //发送404 NOT FOUND页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 404 NOT FOUND\r\n");
	strcat(buff,"Server: http/1.1\r\n");
	strcat(buff,"\r\n");   //空行标识数据部分和头部分开
	strcat(buff,"404 NOT FOUND\n");  //发送给客户端的数据，用于显示
    errorLog(info);
	send(cli_sock,buff,strlen(buff),0);
}

void send_408(int cli_sock, Info info)  //发送408 REQUEST TIMEOUT页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 408 REQUEST TIMEOUT\r\n");
	strcat(buff,"Server: http/1.1\r\n");
	strcat(buff,"\r\n");   //空行标识数据部分和头部分开
	strcat(buff,"408 REQUEST TIMEOUT\n");  //发送给客户端的数据，用于显示
    errorLog(info);
	send(cli_sock,buff,strlen(buff),0);
}

void send_200_head(int cli_sock)  //发送200的头部页面
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 200 OK\r\n");
	strcat(buff,"Server: http/1.1\r\n");
	strcat(buff,"\r\n");   //空行标识数据部分和头部分开
	send(cli_sock,buff,strlen(buff),0);
}

void send_HEAD(int cli_sock, Info info)  //发送HEAD页面
{
	send_200_head(cli_sock);
	commonLog(info);
}


void send_501(int cli_sock, Info info)  //发送501 Not Implemented的页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 501 Not Implemented\r\n");
	strcat(buff,"Server: http/1.1\r\n");
	strcat(buff,"\r\n");   //空行标识数据部分和头部分开
	strcat(buff,"501 Not Implemented\n");
    errorLog(info);  
	send(cli_sock,buff,strlen(buff),0);
    
}

void send_505(int cli_sock, Info info)  //发送505 HTTP Version Not Supported的页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 505 HTTP Version Not Supported\r\n");
	strcat(buff,"Server: http/1.1\r\n");
	strcat(buff,"\r\n");   //空行标识数据部分和头部分开
	strcat(buff,"505 HTTP Version Not Supported\n");
    errorLog(info);  
	send(cli_sock,buff,strlen(buff),0);
    
}


/*发送html页面
 *PS:未找到资源发送404 NOT FOUND
 */
void send_html(char* http_uri, int cli_sock, Info info){
    char pathname[128] = "/var/www/html";
    strcat(pathname, http_uri);
    // printf("获得的文件路径:");
    // printf("%s",pathname);
    // printf("\n");
    int fd = open(pathname,O_RDONLY);
    if(fd == -1){
		info.code = 404;
        send_404(cli_sock, info);
    }
    else{
        send_200_head(cli_sock);
        int length = 0;
        while (1){
            char buff[128] = {0};
            int cont = read(fd, buff, 127);
            if(cont > 0){
                length = length + cont;
            }
            //printf("%s", buff);
            if(cont <= 0){
                close(fd);
                break;
            }
            send(cli_sock, buff, strlen(buff), 0);
        }
        info.fileLength = length;
        commonLog(info);
    }
}

//发送echo页面
void send_echo(int cli_sock, char* get_from_cli, Info info){
    char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 200 OK\r\n");
	strcat(buff,"Server: http/1.1\r\n");
	strcat(buff,"\r\n");   //空行标识数据部分和头部分开
    strcat(buff,get_from_cli);
    info.fileLength = strlen(get_from_cli);
    commonLog(info);
	send(cli_sock,buff,strlen(buff),0);
}
