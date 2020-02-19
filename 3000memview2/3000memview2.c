#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char *gmsg = "Global Message";

const int buffer_size = 100;

int main(int argc, char *argv[], char *envp[])
{
        char *lmsg = "Local Message";
        char *buf[buffer_size];
        int i;

        unsigned long phys = 0;
        int fd = open("/dev/3000physicalview", O_RDONLY);
        ioctl(fd, 0, (unsigned long) argv);

        printf("Memory report\n");
        printf("argv:      %lx\n", (unsigned long) argv);
        printf("argv[0]:   %lx\n", (unsigned long) argv[0]);
        printf("envp:      %lx\n", (unsigned long) envp);
        printf("envp[0]:   %lx\n", (unsigned long) envp[0]);

        printf("lmsg:      %lx\n", (unsigned long) lmsg);
        printf("&lmsg:     %lx\n", (unsigned long) &lmsg);
        printf("gmsg:      %lx\n", (unsigned long) gmsg);
        printf("&gmsg:     %lx\n", (unsigned long) &gmsg);

        printf("main:      %lx\n", (unsigned long) &main);

        printf("sbrk(0):   %lx\n", (unsigned long) sbrk(0));
        printf("&buf:      %lx\n", (unsigned long) &buf);

        for (i = 0; i<buffer_size; i++) {
                buf[i] = (char *) malloc(4096);
        }

        printf("buf[0]:    %lx\n", (unsigned long) buf[0]);
        printf("sbrk(0):   %lx\n", (unsigned long) sbrk(0));

        return 0;
}
