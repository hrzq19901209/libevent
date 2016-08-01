#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#define MAXLINE 1024

char rot13_char(char c){
    if((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')){
        return c+13;    
    }else if((c >= 'n' && c<= 'z') || (c >= 'N' && c <= 'Z')){
        return c-13;    
    }else{
        return c;    
    }
}

void child(void* arg){
    int fd = *(int*)arg;
    char buf[MAXLINE+1];
    size_t buf_used=0;
    ssize_t result;
    char c;
    while(1){
        result = recv(fd, &c, 1, 0);
        if(result == -1){
            perror("recv");
            break;
        }else if(result == 0){
            break;
        }
        if(c == '@'){
            break;//@代表结束
        }
        if(buf_used < sizeof(buf)){//如果一行超过1024，则不处理后续的
            buf[buf_used++] = rot13_char(c);
        }

        if(c == '\n'){
            send(fd, buf, buf_used, 0);
            buf_used = 0;
            continue;
        }
    }
    close(fd);
}

void run(){
    int serverfd;
    struct sockaddr_in sin;
    uint16_t port = 1314;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == 1){
        perror("socket");
        return;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = 0;

    socklen_t sinlen = sizeof(sin);

    if(bind(serverfd, (const struct sockaddr*)&sin, sinlen) == -1){
        perror("bind");
        return;
    }

    if(getsockname(serverfd, (struct sockaddr*)&sin, &sinlen) == -1){
        perror("getsockname");
        return;
    }
    port = ntohs(sin.sin_port);

    if(listen(serverfd, 16) == -1){
        perror("listen");
        return;
    }

    //printf("listening on port %d", port);
    printf("listening on port %" PRIu16 "\n", port);

    while(1){
        struct sockaddr_in client_sin;
        socklen_t csinlen = sizeof(client_sin);
        int fd = accept(serverfd, (struct sockaddr*)&client_sin, &csinlen);
        pthread_t pt;
        if(fd == -1){
            perror("accept");
        }else{
            if(pthread_create(&pt, NULL, (void *)child, (void *)&fd) != 0){
                perror("pthread_create");
            }
        }
    }
    close(serverfd);
}

int main(int argc, char** argv){
    run();
    return 0;
}
