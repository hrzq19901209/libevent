#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv){
    char buf[1024];
    fd_set rsets, rsetsb;
    struct timeval tv;
    int retval;
    ssize_t readnum;
    
    FD_ZERO(&rsetsb);
    FD_SET(0, &rsetsb);

    while(1){
        rsets = rsetsb;//这样会减少FD_ZERO等系统调用
        tv.tv_sec = 10;
        tv.tv_usec = 0;//从下面的select函数可知，tv是地址传递，所以每次调用select函数之前都要重置timeout
        retval = select(1, &rsets, NULL, NULL, &tv);
        if(retval == 0){
            printf("timewout....\n");
            break;
        }else if(retval == -1){
            perror("select");
            return 1;
        }

        if(FD_ISSET(0, &rsets)){
            readnum = read(0, buf, sizeof(buf));
            buf[readnum] = '\0';
            printf("%s", buf);
        }
    }
    return 0;
}
