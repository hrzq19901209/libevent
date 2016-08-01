#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
//自定义协议，@字符代表结束

int main(int argc, char **argv){
    const char query[] = "Iloveyou\ncaonima\n@";/
    const char hostname[] = "10.2.10.200";
    struct sockaddr_in sin;
    struct hostent *h;
    const char *cp;
    int fd;
    ssize_t n_written,remaining;
    char buf[1024];

    h = gethostbyname(hostname);
    if(!h){
        fprintf(stderr, "Could not lookup %s: %s", hostname, hstrerror(h_errno));    
        return 1;
    }

    if(h->h_addrtype != AF_INET){
        fprintf(stderr, "No ipv6 support, sorry.");    
        return 1;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket");    
        return 1;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(1314);
    sin.sin_addr = *(struct in_addr*)h->h_addr;

    if(connect(fd, (const struct sockaddr*)&sin, sizeof(sin)) < 0){
        perror("connect");
        close(fd);
        return 1;
    }

    cp = query;
    remaining = strlen(query);
    while(remaining){
        n_written = send(fd, cp, remaining, 0);
        if(n_written <= 0){
            perror("send");
            return 1;
        }
        remaining -= n_written;
        cp += n_written;
    }

    while(1){
        ssize_t result = recv(fd, buf, sizeof(buf), 0);
        if(result == 0){
            break;    
        }else if(result < 0){
            perror("recv:");
            close(fd);
            return 1;
        }
        fwrite(buf, 1, result, stdout);//行缓存，没有\n结尾就无法输出
    }
    close(fd);
    return 0;
}
