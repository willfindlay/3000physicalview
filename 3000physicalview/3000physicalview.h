#ifndef PHYSICALVIEW_H
#define PHYSICALVIEW_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <asm/pgtable.h>
#include <asm/pgtable_types.h>

#define DEVICE_NAME "3000physicalview"
#define CLASS_NAME "COMP3000"

MODULE_DESCRIPTION("Walk page table for userspace virtual address.");
MODULE_AUTHOR("William Findlay");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

#endif /* PHYSICALVIEW_H */
