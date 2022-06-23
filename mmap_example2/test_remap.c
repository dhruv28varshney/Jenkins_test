#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <string.h>
#include <assert.h>
#include <errno.h>

typedef struct {
    int num;
    char buf[256];
} mmmptest_ctx;

#define DEV_NAME "/dev/mmptest3" 
#define PAGESIZE        4096

static mmmptest_ctx *ctx = NULL;
const char *modify = "## User modify! ##";

int main(int argc, char *argv[])
{
    int kfd;
    kfd = open(DEV_NAME, O_RDWR|O_NDELAY);
    if (kfd < 0) {
        printf("open file %s error\n", DEV_NAME);
        return -1;
    }
    
    ctx = (mmmptest_ctx *)mmap(0, 4*4096, PROT_READ | PROT_WRITE, MAP_SHARED, kfd, 0);
    printf("The initial data in memory: %s\n",ctx->buf);
    //printf("num: %d, buf: %s\n", ctx->num, ctx->buf);
    memcpy(ctx->buf+10, modify, strlen(modify));
    printf("The modified data in memory: %s\n", ctx->buf);
    // munmap(ctx, 4*4096);
    
    return 0;
}
