/*
 * @Author: your name
 * @Date: 2021-09-30 17:37:20
 * @LastEditTime: 2021-10-03 10:51:19
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project-1-master\myLexAndYacc\test.c
 */
#include <stdio.h>

typedef struct MyStruct
{
    int key1;
    int key2;
}MyStruct;


int sum(void){
    return 1;
}

int main(int argv, char* argc[]){
    printf("%d", sum());
    
}




//test.y
%{
int yylex(void);
void yyerror(char* str); 
%}

%token DEFIN_INT DEFIN_DOUBLE,DEFIN_CHAR 

%%

%%

void yyerror(char* str){
    fprintf(stderr, "error: %s", str);
}

int yywrap(){
    return 1;
}

int main(){
    yyparse();
}

//test.l
