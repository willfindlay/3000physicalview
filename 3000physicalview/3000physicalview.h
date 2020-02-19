#ifndef PHYSICALVIEW_H
#define PHYSICALVIEW_H

#ifndef USERSPACE
/* The following is used only in kernelspace */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/pgtable_types.h>

#define DEVICE_NAME "3000physicalview"
#define CLASS_NAME "COMP3000"

MODULE_DESCRIPTION("Walk page table for userspace virtual address.");
MODULE_AUTHOR("William Findlay");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

#else
#include <sys/ioctl.h> /* Include sys/ioctl.h in userspace only */
#endif

/* The following is used in userspace and kernelspace */
struct physicalview_memory
{
    unsigned long virt;
    unsigned long phys;
};

/* Ioctl stuff */
#define PHYSICALVIEW_BASE 'k'
#define PHYSICALVIEW_WALK _IOWR(PHYSICALVIEW_BASE, 1, struct physicalview_memory)

#endif /* PHYSICALVIEW_H */
