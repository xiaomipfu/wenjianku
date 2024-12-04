#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "serverregsiter.h"

#define NUM  1024

int serverFd;
//用来epoll_ctl注册事件的结构体
struct epoll_event  ev;
//接受事件返回状态的结构体
struct epoll_event  events[NUM] = {0};
int clientFdsIdx = 1;

//转发需要多一个数组
int clientFds[NUM];
int cfdIdx = 0;


void myExit();

void hand();

int main(int argc,char* argv[]){
    if(argc <= 1){
        printf("请输入端口号!\n");
        return -1;
    }

    for(int i=0;i<NUM;i++){
        clientFds[i] = -1;

    }

    int port = atoi(argv[1]);
    

    signal(2,hand);
   
    //1
     serverFd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == serverFd) printf("创建socketfd失败:%m\n"),exit(-1);
    printf("创建socketfd %m\n");
    //2 协议地址簇
    struct sockaddr_in sAddr={0};
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = inet_addr("192.168.170.128");
    sAddr.sin_port = htons(port);//使用1W左右的

    //3 绑定

    int r = bind(serverFd,(struct sockaddr*)&sAddr,sizeof sAddr);
    if(-1 == r) printf("绑定失败:%m\n"),close(serverFd),exit(-1);
    printf("绑定 %m\n");

    //4 监听
    r = listen(serverFd,10);
     if(-1 == r) printf("监听失败:%m\n"),close(serverFd),exit(-1);
    printf("监听 %m\n");
   
    
    //服务器的描述符号纳入监视范围
    ev.data.fd = serverFd;
    ev.events = EPOLLIN;


    int epfd = epoll_create(2);
    if(-1 == epfd) printf("创建epoll失败:%m\n"),exit(-1);
    printf("创建epoll成功!\n");

    //epoll_ctl 把服务器的fd加入监视队列
    r = epoll_ctl(epfd,EPOLL_CTL_ADD,serverFd,&ev);
    if(-1 == r) printf("注册服务器描述符号监视失败:%m\n"),exit(-1);
    printf("注册服务器描述符号监视%m!\n");


    struct sockaddr_in cAddr={0};
    int addr_len=0;
    

    int cfd;

    char buff[1024];
    char temp[1024];
    int n;
    int nfds;
    while(1){
        //第三个参数是允许的最大事件个数
        nfds = epoll_wait(epfd,events,1024,0);//非阻塞 
        if(0 == nfds){
            //printf("木有动静!\n");
        }else if(-1 == nfds){
            printf("监视失败，被发现了，赶紧跑路:%m!\n");
            myExit();
        }else{
                //检查是否哪个客户端fd有动静
                for(int i=0;i<nfds;i++){
                    if(serverFd == events[i].data.fd){
                        //printf("有客户端连接事件:r:%d %d!\n",r,serverFd);
                        cfd = accept(serverFd,NULL,NULL);  
                        if(-1 == cfd){
                            printf("服务器崩溃:%m!");
                            myExit();
                        }
                        printf("%d连接上服务器了!\n",cfd);
                        clientFdsIdx++; 
                        //新连接的客户端cfd要纳入监视集合    这里可能有bug 因为可能epoll_wait需要每次都把所有描述符号都epoll_ctl（注册）一下
                        clientFds[cfdIdx++] = cfd;
                        ev.data.fd = cfd;
                        ev.events = EPOLLIN;//EPOLLET
                        r = epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
                        if(-1 == r) printf("注册客户端描述符号%d监视失败:%m\n",cfd),myExit();
                        //printf("注册客户端描述符号%d监视%m!\n",cfd);
                    }
                    else{
                        printf("%d客户端发数据过来!\n",events[i].data.fd);

                        munu(events[i].data.fd);//进入初始化函数



                        if(r > 0){
                            //buff[r] = 0;
                            printf("服务器收到来自%d的数据:%d\n",
                               events[i].data.fd,n);
                            //转发给当前连上服务器的所有客户端
                            //准备好要转发的数据
                            /* memset(temp,0,1024);
                            sprintf(temp,"%d:%s",events[i].data.fd,buff);

                            //转发发送到当前客户端
                            write(clientFds[i],temp,strlen(temp)); */



                            /* //群发
                            for(int j=0;j<NUM;j++){
                                /*if(events[j].data.fd != 0 && 
                                    events[j].data.fd != -1 &&
                                    events[j].data.fd != serverFd){
                                if(clientFds[j]!=-1){
                                    //send(ev[j].data.fd,temp,strlen(temp),0);
                                    write(clientFds[j],temp,strlen(temp));
                                }
                            } */
                            break;//结束当前层for循环
                        }else{//客户端主动断开连接
                            printf("%d已下线!\n",events[i].data.fd);
                            //服务器关闭客户端fd
                            close(events[i].data.fd);
                            //数组里重新设置为-1
                            events[i].data.fd = -1;
                            clientFds[i-1] = -1;
                            break;//结束当前层for循环
                        }
                    }//end of if(-1 != clientFds[i] && FD_ISSET(clientFds[i],&fds))
             }   //end of for(int i=0;i<NUM;i++){
        }//end of epoll_wait 返回值判断 if else
    }//end of while(1)
    return 0;
}


void myExit(){
   /*  close(serverFd);
    for(int i=1;i<clientFdsIdx;i++)
        if(-1 != ev[i].data.fd) 
            close(ev[i].data.fd); */
    exit(-1);
}

void hand(){
    /* //和所有客户端说分手
    for(int i=1;i<clientFdsIdx;i++)
        if(-1 != ev[i].data.fd) 
            close(ev[i].data.fd); */
    //服务器下线
    close(serverFd);
    exit(-2);
}