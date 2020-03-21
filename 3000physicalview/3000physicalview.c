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

#include "3000physicalview.h"

static struct device *device = NULL;
static struct class *class   = NULL;
static int major_number;

/* Helper functions below this line ---------------- */

/* Walk the page table of current task for virtual address addr */
static unsigned long get_physical(unsigned long addr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    unsigned long pfn = 0;
    unsigned long phys = 0;

    /* Find pgd */
    pgd = pgd_offset(current->mm, addr);
    if (!pgd || pgd_none(*pgd) || pgd_bad(*pgd))
    {
        printk(KERN_ERR "Invalid pgd for address 0x%016lx\n", addr);
        return phys;
    }

    /* Find p4d */
    p4d = p4d_offset(pgd, addr);
    if (!p4d || p4d_none(*p4d) || p4d_bad(*p4d))
    {
        printk(KERN_ERR "Invalid p4d for address 0x%016lx\n", addr);
        return phys;
    }

    /* Find pud */
    pud = pud_offset(p4d, addr);
    if (!pud || pud_none(*pud) || pud_bad(*pud))
    {
        printk(KERN_ERR "Invalid pud for address 0x%016lx\n", addr);
        return phys;
    }

    /* Find pmd */
    pmd = pmd_offset(pud, addr);
    if (!pmd || pmd_none(*pmd) || pmd_bad(*pmd))
    {
        printk(KERN_ERR "Invalid pmd for address 0x%016lx\n", addr);
        return phys;
    }

    /* Find pte */
    pte = pte_offset_map(pmd, addr);
    if (!pte || pte_none(*pte))
    {
        printk(KERN_ERR "Invalid pte for address 0x%016lx\n", addr);
        return phys;
    }

    /* Get physical address of page table entry */
    pfn = pte_pfn(*pte);
    phys = (pfn << PAGE_SHIFT) + (addr % PAGE_SIZE);

    return phys;
}

/* Define file operations below this line ------------------- */

/* Callback to device open */
static int physicalview_open(struct inode * inode, struct file * file)
{
    printk(KERN_INFO "3000physicalview open\n");
    return 0;
}

/* Callback to device close */
static int physicalview_release(struct inode * inode, struct file * file)
{
    printk(KERN_INFO "3000physicalview closed\n");
    return 0;
}

/* Callback to device ioctl */
static long physicalview_ioctl(struct file *file, unsigned int cmd, unsigned long addr)
{
    struct physicalview_memory *mem;
    switch (cmd)
    {
        case PHYSICALVIEW_WALK:
            /* Allocate kernel memory for our struct */
            mem = kmalloc(sizeof(struct physicalview_memory), GFP_KERNEL);
            if (!mem)
            {
                printk(KERN_ERR "Unable to allocate space for struct\n");
                return -EFAULT;
            }

            /* Get virt from userspace */
            if (copy_from_user(mem, (struct physicalview_memory *)addr,
                        sizeof(struct physicalview_memory)))
            {
                printk(KERN_ERR "Unable to copy struct from user\n");
                kfree(mem);
                return -EFAULT;
            }

            /* Call helper to get physical mapping for virtual address */
            mem->phys = get_physical(mem->virt);

            /* Give phys back to userspace */
            if (copy_to_user((struct physicalview_memory *)addr, mem,
                        sizeof(struct physicalview_memory)))
            {
                printk(KERN_ERR "Unable to copy struct to user\n");
                kfree(mem);
                return -EFAULT;
            }

            /* Cleanup, cleanup, everybody do their share */
            kfree(mem);
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

/* Register file operations */
static struct file_operations fops = {
    .open = physicalview_open,
    .release = physicalview_release,
    .unlocked_ioctl = physicalview_ioctl,
};

/* World readable and writable because... security? */
static char *physicalview_devnode(struct device *device, umode_t *mode)
{
    if (mode)
        *mode = 0666;
    return NULL;
}

/* Entry and exit points below this line ------------------------------------ */

/* Module initialization */
int init_module(void)
{
    printk(KERN_INFO "3000physicalview initializing\n");

    /* Register character device */
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        goto failed_chrdevreg;
    }

    /* Create device class */
    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class))
    {
        goto failed_classreg;
    }

    /* Set devnode to set our "super secure" permissions from above */
    class->devnode = physicalview_devnode;

    /* Create device */
    device = device_create(class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device))
    {
        goto failed_devreg;
    }

    printk(KERN_INFO "3000physicalview is loaded and regstered with major number %d!\n", major_number);

    return 0;

    /* NOTREACHED... */

    /* Errors here */
failed_devreg:
    printk(KERN_ERR "Failed to register 3000physicalview device!\n");
    class_unregister(class);
    class_destroy(class);
failed_classreg:
    printk(KERN_ERR "Failed to register 3000physicalview class!\n");
    unregister_chrdev(major_number, DEVICE_NAME);
failed_chrdevreg:
    printk(KERN_ERR "Failed to register 3000physicalview as a character device!\n");
    return -1;
}


/* Module destructor callback */
void cleanup_module(void)
{
    /* Cleanup, cleanup, everybody do their share */
    device_destroy(class, MKDEV(major_number, 0));
    class_unregister(class);
    class_destroy(class);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "3000physicalview cleanup complete\n");
}
