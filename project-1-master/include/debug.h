/*
 * @Author: your name
 * @Date: 2021-10-18 21:06:17
 * @LastEditTime: 2021-10-21 11:19:56
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\include\debug.h
 */

#define DEBUG

#ifdef DEBUG

#define BUG_PRINTF(format,content) (printf(format,content))
#define BUG_POINT(point) (printf("point: %d\n", point))

#elif
#define BUG_PRINTF(format,content)
#define BUG_POINT(point)

#endif