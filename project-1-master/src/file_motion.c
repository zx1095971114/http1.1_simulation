/*
 * @Author: your name
 * @Date: 2021-10-15 22:16:43
 * @LastEditTime: 2021-10-21 23:59:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\src\file_motion.c
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>    
#include <fcntl.h>
#include <unistd.h>
#include "my_bool.h"


/**
 * @description: 向文件中写入 
 * @param {char*} path 文件路径
 * @param {char*} content 写入内容
 * @return {*}
 */
bool write_file(char* path, char* content){

    // printf("path: %s\n", path);
    // printf("content: %s\n", content);

    FILE* fp = fopen(path, "ab");
    //printf("ok1\n");
    //printf("%p\n",fp);
    setbuf(fp, NULL);
    //printf("ok2\n");
    fprintf(fp, "%s", content);
    fflush(fp);
    fclose(fp);
    return TRUE;
}


/**
 * @description: 读取文件中所有内容 
 * @param {char*} path 文件路径
 * @param {char*} content 读取的内容存放位置
 * @param {int} length content的长度
 * @return {*} open的返回值
 */
// int read_file(char* path, char* content, int length){
//     int fd_in = open(path, 0);
//     read(fd_in, content, length);
//     // close(fd_in);
//     return fd_in;
// }

/**
 * @description: 删除文件(调用remove,写在这里为了好看) 
 * @param {char*} file_path 文件路径
 * @return {*}
 */
bool rm_file(const char* file_path){
    return remove(file_path);
}

// int main(int argc, char* argv[]){
//     char path[50] = "/home/project-1-master/pipelining/recall";
//     char content[20] = "success\n";
//     write_file(path, content);
//     char buff[30] = {0};
//     read_file(path, buff, 30);
//     printf("buff: %s\n", buff);
//     rm_file(path);
// }