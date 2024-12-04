#include "serverregsiter.h"


client f;
int maxid=99;//用户个数



//接受命令
int recvMing(int fd){
    int r;
    int n;
    r = recv(fd,&n,4,0);
    return n;

}

//初始化
void serverinit(int fd){
    int m=recvMing(fd);
    switch(m)
    {
    case 1:denglu(fd);      break;//登录
    case 2:zhuce(fd);       break;//注册
/*     case 3:shang();       break;//上传
    case 4:xiazai();      break;//下载
    case 5:show();        break;//展示 */

    default:printf("说啥嘞，给你一次机会重新说！！\n");  break;
    }
    
}

void serverinit2(int fd){
    printf("3.上传\n");
    printf("4.下载\n");
    printf("5.显示所有文件\n");
    printf("6.删除文件\n");

int m=recvMing(fd);

printf("m:%d\n",m);
switch (m){
    case 3:revfile(f.zhang, fd);                    break;//上传
    case 4:sendfile(f.zhang, fd);                   break;//下载
    case 5:showall(f.zhang, fd);                    break;//展示
    case 6:shanchu( f.zhang, fd);                   break;//删除文件
default:printf("说啥嘞，给你一次机会重新说！！\n");   break;
}

    serverinit2(fd);
}

//展示所有文件
void showall(char filename[],int fd){
    char buff[50]={0};
    
    DIR* dir = opendir(filename);
    if(NULL == dir) {printf("打开目录失败:%m\n");return ;}
    printf("打开目录%m\n");
    
    struct dirent* pDirent = NULL;
    int cnt = 0;
    do{
        pDirent = readdir(dir);
        if(NULL == pDirent) break;
        printf("第%d个文件:%s\n",cnt+1,pDirent->d_name);
        if(cnt>1)//0，1不打印，当前目录和上级目录.  ..   不打印
        {
            int r=send(fd,pDirent->d_name,strlen(pDirent->d_name),0);
            if(-1==r)printf("send failed\n");
            printf("send success\n");
            usleep(10000);
        }
        
        cnt++;
    }while(pDirent);
    char bu[50];
    strcpy(bu,"end");
    int r=send(fd,bu,strlen(bu),0);
    closedir(dir);
    
}





//接受上传文件
void revfile(char filename[],int fd){
    //接受文件名       
    char name[256] = {0};

    
    int r = recv(fd,name,255,0);
    if(-1==r)printf("接受文件名失败\n");
    if(r > 0){
        printf("接收到的文件名为:%s\n",name);
    }
     
     char filepath[256]={0};

    // 构造完整路径
    snprintf(filepath, sizeof(filepath), "%s/%s", filename, name);
  
    //创建新文件（只写）
    int newFd = open(filepath,O_WRONLY|O_CREAT,0666);
    if(-1 == newFd){
         printf("创建新文件:%m\n");
         return ;//,close(fd),exit(-1);
    }
    printf("创建新文件成功!\n");
    //接受文件大小  
    int fileSize=0;        
    r = recv(fd,&fileSize,sizeof(fileSize),0);
    if(r!=4){
        printf("接收文件大小失败:%m\n");
        close(fd);
        close(newFd);
        exit(-1);
    }
    printf("接收到的文件大小为:%d\n",fileSize);                 
    //循环接收文件内容并写入  
    int cnt = 0;
    char buff[1024];
    while(1){
        r = recv(fd,buff,1024,0);
        if(r > 0){
            //写入  
            write(newFd,buff,r);//读到多少写多少
            cnt += r;
            //10.接收完毕结束(接收的字节数等于文件大小）
            if(cnt >= fileSize) break;
        }
    }
    //关闭文件     
    close(newFd);
}


//下载文件
void sendfile(char filename[],int fd){
    //接受文件名       
    char name[256] = {0};
    int r = recv(fd,name,255,0);
    if(-1==r)printf("接受文件名失败\n");
    if(r > 0){
        printf("接收到的文件名为:%s\n",name);
    }
     
     char filepath[256]={0};

    // 构造完整路径
    snprintf(filepath, sizeof(filepath), "%s/%s", filename, name);

    int ff = open(filepath,O_RDONLY);
    if(-1 == ff){
        printf("请确认文件是否存在:%m\n"),close(fd);
        return ;
    }
    
    
    FILE* fp = fdopen(ff,"rb");//重定向
    fseek(fp,0,SEEK_END);
    int fileSize = ftell(fp);//偏移量
    rewind(fp);//重新将光标置于开头
    
    r = write(fd,&fileSize,sizeof fileSize);
    printf("发送%d字节的文件大小%d到接收端!\n",r,fileSize);

    //循环读取文件内容并发送给接受端
    char buff[1024];
    while(1){
        r = fread(buff,1,1024,fp);
        if(r <= 0){printf("接收文件内容：%m\n"); break;}
        send(fd,buff,r,0);//读到多少发多少
        //第三个参数可以可以用strlen(buff)  因为数据中有可能有0也就是'\0'所以用strlen可能导致少发送
    }

    //关闭文件
    fclose(fp);
    
   /*  memset(buff,0,sizeof(buff));
    strcpy(buff,"文件传输成功");
    r=send(whichfd,buff,strlen(buff),0);
    if(-1==r)printf("send failed!\n");
    printf("send success!\n"); */
    printf("文件传输完毕!\n");
}





//接收结构体
void recvsturct(int fd){
    int r;
    r=recv(fd,&f,sizeof(f),0);
}

//登录
void denglu(int fd){ //e为事件events.fd

    recvsturct(fd);
    client sf =f;
    readfile(&maxid,sf,fd);
    
}



//注册   ok啦
void zhuce(int fd){

    recvsturct(fd);
    //printf("账号为：%s",f.zhang);
    client sf =f;
    char namebuff[50];
    strcpy(namebuff,sf.zhang);
    createfile(sf);
    createdir(namebuff);
    //printf("账号为%s",namebuff);
}


//追加写入文件
void createfile(client sf){
    FILE*  fp = fopen("client.data","a");
    int e=fwrite(&sf,1,sizeof(sf),fp);
    if(-1==e)printf("接受文件名失败\n");
    printf("写入成功\n");
    fclose(fp);
    maxid++;//用户个数加一
    
}

//创建用户文件目录
void createdir(char name[50]){
    char filename[50];
    strcpy(filename,name);
    int r=mkdir(filename,0755);
    if(-1==r){
        printf("创建目录失败！\n");
        return ;
    }
    printf("创建%m\n");
    
}

//登录循环读取结构体
void readfile(int *maxid,client sf,int fd){
    int ture=0;//账号是否匹配到了
    client buf;
    FILE*  fp = fopen("client.data","r");
    //循环max个结构体读取
    for(int i=0;i<*maxid;i++){
    fread(&buf,1,sizeof(buf),fp);
    //判断是否相等
    if(0 == strcmp(buf.zhang,sf.zhang)&&0 == strcmp(buf.mi,sf.mi)){
        printf("用户验证成功\n");ture=1;
        fclose(fp);
        break;
        }
    //fseek(fp,sizeof(sf)*(i-1),SEEK_CUR);
    }
        sendsignal(fd,ture);
        if(1==ture)serverinit2(fd);
        else{
            denglu(fd);
        }//进入第二次选择


}


//发送信号（向客户端）
void sendsignal(int fd,int key){

    write(fd,&key,sizeof(key));

}



void munu(int fd){
    printf("请选择进入方式：\n");
    printf("1.管理员\n");
    printf("2.用户\n");
    int d=recvMing(fd);
    switch (d)
    {
    case 1:
    case 2:serverinit(fd);
        break;
    
    default:
        break;
    }
}

//删除
void shanchu(char filename[50],int fd){
    //接受文件名       
    char name[256] = {0};

    
    int r = recv(fd,name,255,0);
    if(-1==r)printf("接受文件名失败\n");
    if(r > 0){
        printf("接收到的文件名为:%s\n",name);
    }
     
     char filepath[256]={0};

    // 构造完整路径
    snprintf(filepath, sizeof(filepath), "%s/%s", filename, name);
     r = remove(filepath);
    if(0 != r)printf("删除失败\n");
    printf("删除成功\n");
    char buff[128]={0};
    //strcpy(buff,"文件删除成功");
    //r=send(fd,buff,strlen(buff),0);
    //if(-1==r)printf("send failed!\n");
    //printf("send success!\n");
}