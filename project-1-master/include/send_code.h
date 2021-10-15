/*
 * @Author: your name
 * @Date: 2021-10-09 00:22:18
 * @LastEditTime: 2021-10-16 01:49:42
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\include\send_code.h
 */
#ifndef SEND_CODE_H
#define SEND_CODE_H
#include "log.h"

//发送400 BAD REQUEST页面（连带头部）
void send_400(int cli_sock, Info info);

//发送404 NOT FOUND页面（连带头部）
void send_404(int cli_sock, Info info);

//发送408 REQUEST TIMEOUT页面（连带头部）
void send_408(int cli_sock, Info info);

//发送echo的页面（连带头部）
void send_200_head(int cli_sock);  

//发送501 Not Implemented的页面（连带头部）
void send_501(int cli_sock, Info info);

//发送505 HTTP Version Not Supported的页面（连带头部）
void send_505(int cli_sock, Info info);

/*发送html页面
 *PS:未找到资源发送404 NOT FOUND
 */
void send_html(char* http_uri, int cli_sock, Info info);

//发送echo页面
void send_echo(int cli_sock, char* get_from_cli, Info info);

//发送HEAD页面
void send_HEAD(int cli_sock, Info info);

#endif