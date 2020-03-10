# 3000physicalview

Loadable kernel module to run a page table walk on a userspace virtual address.

I designed this as part of a tutorial for the students of COMP3000 Operating Systems.

Exposes an `ioctl` call for returning physical mapping given virtual mapping.

**WARNING:** This module is insecure. It should be used for teaching purposes only. Usage in a virtualized environment is recommended.

## Usage

1. `git clone https://github.com/willfindlay/3000physicalview && cd 3000physicalview`
1. `make && make insert`
1. `./3000memview2`

## Userspace API

Example:

```c
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

...

```
