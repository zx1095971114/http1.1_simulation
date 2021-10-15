/*
 * @Author: your name
 * @Date: 2021-10-15 23:24:01
 * @LastEditTime: 2021-10-15 23:49:54
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\include\segment.h
 */
#ifndef MY_SEGMENT_H
#define MY_SEGMENT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>    
#include <fcntl.h>
#include <unistd.h>
#include "my_bool.h"

/**
 * @description: 分割报文
 * @param {char*} buff 要分割的报文
 * @return {*} 分割出的文件数量
 */
int divide(char* buff);


#endif