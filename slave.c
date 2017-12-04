#include "slave.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
int search_word(char *word, unsigned int *count, char *path);
int main(int argc, char **argv)
{
	struct mail_t recieve_mail;
	//struct mail_t send_mail;
	//char word[32];
	//unsigned int count=0;
	//char path[4096];
	printf("slave (PID: %d)\n", getpid());
	int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR, 0666);
	while(receive_from_fd(sysfs_fd, &recieve_mail)==-1);
	//int ret_val = read(sysfs_fd, &recieve_mail, sizeof(struct mail_t));
	//printf("%d\n", ret_val);
	//printf("%s\n",recieve_mail.data.query_word);
	//printf("%s\n",recieve_mail.file_path);
	close(sysfs_fd);
	//strcpy(recieve_mail.data.query_word,"apple");
	//strcpy(recieve_mail.file_path,"./text_set/dir1/text1-1");
	//strcpy(word,recieve_mail.data.query_word);
	//strcpy(path,recieve_mail.file_path);
	//printf("%s\n", word);
	//printf("%s\n", path);

	//search_word(word,&count,path);
	//send_mail.data.word_count = count;
	//strcpy(send_mail.file_path,path);
	//printf("%d\n", send_mail.data.word_count);
	//printf("%s\n", send_mail.file_path);
	return 0;
}

int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	/*
	 * write something or nothing
	 */

	int ret_val = write(sysfs_fd, mail, sizeof(struct mail_t));
	if (ret_val < 0) {
		//printf("The mailbox is full.\n");
		return -1;
	} else {
		return 0;
	}

	/*
	 * write something or nothing
	 */
}

int receive_from_fd(int sysfs_fd, struct mail_t *mail)
{
	/*
	 * write something or nothing
	 */

	int ret_val = read(sysfs_fd, mail, sizeof(struct mail_t));
	//printf("%d\n", ret_val);
	if (ret_val < 0) {
		//printf("The mailbox is empty.\n");
		return -1;
	} else {
		printf("(Slave Receive) Query Word: %s; ", mail->data.query_word);
		printf("File Path: %s\n",mail->file_path);
		return 0;
	}
}

int search_word(char*word,unsigned int *count, char *path)
{

	FILE *fp;
	fp = fopen(path,"r");
	char buffer[256];
	char * pch;
	if (fp == NULL) {
		printf("Failed to open: %s\n", path);
		return -1;
	} else {
		while(fgets(buffer,256,fp)!=NULL) {
			strtok(buffer, "\n");
			pch = strtok(buffer, " ");
			while (pch != NULL) {
				if(strcmp(pch,word)==0) {
					*count=*count+1;
				}
				pch = strtok (NULL, " ");
			}
		}
		return 0;
	}
}
