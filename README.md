# 3000physicalview

Loadable kernel module to run a page table walk on a userland virtual address.

Exposes an `ioctl` call for returning physical mapping given virtual mapping.

## Usage

1. `git clone https://github.com/willfindlay/3000physicalview && cd 3000physicalview`
1. `make && make insert`
1. `./3000memview2`
