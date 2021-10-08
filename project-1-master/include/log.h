/*
 * @Author: 周翔
 * @Date: 2021-10-09 00:04:06
 * @LastEditTime: 2021-10-09 00:39:59
 * @LastEditors: Please set LastEditors
 * @Description: 日志头文件
 * @FilePath: \project-1-master\include\log.h
 */


#ifndef LOG_H
#define LOG_H
/**
 * @description:日志信息 
 */
typedef struct info{
    //用户IP
    char* IP;
    //方法
    char* method;
    //发送给客户的回应报文的实体长度
    int fileLength;
    //请求的uri
    char* path;
    //回应报文的状态码
    int code ;
    //错误的严重程度
    char* seriousness;
    //用户版本号
    char* version;
}Info;

/**
 * @description:记正常日志 
 * @param {Info} info 日志信息
 * @return {*}
 */
void commonLog(Info info);

/**
 * @description: 记错误日志
 * @param {Info} info请求信息
 * @return {*}
 */
void errorLog(Info info);

#endif