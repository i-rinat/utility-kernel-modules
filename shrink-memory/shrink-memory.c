#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kallsyms.h>

#define BANNER KERN_INFO "shrink-memory: "

static struct proc_dir_entry *shrink_memory_proc_entry;
static unsigned long (*shrink_all_memory)(unsigned long nr_pages);

static ssize_t
shrink_memory_proc_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *offp)
{
	if (shrink_all_memory) {
		int k = 0;
		printk(BANNER "trying to reclaim pages\n");
		for (k = 12; k >= 0; k--) {
			unsigned long reclaimed;
			reclaimed = shrink_all_memory((unsigned long)-1);
			printk(BANNER "reclaimed %lu pages\n", reclaimed);
		}
	}
	return count;
}

struct file_operations proc_fops = {
	.write = shrink_memory_proc_write,
};

static int __init
shrink_memory_module_init(void)
{
	printk(BANNER "module loaded\n");
	shrink_memory_proc_entry = proc_create("shrink-memory", 0666, NULL,
	                                       &proc_fops);
	if (!shrink_memory_proc_entry)
		printk(BANNER "can't create proc entry\n");

	shrink_all_memory = (void *)kallsyms_lookup_name("shrink_all_memory");
	if (!shrink_all_memory)
		printk(BANNER "can't resolve shrink_all_memory()\n");

	return 0;
}

static void __exit
shrink_memory_module_exit(void)
{
	printk(BANNER "module unloaded\n");
	proc_remove(shrink_memory_proc_entry);
}

module_init(shrink_memory_module_init);
module_exit(shrink_memory_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rinat Ibragimov");
MODULE_DESCRIPTION("Shrink active/inactive page LRU lists.");
