/*
 * @Author: your name
 * @Date: 2021-09-29 00:49:48
 * @LastEditTime: 2021-10-07 21:39:23
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\src\parse.c
 */
#include "parse.h"
#include <string.h>
#define COMPARE_CONNECTION(ptr) (ptr[0] == 'C' && ptr[1] == 'o' && ptr[2] == 'n' && ptr[3] == 'n' && ptr[4] == 'e' && ptr[5] == 'c' && ptr[6] == 't' && ptr[7] == 'i' && ptr[8] == 'o' && ptr[9] == 'n')
#define COMPARE_CLOSE(ptr) (ptr[0] == 'c' && ptr[1] == 'l' && ptr[2] == 'o' && ptr[3] == 's' && ptr[4] == 'e')


/**
* Given a char buffer returns the parsed request headers
*/
Request * parse(char *buffer, int size, int socketFd) {
  //Differant states in the state machine
	enum {
		STATE_START = 0, STATE_CR, STATE_CRLF, STATE_CRLFCR, STATE_CRLFCRLF
	};

	int i = 0, state;
	size_t offset = 0;
	char ch;
	char buf[8192];//读入的报文存放的位置
	memset(buf, 0, 8192);

	//首部字段的数目
	int headers_num = 0;
	/*
	* 读入报文至buf中
	*/
	state = STATE_START;
	while (state != STATE_CRLFCRLF) {
		char expected = 0;

		if (i == size)
			break;

		ch = buffer[i];
		i++;
		buf[offset] = ch;
		offset++;

		switch (state) {
		case STATE_START:
			expected = '\r';
			break;
		case STATE_CRLF:
			headers_num++;
			expected = '\r';
			break;
		case STATE_CR:
			expected = '\n';
			break;
		case STATE_CRLFCR:
			expected = '\n';
			break;
		default:
			state = STATE_START;
			continue;
		}

		if (ch == expected)
			state++;
		else
			state = STATE_START;

	}

	headers_num = headers_num - 1;

    //Valid End State
	if (state == STATE_CRLFCRLF) {
		printf(buf);
		printf("\n");
		Request *request = (Request *) malloc(sizeof(Request));
        request->header_count=0;
		//isPermanent属性用以标识是否要持续连接
		request->isPermanent = 1;

        //TODO You will need to handle resizing this in parser.y
        request->headers = (Request_header *) malloc(sizeof(Request_header)*headers_num);
		set_parsing_options(buf, i, request);

		if (yyparse() == SUCCESS) {
			printf("=======================================\n");
			for(int index = 0; index < request->header_count; index++){
				if(COMPARE_CONNECTION(request->headers[index].header_name) && COMPARE_CLOSE(request->headers[index].header_value)){
					request->isPermanent = 0;
				}
			}
            return request;
		}
		else{
			return NULL;
		}
	}
    //TODO Handle Malformed Requests
    printf("Parsing Failed\n");
	return NULL;
}

