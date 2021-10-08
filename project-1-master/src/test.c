/*
 * @Author: your name
 * @Date: 2021-10-09 02:08:18
 * @LastEditTime: 2021-10-09 02:08:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\src\test.c
 */
#include "log.h"

int main(){
    Info info;
    info.fileLength = 60;
    info.IP = "127.0.0.1";
    info.method = "GET";
    info.version = "HTTP/1.1";
    info.code = 200;
    info.path = "/var/www/html/test.html";
    info.seriousness = "error";
    // printf("1\n");
    commonLog(info);
}