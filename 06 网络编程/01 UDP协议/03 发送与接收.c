#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


void * RecvMsg (void * arg)
{
    int sockfd = *(int *)arg ;
    // 等待消息到达
    char * msg = calloc(1 ,128) ;
    struct sockaddr_in formAddr ;
    int addrlen = sizeof(formAddr);

    while (1)
    {
        // 清空接收消息的缓冲区
        memset(msg , 0 , 128 );


        printf("等待消息到达...\n");
        ssize_t ret_val = recvfrom( sockfd, msg  , 128 ,  0 ,
                            (struct sockaddr *) &formAddr, &addrlen);
        printf("成功收到【%s】【%d】消息尺寸：%ld 收到消息：%s\n"  , 
                    inet_ntoa(formAddr.sin_addr) , ntohs(formAddr.sin_port), ret_val , msg );
    }


}

int main(int argc, char const *argv[])
{    
    
    // 创建套接字
    int sockfd = socket(AF_INET , SOCK_DGRAM , 0  );
    if ( sockfd == -1 )
    {
        perror("socket error");
        return -1 ;
    }
    printf("套接字创建成功...\n");
    
    // 设置好自己的地址与端口号
    struct sockaddr_in myAddr = {
        .sin_family = AF_INET ,     // 选择使用IPV4协议
        .sin_addr = inet_addr("192.168.1.11") , // 直接默认使用当前设备的IP地址 （各个网卡都适用）
        .sin_port = htons(atoi(argv[1]))  // 设置端口号为 65000
    };

    // 绑定自己的端口以及地址信息
    socklen_t addrlen = sizeof(myAddr) ;
    if(bind(sockfd, (const struct sockaddr *)&myAddr,addrlen))
    {
        perror("bind error");
        return -1 ;
    }
    printf("绑定成功...\n");


    // 开启一个线程来等待消息的到达
    pthread_t thread ;
    pthread_create(&thread, NULL , RecvMsg , (void *)&sockfd );



    printf("请输入对方的地址以及端口号\n");
    char ip [ 16 ] = {0};
    short port = 0 ;
    scanf( "%s %hd"  , ip , &port );


    // 设置好 接收方的 IP地址信息 （IP+端口+协议）
    struct sockaddr_in toAddr = {
        .sin_family = AF_INET ,     // 选择使用IPV4协议
        .sin_addr = inet_addr(ip), // 设置目标地址为"192.168.1.1" 
        .sin_port = htons(port)  // 设置端口号为 65000
    };



    // 发送消息
    char * msg = calloc( 1, 128 ) ;

    while (1)
    {
        // 清空接收消息的缓冲区
        memset(msg , 0 , 128 );


        printf("请输入需要发送的消息..\n");
        fgets(msg , 128 , stdin );

        int ret_val = sendto(sockfd , msg , strlen(msg) , 0 ,
                        (const struct sockaddr *) &toAddr , sizeof(toAddr));
        printf("成功发送%d字节的数据....\n" , ret_val );
    }
    

    // 关闭套接字
    close(sockfd );

    return 0;
}
