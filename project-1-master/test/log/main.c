/*
 * @Author: your name
 * @Date: 2021-10-08 16:54:02
 * @LastEditTime: 2021-10-08 17:01:23
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\test\log\main.c
 */
#include <stdio.h>
#include "logTest.h"
 
int test_func1()
{
	LOG_INFO(LOG_DEBUG, "%s", "hello world!!"); 
	return 0;
}
 
int main(int argc, char *argv[])
{
	// LOG_SetPrintDebugLogFlag(1);//打印调试信息
	LOG_SetPrintLogPlaceFlag(1);//保存打印信息到文件
	LOG_Init("info", 8000);
	
	LOG_INFO(LOG_DEBUG, "%s", "Init log!!");
	
	test_func1();
	
	LOG_INFO(LOG_DEBUG, "%s", "Destroy log!!");
	LOG_Destroy();
}