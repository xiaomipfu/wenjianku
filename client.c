#include "client.h"



//发送信号
void sendsignal(int  n,int fd){
    
    int r = send(fd,&n,sizeof(n),0); 

    //printf("向服务器发送了%d字节数据!\n",r);      
}



//二级目录
void init2(int fd){
   
    printf("请选择：\n");
    printf("3.上传文件\n");
    printf("4.下载文件\n");
    printf("5.展示所有文件\n");
    printf("6.删除文件\n");
    int m;
    scanf("%d",&m);
    
    switch(m){

        case 3:sendsignal(m,fd);shangchuan(fd);     break;//上传
        case 4:sendsignal(m,fd);xiazai(fd);         break;//下载 
        case 5:sendsignal(m,fd);chakan(fd);         break;//查看全部文件
        case 6:sendsignal(m,fd); shanchu( fd);   break;//删除文件
    

    default:printf("说啥嘞，给你一次机会重新说！！\n");  break;
    }
    init2( fd);
}






//查看所有文件
void chakan(int fd){
    //printf("-----------\n");
    char buff[1024]={0};
     char temp[1024];
     strcpy(temp,"end");
    printf("已存储的文件：\n");
    while(1){
        
        int r = recv(fd,buff,1023,0);
        if(r>0){
            buff[r] = 0;//添加结束符号
            if(0==strcmp(buff,temp)){
                printf("文件传输成功！\n");
                break;
            }
            printf("%s\n",buff);
        }else if(0==r){
            printf("文件名接收完毕\n");
            break;
        }else{
            printf("接收失败\n");
        }
    }
}




//上传文件               
void shangchuan(int fd){
     // 获取文件名
    char filename[256] = {0};
    printf("请输入要发送的文件名:");
    scanf("%s",filename);
    //发送文件名
    int r=send(fd,filename,strlen(filename),0);
    if(-1==r)printf("send failed\n");
    // 获取文件大小
    int ff = open(filename,O_RDONLY);
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
        if(r <= 0) break;
        send(fd,buff,r,0);//读到多少发多少
        //第三个参数可以可以用strlen(buff)  因为数据中有可能有0也就是'\0'所以用strlen可能导致少发送
    }

    //关闭文件
    fclose(fp);
    printf("文件传输完毕!\n");
}


//下载文件
void xiazai(int fd){
    //要下载的文件名       
    char name[256] = {0};
    printf("要下载文件名：\n");
    scanf("%s",name);
    int r = send(fd,name,255,0);
    if(r > 0){
        printf("发送文件名:%s成功\n",name);
    }
    
    //创建新文件（只写）   
    int newFd = open(name,O_WRONLY|O_CREAT,0666);
    if(-1 == newFd) printf("创建新文件失败:%m\n"),close(fd),exit(-1);
    printf("创建新文件成功!\n");
    //接受文件大小  
    int fileSize=0;        
    r = recv(fd,&fileSize,sizeof(fileSize),0);
    if(r!=4){
        printf("接收文件大小:%m\n");
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
            if(cnt > fileSize){
               // printf("接受文件成功\n");
            } break;
        }
    }
    //关闭文件     
    close(newFd);
}  














/* //第二次选择
void init2(int fd){
    int m;//命令
    printf("请选择操作方式：\n");
    printf("3.上传\n");
    printf("4.下载\n");
    printf("5.展示所有文件\n");
    
    scanf("%d",&m);
    //
    switch(m){

        case 3:shangchuan(fd);     break;//登录
        /* case 4:sendsignal(m,fd);zhuce();  sendstuct(f, fd);     break;//注册
        case 5:sendsignal(m,fd);zhanshi();  sendstuct(f, fd);   break;//注册 
        default:printf("说啥嘞，给你一次机会重新说！！");  break;
    }
    } */
void init(int fd){
    int m;
    printf("请选择进入方式：\n");
    printf("1.登录\n");
    printf("2.注册\n");
    
    scanf("%d",&m);


    //
    switch(m){

        case 1:sendsignal(m,fd);denglu(fd) ;     break;//登录
        case 2:sendsignal(m,fd);zhuce();  sendstuct(f, fd);     break;//注册
        default:printf("说啥嘞，给你一次机会重新说！！\n");  break;
    }

}



//删除文件
void shanchu(int fd){
     // 获取文件名
    char filename[256] = {0};
    printf("请输入要删除的文件名:");
    scanf("%s",filename);
    //发送文件名
    int r=send(fd,filename,strlen(filename),0);
    if(-1==r)printf("send failed\n");
    printf("send success\n");
}



//管理员模式和用户模式
void munu(int fd){
    printf("请选择进入方式：\n");
    printf("1.管理员\n");
    printf("2.用户\n");
    int d;
    scanf("%d",&d); 
    sendsignal(d,fd);
        switch(d){

        case 1:               break;//管理员
        case 2:init(fd);         break;//普通用户 
        }

}


//登录
void denglu(int fd){



    int shou;
    //int t=1;
    char z[50];
    char m[50];
    printf("请输入账号：");
    scanf("%s",z);
    strcpy(f.zhang,z);
    printf("请输入密码：");
    scanf("%s",m);
    cunstuct(z, m);//存入结构体
    sendstuct(f, fd);//发送信号
    int r = recv(fd,&shou,sizeof(shou),0);
    if(r>0){
            if(shou==1){
            printf("登录成功\n");
            init2(fd);
            }else if(shou ==0){
                printf("登录失败，请重新输入！\n");
                denglu( fd);
                
            }
            }
}




//注册账号
void zhuce(){
    char z[50];
    char m[50];
    printf("请输入要注册的账号：\n");
    scanf("%s",f.zhang);
    printf("请设置密码：\n");
    scanf("%s",f.mi);

}





//发送结构体
void sendstuct(client f,int fd){
    int r = send(fd,&f,sizeof(f),0);
    if(r==-1){printf("发送失败\n");}
    printf("发送成功\n");
    
   // printf("向服务器发送了%d字节数据!服务器数据大小为：%d\n",r,sizeof(f));
}


//存入结构体
void cunstuct(char z[50],char m[50]){
     strcpy(f.zhang,z);
     strcpy(f.mi,m);
     printf("账号：%s",f.zhang);

}


