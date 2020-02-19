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

    unsigned long phys = 0;

    /* Find pgd */
    pgd = pgd_offset(current->mm, addr);
    if (!pgd || pgd_none(*pgd) || pgd_bad(*pgd))
    {
        printk(KERN_ERR "Invalid pgd for address 0x%016lx\n", addr);
        return phys;
    }

    p4d = p4d_offset(pgd, addr);
    if (!p4d || p4d_none(*p4d) || p4d_bad(*p4d))
    {
        printk(KERN_ERR "Invalid p4d for address 0x%016lx\n", addr);
        return phys;
    }

    pud = pud_offset(p4d, addr);
    if (!pud || pud_none(*pud) || pud_bad(*pud))
    {
        printk(KERN_ERR "Invalid pud for address 0x%016lx\n", addr);
        return phys;
    }

    pmd = pmd_offset(pud, addr);
    if (!pmd || pmd_none(*pmd) || pmd_bad(*pmd))
    {
        printk(KERN_ERR "Invalid pmd for address 0x%016lx\n", addr);
        return phys;
    }

    pte = pte_offset_map(pmd, addr);
    if (!pte || pte_none(*pte))
    {
        printk(KERN_ERR "Invalid pte for address 0x%016lx\n", addr);
        return phys;
    }

    phys = pte->pte & PTE_PFN_MASK;

    return phys;
}

/* Define file operations below this line ------------------- */
static int physicalview_open(struct inode * inode, struct file * file)
{
    printk(KERN_INFO "3000physicalview open\n");
    return 0;
}

static int physicalview_release(struct inode * inode, struct file * file)
{
    printk(KERN_INFO "3000physicalview closed\n");
    return 0;
}

/* http://www.cs.otago.ac.nz/cosc440/labs/lab06.pdf */
static long physicalview_ioctl(struct file *file, unsigned int cmd, unsigned long addr)
{
    struct physicalview_memory *mem;
    switch (cmd)
    {
        case PHYSICALVIEW_WALK:
            mem = kmalloc(sizeof(struct physicalview_memory), GFP_KERNEL);
            if (!mem)
            {
                printk(KERN_ERR "Unable to allocate space for struct\n");
                return -EFAULT;
            }

            if (raw_copy_from_user(mem, (struct physicalview_memory *)addr,
                        sizeof(struct physicalview_memory)))
            {
                printk(KERN_ERR "Unable to copy struct from user\n");
                kfree(mem);
                return -EFAULT;
            }

            mem->phys = get_physical(mem->virt);
            //if (!mem->phys)
            //{
            //    kfree(mem);
            //    return -EFAULT;
            //}

            printk(KERN_INFO "virt 0x%016lx maps to phys 0x%016lx\n", mem->virt, mem->phys);

            if (raw_copy_to_user((struct physicalview_memory *)addr, mem,
                        sizeof(struct physicalview_memory)))
            {
                printk(KERN_ERR "Unable to copy struct to user\n");
                kfree(mem);
                return -EFAULT;
            }

            kfree(mem);
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

static struct file_operations fops = {
    .open = physicalview_open,
    .release = physicalview_release,
    .unlocked_ioctl = physicalview_ioctl,
};

static char *physicalview_devnode(struct device *device, umode_t *mode)
{
    if (mode)
        *mode = 0666;
    return NULL;
}

/* Entry and exit points below this line -------------------- */

/* Module initialization */
int init_module(void)
{
    printk(KERN_INFO "3000physicalview initalizing\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        goto failed_chrdevreg;
    }

    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class))
    {
        goto failed_classreg;
    }

    class->devnode = physicalview_devnode;

    device = device_create(class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device))
    {
        goto failed_devreg;
    }

    printk(KERN_INFO "3000physicalview is loaded and regstered with major number %d!\n", major_number);

    return 0;

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
    device_destroy(class, MKDEV(major_number, 0));
    class_unregister(class);
    class_destroy(class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "3000physicalview cleanup complete\n");
}
