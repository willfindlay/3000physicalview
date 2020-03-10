/* 3000physicalview.c  A kernel module to expose virtual->physical memory mappings in userspace as an ioctl
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

/* Warning: This module is extremely insecure.
 * It is designed purely for teaching purposes.
 * Using it is stupid unless you are in COMP3000. */

#ifndef PHYSICALVIEW_H
#define PHYSICALVIEW_H

#ifndef USERSPACE /* Kernelspace only */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/pgtable_types.h>

#define DEVICE_NAME "3000physicalview"
#define CLASS_NAME "COMP3000"

MODULE_DESCRIPTION("Walk page table for userspace virtual address.");
MODULE_AUTHOR("William Findlay");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

#else /* Userspace only */
#include <sys/ioctl.h>
#endif

/* Both userspace and kernelspace */

struct physicalview_memory
{
    unsigned long virt;
    unsigned long phys;
};

/* Ioctl stuff */
#define PHYSICALVIEW_BASE 'k'
#define PHYSICALVIEW_WALK _IOWR(PHYSICALVIEW_BASE, 1, struct physicalview_memory)

#endif /* PHYSICALVIEW_H */
