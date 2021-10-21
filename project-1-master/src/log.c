/*
 * @Author: 周翔
 * @Date: 2021-10-08 17:04:34
 * @LastEditTime: 2021-10-17 10:35:22
 * @LastEditors: Please set LastEditors
 * @Description: 用于记录日志信息
 * @FilePath: \project-1-master\src\log.c
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "log.h"

//获取当前时间
void getTime(char* nowTime){
    time_t timep;
    struct tm* timeStruct;

    time(&timep);
    timeStruct = localtime(&timep);

    strftime(nowTime, 24, "%Y-%m-%d %H:%M:%S", timeStruct);
}


/**
 * @description:记正常日志 
 * @param {Info} info 日志信息
 * @return {*}
 */
void commonLog(Info info){
    const char* fileName = "/home/project-1-master/log/access.log"; 
    FILE* fp = fopen(fileName,"ab");
    setbuf(fp,NULL);
    if(fp == NULL){
        printf("打开access.log文件失败");
        return;
    }

    char log[128];
    strcpy(log, info.IP);
    strcat(log, "--[");

    char time[24] = {0};
    getTime(time);
    strcat(log, time);

    strcat(log, " +0800] \"");
    strcat(log, info.method);
    strcat(log, " /");
    strcat(log, info.version);
    strcat(log, "\" ");

    char code[10] = {0};
    sprintf(code, "%d", info.code);
    strcat(log, code);
    strcat(log, " ");
    
    char length[30] = {0};
    sprintf(length, "%d", info.fileLength);
    strcat(log, length);
    strcat(log, "\n");

    printf("===========================\n");
    printf("%s", log);
    printf("\n");
    int a = fprintf(fp, "%s", log);
    fflush(fp);
    printf("写入字符数:%d\n", a);
    printf("============================\n");
    fclose(fp);
}


/**
 * @description: 记错误日志
 * @param {Info} info请求信息
 * @return {*}
 */
void errorLog(Info info){
    const char* fileName = "/home/project-1-master/log/error.log"; 
    FILE* fp = fopen(fileName,"ab");
    setbuf(fp,NULL);
    if(fp == NULL){
        printf("打开error.log文件失败");
        return;
    }

    char log[256];
    strcpy(log, "[");

    char time[24] = {0};
    getTime(time);
    strcat(log, time);

    strcat(log, " +0800] [");
    strcat(log, info.seriousness);
    strcat(log, "] [client ");
    strcat(log, info.IP);
    strcat(log, "]\n");
    
    switch (info.code)
    {
    case 400:
        strcat(log, "400: The format of the request is wrong!\n");
        break;
    case 404:
        strcat(log, "404: Files does not exist: ");
        strcat(log, info.path);
        strcat(log, "\n");
        break;
    case 408:
        strcat(log, "408: The request is timed out!\n");
        break;
    case 501:
        strcat(log, "501: The method is not implemented: ");
        strcat(log, info.method);
        strcat(log, "\n");
        break;
    case 505:
        strcat(log, "505: The version of the client does not match the server's: ");
        strcat(log, info.version);
        strcat(log, "\n");
        break;
    default:
        break;
    }
    strcat(log, "\n");

    fprintf(fp, "%s", log);
    fflush(fp);
    fclose(fp);
}

// int main(){
//     Info info;
//     info.fileLength = 60;
//     info.IP = "127.0.0.1";
//     info.method = "GET";
//     info.version = "HTTP/1.1";
//     info.code = 200;
//     info.path = "/var/www/html/test.html";
//     info.seriousness = "error";
//     // printf("1\n");
//     commonLog(info);
// }
