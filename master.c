#include "master.h"
#include "stdlib.h"
#include "unistd.h"
int main(int argc, char **argv)
{
	char ch;
	char* word;
	char* directory;
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
	printf("%s\n",word);
	printf("%s\n",directory);
	printf("%d\n",K);
	struct mail_t *mail;
	mail.query_word=word;
	mail.file_path=directory;
	int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_WRONLY|O_CREAT, 0700);
	send_to_fd(sysfs_fd, &mail);
	read_from_fd(sysfs_fd, &mail);
	close(f);
	return 0;
}
int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	/*
	 * write something or nothing
	 */

	int ret_val = write(sysfs_fd, &mail, sizeof(struct mail_t));
	if (ret_val == ERR_FULL) {
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
int receive_from_fd(int sysfs_fd, struct mail_t *mail)
{
	/*
	 * write something or nothing
	 */

	int ret_val = read(sysfs_fd, &mail, sizeof(struct mail_t));
	if (ret_val == ERR_EMPTY) {
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
