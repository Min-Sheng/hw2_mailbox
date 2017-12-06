#ifndef MAILBOX_H
#define MAILBOX_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/spinlock_types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#define ERR_EMPTY -1
#define ERR_FULL -2

struct mail_t {
	union {
		char query_word[32];
		unsigned int word_count;
	} data;
	char file_path[4096];
};

struct mailbox_head_t {
	/*
	 * some structure members you define
	 */
	struct list_head head;
};

struct mailbox_entry_t {
	int recipient;
	struct mail_t mail;
	struct list_head entry;
};

#endif
