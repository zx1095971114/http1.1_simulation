/*
 * @Author: your name
 * @Date: 2021-10-15 23:20:07
 * @LastEditTime: 2021-10-15 23:27:01
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\include\file_motion.h
 */

#ifndef FILE_MOTION_H
#define FILE_MOTION_H

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
bool write_file(char* path, char* content);

/**
 * @description: 读取文件中所有内容 
 * @param {char*} path 文件路径
 * @param {char*} content 读取的内容存放位置
 * @param {int} length content的长度
 * @return {*}
 */
bool read_file(char* path, char* content, int length);

/**
 * @description: 删除文件(调用remove,写在这里为了好看) 
 * @param {char*} file_path 文件路径
 * @return {*}
 */
bool rm_file(const char* file_path);

#endif