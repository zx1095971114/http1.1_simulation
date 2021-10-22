/*
 * @Author: your name
 * @Date: 2021-10-15 23:24:01
 * @LastEditTime: 2021-10-22 22:04:59
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

typedef struct segment{
    char* message;
    struct segment* next; 
}Segment;

typedef struct segment_queue
{
    Segment* front;
    Segment* last;
}Segment_queue;

/**
 * @description: 初始化队列
 * @param {Segment_queue*} queue
 * @return {*}
 */
bool init_Segment_queue(Segment_queue* queue);

/**
 * @description: 销毁队列
 * @param {Segment_queue*} queue
 * @return {*}
 */
bool destroy_Segment_queue(Segment_queue* queue);

/**
 * @description: 根据数据创建元素，元素入队
 * @param {Segment_queue*} queue
 * @param {char*} msg
 * @return {*}
 */
bool push(Segment_queue* queue, char* msg);

/**
 * @description: 出队
 * @param {Segment_queue*} queue
 * @param {char*} msg
 * @return {*}
 */
bool pop(Segment_queue* queue, char* msg);


/**
 * @description: 分割报文,注意分割完后buff会被改变!!!!!!!
 * @param {char*} buff 要分割的报文
 * @param {Segment_queue*} queue 分割好的报文存在的队列
 * @return {*} 分割出的报文数量
 */
int divide(char* buff, Segment_queue* queue);


#endif