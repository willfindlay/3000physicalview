# 3000physicalview

Loadable kernel module to run a page table walk on a userland virtual address.

Exposes an `ioctl` call for returning physical mapping given virtual mapping.

**WARNING:** This module is unsecure. It should be used for teaching purposes only. Usage in a virtualized environment is recommended.

## Usage

1. `git clone https://github.com/willfindlay/3000physicalview && cd 3000physicalview`
1. `make && make insert`
1. `./3000memview2`

## Userspace API

Example:

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "3000physicalview.h"

int var;

int main()
{
  struct physicalview_memory mem = {
    .virt = (unsigned long)&var;
  };
  
  int fd = open("/dev/3000physicalview", O_RDONLY);
  
  ioctl(fd, PHYSICALVIEW_WALK, (unsigned long)&mem);
  
  printf("%lx maps to %lx\n", mem.virt, mem.phys);
  
  return 0;
}
```
