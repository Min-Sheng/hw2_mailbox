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

static spinlock_t list_lock;

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
			struct list_head *ptr;
			struct mailbox_entry_t *mail_entry_ptr;
			spin_lock(&list_lock);
			ptr = mail_head_ptr->head.next;
			mail_entry_ptr = list_entry(ptr, struct mailbox_entry_t, entry);
			if(mail_entry_ptr->recipient == 1) {
				//printk("Q: %s; P: %s\n",mail_entry_ptr->mail.data.query_word,mail_entry_ptr->mail.file_path);
				sprintf(buf, "%s%c%s", mail_entry_ptr->mail.data.query_word,'\n',
				        mail_entry_ptr->mail.file_path);
				list_del(ptr);
				kfree(mail_entry_ptr);
				spin_unlock(&list_lock);
				return sizeof(struct mail_t);
			} else {
				spin_unlock(&list_lock);
				return ERR_EMPTY;
			}
		} else if(strcmp(process_name,"master")==0) {
			struct list_head *ptr;
			struct mailbox_entry_t *mail_entry_ptr;
			spin_lock(&list_lock);
			ptr = mail_head_ptr->head.next;
			mail_entry_ptr = list_entry(ptr, struct mailbox_entry_t, entry);
			if (mail_entry_ptr->recipient == 0) {
				//printk("C: %d; P: %s\n",mail_entry_ptr->mail.data.word_count,mail_entry_ptr->mail.file_path);
				sprintf(buf, "%d%c%s", mail_entry_ptr->mail.data.word_count,'\n',
				        mail_entry_ptr->mail.file_path);
				list_del(ptr);
				kfree(mail_entry_ptr);
				spin_unlock(&list_lock);
				return sizeof(struct mail_t);
			} else {
				spin_unlock(&list_lock);
				return ERR_EMPTY;
			}
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
			mail_entry_ptr->recipient=1;
			//printk("R: %d; Q: %s; P: %s\n", mail_entry_ptr->recipient, mail_entry_ptr->mail.data.query_word, mail_entry_ptr->mail.file_path);
			spin_lock(&list_lock);
			list_add_tail(&mail_entry_ptr->entry, &mail_head_ptr->head);
			spin_unlock(&list_lock);
			/*
			struct list_head *ptr;
			list_for_each(ptr, &mail_head_ptr->head){
				struct mailbox_entry_t *e;
				e = list_entry(ptr, struct mailbox_entry_t, entry);
				printk("R: %d; Q: %s; P: %s\n",
				e->recipient,
				e->mail.data.query_word,e->mail.file_path);
			}
			*/
			return sizeof(struct mail_t);
		} else if(strcmp(process_name,"slave")==0) {
			struct mailbox_entry_t *mail_entry_ptr;
			mail_entry_ptr=kmalloc(sizeof(struct mailbox_entry_t), GFP_KERNEL);
			memcpy(&mail_entry_ptr->mail, buf, sizeof(struct mail_t));
			mail_entry_ptr->recipient=0;
			spin_lock(&list_lock);
			list_add_tail(&mail_entry_ptr->entry, &mail_head_ptr->head);
			spin_unlock(&list_lock);
			/*
			struct list_head *ptr;
			list_for_each(ptr, &mail_head_ptr->head){
				struct mailbox_entry_t *e;
				e = list_entry(ptr, struct mailbox_entry_t, entry);
				printk("C: %d; P: %s\n",e->mail.data.word_count,e->mail.file_path);
			}
			*/
			return sizeof(struct mail_t);
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
	spin_lock_init(&list_lock);
	return 0;
}

static void __exit mailbox_exit(void)
{
	printk("Remove\n");
	kobject_put(hw2_kobject);
}

module_init(mailbox_init);
module_exit(mailbox_exit);
