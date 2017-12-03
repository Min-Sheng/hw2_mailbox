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

static struct mailbox_head_t *mail_head_ptr;

module_param(num_entry_max, int, S_IRUGO);

static void get_process_name(char *ouput_name)
{
	memcpy(ouput_name, current->comm, sizeof(current->comm));
}
int list_full(const struct list_head *head)
{
	int number = 0;
	struct list_head *pos;
	for (pos = (head)->next; pos != (head); pos = pos->next) {
		number++;
	}
	//printk("%d\n",number);
	return number == num_entry_max;
}
static ssize_t mailbox_read(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf)
{
	char process_name[100];
	get_process_name(process_name);
	if (list_empty(&mail_head_ptr->head)) {
		return ERR_EMPTY;
	} else {
		if(strcmp(process_name,"slave")==0) {
			/*
			struct list_head *ptr;
			struct mailbox_entry_t *e;
			ptr = mail_head_ptr->head.next;
			e = list_entry(ptr, struct mailbox_entry_t, entry);
			*/
			struct list_head *ptr;
			list_for_each(ptr, &mail_head_ptr->head){
				struct mailbox_entry_t *e;
				e = list_entry(ptr, struct mailbox_entry_t, entry);
				printk("Q: %s; P: %s\n",e->mail.data.query_word,e->mail.file_path);
			}
			/*
			printk("Q: %s; P: %s\n",e->mail.data.query_word,e->mail.file_path);
			struct mail_t mail_temp = e->mail;
			memset(buf,0,sizeof(struct mail_t));
			memcpy(buf, &mail_temp, sizeof(struct mail_t));
			*/
			/*
			memcpy(buf, &mail_temp.data.query_word, sizeof(mail_temp.data.query_word));
			memcpy(buf + sizeof(mail_temp.data.query_word), &mail_temp.data.word_count, sizeof(mail_temp.data.word_count));
			memcpy(buf + sizeof(mail_temp.data.query_word) + sizeof(mail_temp.data.word_count), &mail_temp.file_path, sizeof(mail_temp.file_path));
			*/
			//copy_to_user(buf, &mail_temp, sizeof(struct mail_t));
			//list_del(&e->entry);
			return 0;
		} else if(strcmp(process_name,"master")==0) {
			struct list_head *ptr;
			struct mailbox_entry_t *e;
			ptr = mail_head_ptr->head.next;
			e = list_entry(ptr, struct mailbox_entry_t, entry);
			//struct mail_t mail_temp = e->mail;
			//copy_to_user(buf, &mail_temp, sizeof(struct mail_t));
			//list_del(&e->entry);
			return 0;
		} else {
			return -1;
		}
	}
}

static ssize_t mailbox_write(struct kobject *kobj,
                             struct kobj_attribute *attr, const char *buf, size_t count)
{
	char process_name[100];
	get_process_name(process_name);
	if (list_full(&mail_head_ptr->head)) {
		return ERR_FULL;
	} else {
		if(strcmp(process_name,"master")==0) {
			struct mailbox_entry_t *mail_entry_ptr;
			mail_entry_ptr=kmalloc(sizeof(struct mailbox_entry_t), GFP_KERNEL);
			memcpy(&mail_entry_ptr->mail, buf, sizeof(struct mail_t));
			list_add_tail(&mail_entry_ptr->entry,&mail_head_ptr->head);
			/*
			struct list_head *ptr;
			list_for_each(ptr, &mail_head_ptr->head){
				struct mailbox_entry_t *e;
				e = list_entry(ptr, struct mailbox_entry_t, entry);
				printk("Q: %s; P: %s\n",e->mail.data.query_word,e->mail.file_path);
			}
			*/
			return 0;
		} else if(strcmp(process_name,"slave")==0) {
			return 0;
		} else {
			return -1;
		}
	}
}

static int __init mailbox_init(void)
{
	printk("Insert\n");
	hw2_kobject = kobject_create_and_add("hw2", kernel_kobj);
	sysfs_create_file(hw2_kobject, &mailbox_attribute.attr);
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
