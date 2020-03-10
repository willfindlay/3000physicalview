/* 3000memview2.c  Userland demonstration of 3000physicalview ioctl interface
 * Copyright (C) 2020  William Findlay
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define USERSPACE
#include "3000physicalview.h"

char *gmsg = "Global Message";

const int buffer_size = 30;

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
        printf("%s 0x%016lx -> UNKNOWN\n", prefix, mem.virt);
        return;
    }

    printf("%s 0x%016lx -> 0x%016lx\n", prefix, mem.virt, mem.phys);
}

int main(int argc, char *argv[], char *envp[])
{
        char format[16];
        char *lmsg = "Local Message";
        char *buf[buffer_size];
        int i;

        int fd = open("/dev/3000physicalview", O_RDONLY);

        printf("Memory map report (virtual -> physical)\n");
        report_memory("argv:      ", fd, (unsigned long)argv);
        report_memory("argv[0]:   ", fd, (unsigned long)argv[0]);
        report_memory("envp:      ", fd, (unsigned long)envp);
        report_memory("envp[0]:   ", fd, (unsigned long)envp[0]);

        report_memory("lmsg:      ", fd, (unsigned long)lmsg);
        report_memory("&lmsg:     ", fd, (unsigned long)&lmsg);
        report_memory("gmsg:      ", fd, (unsigned long)gmsg);
        report_memory("&gmsg:     ", fd, (unsigned long)&gmsg);

        report_memory("main:      ", fd, (unsigned long)&main);

        report_memory("&buf:      ", fd, (unsigned long)&buf);

        for (i = 0; i<buffer_size; i++) {
                buf[i] = (char *) malloc(4096);
                snprintf(format, 16, "buf[%02d]:   ", i);
                report_memory(format, fd, (unsigned long)buf[i]);
        }

        return 0;
}
