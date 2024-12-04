#include <sys/types.h>          
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 #include <pthread.h>
 #include <fcntl.h>
#include <sys/epoll.h>
#pragma once
#pragma comment(lib,"Ws2_32.lib")



#define SIZE 50

typedef struct Client{
    char zhang[50];
    char mi[50];
}client;
client f;

//注册
void zhuce();
//登录
void denglu(int fd);

void init();

//账号密码存入结构体
void cunstuct(char z[SIZE],char m[SIZE]);


//发送结构体
void sendstuct(client f,int fd);

//发送信号
void sendsignal(int  n,int fd);


//二级目录
void init2(int fd);




//上传文件               
void shangchuan(int fd);


//下载文件
void xiazai(int fd);


//管理员模式和用户模式
void munu();


//查看所有文件
void chakan(int fd);



//删除文件
void shanchu(int fd);