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
#include <dirent.h>
 #include <sys/stat.h>
#pragma once
#pragma comment(lib,"Ws2_32.lib")


typedef struct Client{
    char zhang[50];
    char mi[50];
}client;


client f;


//初始化
void serverinit(int fd);



//初始化第二次选择
void serverinit2(int fd);



//登录
void denglu(int fd);

//注册
void zhuce(int fd);

//接收结构体
void recvsturct(int fd);


//追加写入文件
void createfile(client sf);


//读取结构体
void readfile(int *maxid,client sf,int fd);

//接受命令
int recvMing(int fd);

//发送信号（向客户端）
void sendsignal(int fd,int key);


//接受上传文件
void revfile(char filename[],int fd);



//下载文件
void sendfile(char filename[],int fd);


//展示所有文件
void showall(char filename[],int fd);


//创建用户文件目录
void createdir(char name[50]);


//管理员普通用户选择
void munu(int fd);



//删除
void shanchu(char filename[50],int fd);