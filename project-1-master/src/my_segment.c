/*
 * @Author: your name
 * @Date: 2021-10-14 20:48:14
 * @LastEditTime: 2021-10-23 18:07:31
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
#include "my_segment.h"
#include <assert.h>

// #define DEBUG
#include "debug.h"

bool init_Segment_queue(Segment_queue* queue){
    Segment* p = (Segment*) malloc(sizeof(Segment));
    p->message = (char*) malloc(sizeof(char) * 256);
    strcpy(p->message, "0");
    p->next = NULL;

    queue->front = p;
    queue->last = queue->front;
    return TRUE;
}

bool destroy_Segment_queue(Segment_queue* queue){
    while (!queue->front)
    {
        queue->last = queue->front->next;
        free(queue->front);
        queue->front = queue->last;
    }
    return TRUE;
}

bool push(Segment_queue* queue, char* msg){
    Segment* node = (Segment*) malloc(sizeof(Segment));
    node->message = (char*) malloc(sizeof(char) * 2048);
    strcpy(node->message, msg);
    node->next = NULL;
    queue->last->next = node;
    queue->last = node;
    return TRUE;
}

bool pop(Segment_queue* queue, char* msg){
    if(queue->front == queue->last){
        return FALSE;
    }
    Segment* segment = queue->front->next;
    strcpy(msg, segment->message);
    queue->front->next = segment->next;
    if(segment == queue->last){
        queue->last = queue->front;
    }
    free(segment);
    return TRUE;
}


int divide(char* buff, Segment_queue* queue){
    BUG_PRINTF("buff:\n%s", buff);
    
    // //printf("buff: %s", buff);
    //记录被分割的报文数目
    int file_num = 0;
    char* msg;
    //用于标记报文分割位置
    bool isRear = FALSE;
    //用于确定是不是分割到了头部
    bool isHead = TRUE;

    //printf("1\n");
    msg = strtok(buff, "\n");
    //printf("2\n");

    //存每个报文的segment
    char segment[4096];
    memset(segment, 0, sizeof(segment));
    BUG_POINT(10);
    while (msg != NULL)
    {
        BUG_POINT(11);
        char msg_former[256] = {0};
        strcpy(msg_former, msg);

        char msg_copy[1024];
        memset(msg_copy, 0, sizeof(msg_copy));
        strcpy(msg_copy, msg);  
        strcat(msg_copy, "\n");

        BUG_POINT(12);
        //到报文尾部
        if(isRear == TRUE){
            //一个报文入队，清空segment
            BUG_POINT(13);
            strcat(segment, msg_copy);
            push(queue, segment);
            memset(segment, 0, sizeof(segment));
            file_num++;
            isRear = FALSE;
            isHead = TRUE;
        }
        //到报文头部
        else if(isHead){
            BUG_POINT(14);
            strcpy(segment, msg_copy);
            isHead = FALSE;
        }
        //报文中间
        else{
            BUG_POINT(15);
            strcat(segment, msg_copy);
        }

        msg = strtok(NULL, "\n");

        if(msg == NULL){
            if(file_num == 0){
                push(queue, segment);
                memset(segment, 0, sizeof(segment));
                file_num++;
            }
            break;
        }
        
        //前一个msg和后一个msg都是'\r',意味着到尾部了
        if(msg[0] == '\r' && msg_former[0] == '\r'){
            isRear = TRUE;
        }             
        
    }

    return file_num;
}






// int main(int agc, char* argv[]){
    
//     char msg[4096];
//     int fd_in = open("../cp1/sample_request_example", 0);
//     int readRet = read(fd_in,msg,4096);
//     Segment_queue* queue = (Segment_queue*) malloc(sizeof(Segment_queue));
//     init_Segment_queue(queue);
//     BUG_POINT(1);
//     int num = divide(msg, queue);
//     BUG_POINT(2);

//     for(int i = 1; i <= num; i++){
//         char message[2048];
//         memset(message, 0, sizeof(message));
//         pop(queue, message);
//         printf("message: %s\n", message);
//     }
//     BUG_PRINTF("num: %d\n", num);

//     destroy_Segment_queue(queue);
    
// }





// int main(int agc, char* argv[]){
    
//     char msg[4096];
//     int fd_in = open("../cp1/sample_request_example", 0);
//     int readRet = read(fd_in,msg,4096);
//     divide(msg);

//     // char m[50] = {0};
//     // read_file("/home/project-1-master/pipelining/recall1", m, 50);
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


// int divide(char* buff){
    
//     // //printf("buff: %s", buff);
//     //记录被分割的报文数目
//     int file_num = 0;
//     char* msg;
//     //用于标记报文分割位置
//     bool isRear = FALSE;

//     //printf("1\n");
//     msg = strtok(buff, "\n");
//     file_num++;
//     //printf("2\n");
//     while (msg != NULL)
//     {
//         char msg_former[256] = {0};
//         strcpy(msg_former, msg);
//         //printf("3\n");

//         char file_path[50] = "/home/project-1-master/pipelining/recall";
//         char sfile_num[5] = {0};
//         sprintf(sfile_num, "%d", file_num);
//         strcat(file_path, sfile_num);

//         char msg_copy[512];

//         strcpy(msg_copy, msg);
        
//         strcat(msg_copy, "\n");
        
//         write_file(file_path, msg_copy);
//         // printf("msg_copy: %s", msg_copy);
//         // free(msg_copy);
        
        
//         msg = strtok(NULL, "\n");
        
        
//         if(isRear == TRUE){
//             file_num++;
//             isRear = FALSE;
//         }   

//         if(msg == NULL){
//             break;
//         }
//         if(msg[0] == '\r' && msg_former[0] == '\r'){
//             isRear = TRUE;
//         }
//     }

//     return file_num;
// }