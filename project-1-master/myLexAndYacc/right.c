
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
//仿写http服务器,需要关闭httpd以及使用root用户操作
 
void SendError(int c)  //发送错误页面（连带头部）
{
	char buff[1024]={0};     
	strcpy(buff,"HTTP/1.1 404 NOT FOUND\n\r");
	strcat(buff,"Server:myhttp/1.0\n\r");
	strcat(buff,"Content-Length: ");
	sprintf(buff+strlen(buff),"%d",0);
	strcat(buff,"\n\r");
	strcat(buff,"Content-Type:text/html;charset=utf-8\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	strcat(buff,"404 NOT FOUND");  //发送给客户端的数据，用于显示
	send(c,buff,strlen(buff),0);
}
void SendHead(int size,int c)  //发送给客户端应答报头
{
	char buff[1024]={0};
	strcpy(buff,"HTTP/1.1 200 OK\n\r");
	strcat(buff,"Server:myhttp/1.0\n\r");
	strcat(buff,"Content-Length: ");
	sprintf(buff+strlen(buff),"%d",size);
	strcat(buff," \n\r");
	strcat(buff,"Content-Type:text/html;charset=utf-8\n\r");
	strcat(buff,"\n\r");   //空行标识数据部分和头部分开
	send(c,buff,strlen(buff),0);
}
void AnayRequest(char *buff,char *pathname) //解析参数获取到请求的页面地址
{
	//GET  /index.html HTTP/1.1   Linux上获取到的请求第一行，并没有域名及IP
	char *p=strtok(buff," ");
	p=strtok(NULL," ");   //获取到了/index.html
	strcpy(pathname,"/var/www/html");  //请求的页面在本机的var/www/html底下
	strcat(pathname,p); //将分割的页面名称与之相连接，得到请求页面的绝对地址
}
void SendData(char *pathname,int c)  //发送页面
{
	struct stat st;
	if(-1!=stat(pathname,&st))  //通过路径获取文件属性
	{
		SendHead(st.st_size,c);  //发送应答报头，传入请求页面文件的大小
	}
	else
	{
		SendError(c);  //所有错误都发送这个404错误（只发送一句话而已）
		return;
	}
	int fd=open(pathname,O_RDONLY);//发送页面正文给客户端进行显示，即请求的页面
	if(fd==-1)
	{
		SendError(c);
		return;
	}
	while(1)
	{
		char buff[128]={0};
		int n=read(fd,buff,127);
		if(n<=0)
		{
			close(fd);
			break;
		}
		send(c,buff,strlen(buff),0); //发送页面内容给客户端
	}
}
 
 
int main()
{
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd!=-1);
 
	struct sockaddr_in ser,cli;
	ser.sin_family=AF_INET;
	ser.sin_port=htons(9999);
	ser.sin_addr.s_addr=inet_addr("127.0.0.1");  //本地回环地址
 
	int res=bind(sockfd,(struct sockaddr *)&ser,sizeof(ser));
	assert(res==0);
	listen(sockfd,5);
 
	while(1)
	{
		int len=sizeof(cli);
		int c=accept(sockfd,(struct sockaddr*)&cli,&len);
		if(c<0)
			continue;
		char buff[1024]={0};   //短连接不需要循环读取
		int n=recv(c,buff,1023,0);
		if(n<=0)
		{
			close(c);
			continue;
		}
		printf("%s\n",buff);  //将客户端HTTP请求内容输出
		char pathname[128]={0};
		AnayRequest(buff,pathname);  //分析参数
		SendData(pathname,c);   //发送请求的html网页
		close(c);
	}
}
