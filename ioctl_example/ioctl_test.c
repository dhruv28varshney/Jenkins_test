#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define IOCTL_SET_MIXED _IO(258,0)
#define IOCTL_SET_UPPER _IO(258,1)

int main(int argc, char **argv){

   if (argc != 2) {
        printf("Error: need to specify mode.\n");
   }

   int fd= open("/dev/chardev1",O_RDWR);
   if (fd < 0){
     printf("Error: could not open char device file.\n");
     exit(0);
   }

   int rc;
   switch(atoi(argv[1])){
   case 0:
     rc = ioctl(fd, IOCTL_SET_MIXED, 0);
     break;
   case 1:
     rc = ioctl(fd, IOCTL_SET_UPPER, 0);
     break;
   }
   close(fd);

   printf("%d\n",rc); /* 0 is prined if the ioctl call is successful*/
   return 0;
}
