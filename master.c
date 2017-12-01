#include "master.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "string.h"
int main(int argc, char **argv)
{
	char ch;
	char* word='\0';
	char* directory='\0';
	int K=1;
	while ((ch = getopt(argc, argv, "q:d:s:")) != EOF) {
		switch (ch) {
		case 'q':
			printf("-q %s\r\n", optarg);
			word=optarg;
			break;
		case 'd':
			printf("-d %s\r\n", optarg);
			directory=optarg;
			break;
		case 's':
			printf("-s %s\r\n", optarg);
			K=atoi(optarg);
			break;
		default:
			fprintf(stderr, "Unknown option: '%s'\n", optarg);
			return 1;
		}

	}
	struct mail_t mail;
	strcpy(mail.data.query_word,word);
	strcpy(mail.file_path,directory);
	//printf("%s\n", mail.data.query_word);
	//printf("%s\n", mail.file_path);
	int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR, 0666);
	//printf("%d\n", sysfs_fd);
	//int ret_val = write(sysfs_fd, &mail, sizeof(struct mail_t));
	//printf("%d\n",ret_val);
	send_to_fd(sysfs_fd, &mail);
	//read_from_fd(sysfs_fd, &mail);
	close(sysfs_fd);
	return 0;
}
int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	/*
	 * write something or nothing
	 */

	int ret_val = write(sysfs_fd, mail, sizeof(struct mail_t));
	printf("%d\n",ret_val);
	if (ret_val < 0) {
		printf("The mailbox is full.\n");
	} else {
		/*
		 * write something or nothing
		 */
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
	if (ret_val < 0) {
		/*
		 * write something or nothing
		 */
	} else {
		/*
		 * write something or nothing
		 */
	}

	/*
	 * write something or nothing
	 */
}
