#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define USERSPACE
#include "3000physicalview.h"

char *gmsg = "Global Message";

const int buffer_size = 100;

void report_memory(char *prefix, int fd, unsigned long virt)
{
    struct physicalview_memory mem = {};
    mem.virt = virt;

    if (ioctl(fd, PHYSICALVIEW_WALK, (unsigned long)&mem))
    {
        fprintf(stderr, "Error making ioctl call\n");
        return;
    }

    if (!mem.phys)
    {
        fprintf(stderr, "%s 0x%016lx -> UNKNOWN\n", prefix, mem.virt);
        return;
    }

    printf("%s 0x%016lx -> 0x%016lx\n", prefix, mem.virt, mem.phys);
}

int main(int argc, char *argv[], char *envp[])
{
        char *lmsg = "Local Message";
        char *buf[buffer_size];
        int i;

        int fd = open("/dev/3000physicalview", O_RDONLY);

        printf("Memory report\n");
        report_memory("argv:      ", fd, (unsigned long)argv);
        report_memory("argv[0]:   ", fd, (unsigned long)argv[0]);
        report_memory("envp:      ", fd, (unsigned long)envp);
        report_memory("envp[0]:   ", fd, (unsigned long)envp[0]);

        report_memory("lmsg:      ", fd, (unsigned long)lmsg);
        report_memory("&lmsg:     ", fd, (unsigned long)&lmsg);
        report_memory("gmsg:      ", fd, (unsigned long)gmsg);
        report_memory("&gmsg:     ", fd, (unsigned long)&gmsg);

        report_memory("main:      ", fd, (unsigned long)&main);

        report_memory("sbrk(0):   ", fd, (unsigned long)sbrk(0));
        report_memory("&buf:      ", fd, (unsigned long)&buf);

        for (i = 0; i<buffer_size; i++) {
                buf[i] = (char *) malloc(4096);
        }

        report_memory("buf[0]:    ", fd, (unsigned long)buf[0]);
        report_memory("sbrk(0):   ", fd, (unsigned long)sbrk(0));

        return 0;
}
