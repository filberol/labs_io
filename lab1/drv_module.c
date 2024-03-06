#ifndef CHARACTER_MODULE
#define CHARACTER_MODULE

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/slab.h>

#define CLASS_NAME "drv_modules"
#define DEVICE_NAME "var2"

# define LLMAX (long long) 0x7FFFFFFFFFFFFFFF


struct list_res {
    struct list_head list;
    long result;
};

static struct list_head head_res;

static int list_length(struct list_head *head_ptr) {
    int len = 0;
    struct list_head *ptr;
    list_for_each(ptr, head_ptr) {
        len++;
    }
    return len;
}

static struct proc_dir_entry* entry;

static int majorNumber = 0;
static int minorNumber = 0;
static struct device* chr_device;
static struct class* class;

static int device_open_count = 0;

static int device_open(struct inode*, struct file*);
static int device_close(struct inode*, struct file*);
static ssize_t device_write(struct file*, const char __user*, size_t, loff_t*);
static ssize_t device_read(struct file*, char __user*, size_t, loff_t*);

static long process(const char __user* buffer, size_t len, loff_t* offset) {
    long long result;
    char wasAny = 0;
    char isFirst = 1;
    long long operand_1 = 0;
    long long operand_2 = 0;
    char operator = ' ';

    size_t i;
    for (i = *offset; i < *offset + len; i++) {
        char getChar;
        get_user(getChar, buffer + i);

        if (getChar >= '0' && getChar <= '9') {
            wasAny = 1;
            if (isFirst) {
                operand_1 *= 10;
                operand_1 += getChar - '0';
            } else {
                operand_2 *= 10;
                operand_2 += getChar - '0';
            }
        } else {
            switch (getChar) {
                case '+':
                case '-':
                case '*':
                case '/':
                if (isFirst) {
                    isFirst = 0;
                    operator = getChar;
                } else {
                    len = i;
                break;
                }
            }
        }
    }

    switch (operator) {
        case '+':
            result = operand_1 + operand_2;
            break;
        case '-':
            result = operand_1 - operand_2;
            break;
        case '*':
            result = operand_1 * operand_2;
            break;
        case '/':
            if (operand_2 == 0) {
                result = LLMAX;
            } else {
                result = operand_1 / operand_2;
            }
            break;
        default:
            result = LLMAX;
        }
    return result;
}

static const struct file_operations dev_ops = {
        .owner   = THIS_MODULE,
        .open    = device_open,
        .release = device_close,
        .read    = device_read,
        .write   = device_write
};

static int device_open(struct inode* inode, struct file* file) {
    if (device_open_count)
        return -EBUSY;
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_close(struct inode* i, struct file* f) {
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_write(struct file* filep, const char __user* buffer, size_t len, loff_t* offset) {
    struct list_res *r = kmalloc(sizeof(struct list_res), GFP_KERNEL);
    r->result = process(buffer, len, offset);
    list_add(&r->list, &head_res);
    return *offset + len;
}

#define LONG_STR_LEN 25

static ssize_t device_read(struct file* file, char __user *ubuf, size_t count, loff_t* ppos) {
    char *buf = kzalloc(sizeof(char) * LONG_STR_LEN * list_length(&head_res), GFP_KERNEL);

    struct list_head *ptr;
    struct list_res *entry;
    size_t i = 0;

    size_t list_size = list_length(&head_res);
    long long all_sum = 0;
    list_for_each(ptr, &head_res) {
        entry = list_entry(ptr, struct list_res, list);
        snprintf(buf+(i*LONG_STR_LEN), LONG_STR_LEN, "Result %ld: %ld\n", list_size - i, entry->result);
        printk(KERN_NOTICE "%s: Result %ld: %ld\n", THIS_MODULE->name, list_size - i, entry->result);
        all_sum += entry->result;
        i++;
    }

    size_t len = LONG_STR_LEN * list_size;
    if (*ppos > 0 || count < len){
        return 0;
    }
    if (copy_to_user(ubuf, buf, len)){
        return -EFAULT;
    }
    *ppos = len;
    kfree(buf);
    return len;
}

static int __init init(void) {
    printk(KERN_INFO "Loading module");

    majorNumber = register_chrdev(majorNumber, DEVICE_NAME, &dev_ops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "%s: failed to register major\n", THIS_MODULE->name);
        return majorNumber;
    } else {
        printk(KERN_INFO "%s: Registered with major %d\n", THIS_MODULE->name, majorNumber);
    }

    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "%s: Failed to register device class: %s\n", THIS_MODULE->name, CLASS_NAME);
        return PTR_ERR(class);
    } else {
        printk(KERN_INFO "%s: Registered device class: %s\n", THIS_MODULE->name, CLASS_NAME);
    }

    chr_device = device_create(class, NULL, MKDEV(majorNumber, minorNumber), NULL, DEVICE_NAME);
    if (IS_ERR(chr_device)) {
        class_destroy(class);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "%s: Failed to create device: %s\n", THIS_MODULE->name, DEVICE_NAME);
        return PTR_ERR(chr_device);
    } else {
        printk(KERN_INFO "%s: Registered device: /dev/%s\n", THIS_MODULE->name, DEVICE_NAME);
    }

    INIT_LIST_HEAD(&head_res);
    return 0;
}

static void __exit exit(void) {
    printk(KERN_INFO "Unloading module");

    device_destroy(class, MKDEV(majorNumber, 0));
    class_unregister(class);
    class_destroy(class);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "%s: Device /dev/%s destroyed\n", THIS_MODULE->name, DEVICE_NAME);

    struct list_res *entry, *next;

    list_for_each_entry_safe(entry, next, &head_res, list) {
        list_del(&entry->list);
        kfree(entry);
    }
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vadim Mikhu, Anna Chernova");
MODULE_DESCRIPTION("Simple Linux module for symbolic device with trivial arithmetic logic");

module_init(init);
module_exit(exit);
#endif
