#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define pipe_for_w "RESP_PIPE_61482"
#define pipe_for_r "REQ_PIPE_61482"

int rfd, wfd;

void conexiune_prin_pipe() {
    unlink(pipe_for_w);
    if (mkfifo(pipe_for_w, 0666) != 0) {
        printf("ERROR\ncannot create the response pipe\n");
        exit(1);
    }

    rfd = open(pipe_for_r, O_RDONLY);
    if (rfd == -1) {
        printf("ERROR\ncannot open the request pipe\n");
        exit(1);
    }

    wfd = open(pipe_for_w, O_WRONLY);
    if (wfd == -1) {
        printf("ERROR\ncannot open the request pipe\n");
        exit(1);
    }

    write(wfd, "CONNECT!", 8);

    printf("SUCCESS\n");
}

int main() {
    conexiune_prin_pipe();

    return 0;
}

