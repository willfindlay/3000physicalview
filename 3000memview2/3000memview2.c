#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char *gmsg = "Global Message";

const int buffer_size = 100;

void check_phys_error(unsigned long *phys)
{
    if (!phys)
        return;

    if (phys <= 0)
    {
        fprintf(stderr, "ERROR: unable to get physical address for %016lx", addr);
        phys = 0;
    }
}

int main(int argc, char *argv[], char *envp[])
{
        char *lmsg = "Local Message";
        char *buf[buffer_size];
        int i;

        unsigned long phys = 0;
        int fd = open("/dev/3000physicalview", O_RDONLY);

        ioctl(fd, 0, (unsigned long) argv);
        ioctl(fd, 0, (unsigned long) argv[0]);
        ioctl(fd, 0, (unsigned long) envp);
        ioctl(fd, 0, (unsigned long) envp[0]);

        ioctl(fd, 0, (unsigned long) lmsg);
        ioctl(fd, 0, (unsigned long) &lmsg);
        ioctl(fd, 0, (unsigned long) gmsg);
        ioctl(fd, 0, (unsigned long) &gmsg);

        ioctl(fd, 0, (unsigned long) &main);

        ioctl(fd, 0, (unsigned long) sbrk(0));
        ioctl(fd, 0, (unsigned long) &buf);

        printf("Memory report\n");
        printf("argv:      %016lx -> %016lx\n", (unsigned long) argv, phys);
        printf("argv[0]:   %016lx -> %016lx\n", (unsigned long) argv[0], phys);
        printf("envp:      %016lx -> %016lx\n", (unsigned long) envp, phys);
        printf("envp[0]:   %016lx -> %016lx\n", (unsigned long) envp[0], phys);

        printf("lmsg:      %016lx -> %016lx\n", (unsigned long) lmsg, phys);
        printf("&lmsg:     %016lx -> %016lx\n", (unsigned long) &lmsg, phys);
        printf("gmsg:      %016lx -> %016lx\n", (unsigned long) gmsg, phys);
        printf("&gmsg:     %016lx -> %016lx\n", (unsigned long) &gmsg, phys);

        printf("main:      %016lx -> %016lx\n", (unsigned long) &main, phys);

        printf("sbrk(0):   %016lx -> %016lx\n", (unsigned long) sbrk(0), phys);
        printf("&buf:      %016lx -> %016lx\n", (unsigned long) &buf, phys);

        for (i = 0; i<buffer_size; i++) {
                buf[i] = (char *) malloc(4096);
        }

        printf("buf[0]:    %lx\n", (unsigned long) buf[0]);
        printf("sbrk(0):   %lx\n", (unsigned long) sbrk(0));

        return 0;
}
