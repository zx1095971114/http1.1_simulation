/*
 * @Author: your name
 * @Date: 2021-10-09 02:45:29
 * @LastEditTime: 2021-10-15 00:04:02
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\test\log\main.c
 */
#include <stdio.h>
// #pragma warning(disable:4996)
#include <stdlib.h>
#include <string.h>  
 
 
int main()
{
	char str[] = "我,,是,,中国,,程序员";
	char *ptr;
	char *p;
	printf("开始前:  str=%s\n", str);
	printf("开始分割:\n");
	ptr = strtok(str, ",,");
	while (ptr != NULL) {
		printf("ptr=%s\n", ptr);
		printf("str=%s\n", str);
		ptr = strtok(NULL, ",,");
	}
	return 0;
}