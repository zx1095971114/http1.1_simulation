/*
 * @Author: your name
 * @Date: 2021-09-30 17:37:20
 * @LastEditTime: 2021-10-07 09:11:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\myLexAndYacc\test.c
 */
#include <string.h>
#include <stdio.h>
 
int main () {
   char str[80] = "This is - www.runoob.com - website";
   const char s[2] = "-";
   char *token;
   
   /* 获取第一个子字符串 */
   token = strtok(str, s);
   
   /* 继续获取其他的子字符串 */
   while( token != NULL ) {
      printf( "%s\n", token );
    
      token = strtok(NULL, s);
   }
   
   return(0);
}

