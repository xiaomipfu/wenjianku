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
#include <sys/select.h>
#include "client.h"
//#include "client.h"
//#include "client.h"
//enum gongneng {"登录","注册","上传文件","下载文件","删除文件"};

int main(int argc,char* argv[]){
    if(argc <= 1){
        printf("请输入端口号!\n");
        return -1;
    }
    int port = atoi(argv[1]);
    //1
    int clientFd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == clientFd) printf("创建socketfd失败:%m\n"),exit(-1);
    printf("创建socketfd %m\n");
    //2 服务器的协议地址簇
    struct sockaddr_in sAddr={0};
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = inet_addr("192.168.170.128");
    sAddr.sin_port = htons(port);//使用1W左右的
    //3 连接服务器
    int r = connect(clientFd,(struct sockaddr*)&sAddr,sizeof sAddr);
    if(-1 == r) printf("连接服务器失败:%m\n"),close(clientFd),exit(-1);
    printf("连接服务器:%m\n");

    //4 通信
    fd_set set;
    int maxFd;
    while(1){//循环监视
        //准备好要监视的描述符号
        FD_ZERO(&set);

        FD_SET(clientFd,&set);
        maxFd=clientFd;
        FD_SET(1,&set);
        maxFd = (maxFd>1) ? maxFd : 1;

        //开始监视
        r = select(maxFd+1,&set,NULL,NULL,NULL);
        if(-1 == r){
            printf("监视失败:%m!\n");
            return -1;
        }else if (0 == r){
            //printf("没有动静!\n");
        }else{
            if(FD_ISSET(1,&set)){//用户输入了

                munu(clientFd);
               // printf("向服务器发送了%d字节数据!\n",r);
            }
            
            char buff[1024];
            if(FD_ISSET(clientFd,&set)){//服务器发数据过来了
                r = recv(clientFd,buff,1023,0);
                if(r>0){
                    buff[r] = 0;//添加结束符号
                    printf("%s\n",buff);
                }else{
                    printf("服务器关闭了!\n");
                    close(clientFd);
                    return -1;
                }
            }
        }
    }
 return 0;
} 