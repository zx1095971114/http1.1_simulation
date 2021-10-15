/*
 * @Author: your name
 * @Date: 2021-10-14 20:48:14
 * @LastEditTime: 2021-10-16 01:15:32
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\src\segment.c
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>    
#include <fcntl.h>
#include <unistd.h>
#include "my_bool.h"
#include "file_motion.h"


int divide(char* buff){
    // printf("buff: %s", buff);
    //记录被分割的报文数目
    int file_num = 0;
    char* msg;
    //用于标记报文分割位置
    bool flag = FALSE;

    msg = strtok(buff, "\n");
    file_num++;
    while (msg != NULL)
    {
        char msg_former[45] = {0};
        strcpy(msg_former, msg);

        char file_path[50] = "/home/project-1-master/pipelining/recall";
        char sfile_num[5] = {0};
        sprintf(sfile_num, "%d", file_num);
        strcat(file_path, sfile_num);

        // printf("msg: %s", msg);
        // strcat(msg, "\n");
        // char* msg_copy = (char*) malloc(sizeof(char) * (strlen(msg) + 1));
        char msg_copy[45] = {0};
        strcpy(msg_copy, msg);
        strcat(msg_copy, "\n");
        write_file(file_path, msg_copy);

        // printf("msg_length: %ld", strlen(msg));
        msg = strtok(NULL, "\n");
        
        if(flag == TRUE){
            file_num++;
            flag = FALSE;
        }   

        if(msg == NULL){
            break;
        }
        if(msg[0] == '\r' && msg_former[0] == '\r'){
            flag = TRUE;
        }
    }

    return file_num - 1;
}

// int main(int agc, char* argv[]){
//     char msg[4096];
//     int fd_in = open("../cp1/sample_request_realistic", 0);
//     int readRet = read(fd_in,msg,4096);
//     divide(msg);
//     char m[50] = {0};
//     read_file("/home/project-1-master/pipelining/recall1", m, 50);
// }










// typedef struct segment{
//     char* message;
//     struct segment* next; 
// }Segment;

// typedef struct segment_queue
// {
//     Segment* front;
//     Segment* last;
// }Segment_queue;

// bool init_Segment_queue(Segment_queue* queue){
//     Segment* p = (Segment*) malloc(sizeof(Segment));
//     p->message = (char*) malloc(sizeof(char) * 256);
//     strcpy(p->message, "0");
//     p->next = NULL;

//     queue->front = p;
//     queue->last = queue->front;
//     return TRUE;
// }

// bool destroy_Segment_queue(Segment_queue* queue){
//     while (!queue->front)
//     {
//         queue->last = queue->front->next;
//         free(queue->front);
//         queue->front = queue->last;
//     }
//     return TRUE;
// }

// bool push(Segment_queue* queue, char* msg){
//     Segment* node = (Segment*) malloc(sizeof(Segment));
//     node->message = (char*) malloc(sizeof(char) * 256);
//     strcpy(node->message, msg);
//     node->next = NULL;
//     queue->last->next = node;
//     queue->last = node;
//     return TRUE;
// }

// bool pop(Segment_queue* queue, char* msg){
//     if(queue->front == queue->last){
//         return FALSE;
//     }
//     strcpy(msg, queue->front->next->message);
//     free(queue->front);
//     queue->front = queue->front->next;
//     return TRUE;
// }