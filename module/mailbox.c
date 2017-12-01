#include "mailbox.h"
MODULE_LICENSE("Dual BSD/GPL");

static void get_process_name(char *ouput_name);
static ssize_t mailbox_read(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf);
static ssize_t mailbox_write(struct kobject *kobj,
                             struct kobj_attribute *attr, const char *buf, size_t count);
static struct kobject *hw2_kobject;
static struct kobj_attribute mailbox_attribute
    = __ATTR(mailbox, 0660, mailbox_read, mailbox_write);

static int num_entry_max = 2;

static struct mailbox_head_t mail_head;
static struct mailbox_head_t *mail_head_ptr;

module_param(num_entry_max, int, S_IRUGO);

static void get_process_name(char *ouput_name)
{
	memcpy(ouput_name, current->comm, sizeof(current->comm));
}

static ssize_t mailbox_read(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf)
{
	return ERR_EMPTY;
}

static ssize_t mailbox_write(struct kobject *kobj,
                             struct kobj_attribute *attr, const char *buf, size_t count)
{
	char process_name[100];
	struct mailbox_entry_t *mail_entry_ptr;
	mail_entry_ptr=kmalloc(sizeof(struct mailbox_entry_t), GFP_KERNEL);
	get_process_name(process_name);
	printk("%s\n",process_name);
	memcpy(&mail_entry_ptr->mail, buf, sizeof(struct mail_t));
	list_add_tail(&mail_entry_ptr->entry,&mail_head.head);
	/*
	list_for_each_entry(mail_head, &mail_head.head, head) {
        printk("Query_Word: %s; File_Path: %s\n", mail_entry_ptr->mail.data.query_word, mail_entry_ptr->mail.file_path);
    }
	*/
	printk("%s\n", mail_entry_ptr->mail.data.query_word);
	printk("%s\n", mail_entry_ptr->mail.file_path);
	return 0;
	//return ERR_FULL;

}

static int __init mailbox_init(void)
{
	printk("Insert\n");
	hw2_kobject = kobject_create_and_add("hw2", kernel_kobj);
	sysfs_create_file(hw2_kobject, &mailbox_attribute.attr);
	INIT_LIST_HEAD(&mail_head.head);
	mail_head_ptr = kmalloc(sizeof(struct mailbox_head_t),GFP_KERNEL);
	INIT_LIST_HEAD(&mail_head_ptr->head);
	return 0;
}

static void __exit mailbox_exit(void)
{
	printk("Remove\n");
	kobject_put(hw2_kobject);
}

module_init(mailbox_init);
module_exit(mailbox_exit);
