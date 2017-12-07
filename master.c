#include "master.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
int complete = 0;
void find_path(char *directory, char (*path)[4096], int i);
int main(int argc, char **argv)
{
	char* word='\0';
	char* directory='\0';
	int K = 1;
	char ch;
	int total = 0;
	while ((ch = getopt(argc, argv, "q:d:s:")) != EOF) {
		switch (ch) {
		case 'q':
			//printf("-q %s\r\n", optarg);
			word=optarg;
			break;
		case 'd':
			//printf("-d %s\r\n", optarg);
			directory=optarg;
			break;
		case 's':
			//printf("-s %s\r\n", optarg);
			K=atoi(optarg);
			break;
		default:
			fprintf(stderr, "Unknown option: '%s'\n", optarg);
			return -1;
		}
	}
	//printf("Number of slaves = %d\n", K);
	int num = 0;
	pid_t pID;
	pid_t *Child_pID;
	Child_pID = malloc(K * sizeof(pid_t));
	for (num = 0; num < K; num++) {
		pID = fork();
		if(pID==0) {
			Child_pID[num]=getpid();
			break;
		}
		else if (pID < 0)
		{
			fprintf(stderr,"Failed to fork.\n");
			return -1;
		}
	}
	if(pID==0){
		if (execlp("./slave", "slave", (char *)0) == -1) {
				fprintf(stderr,"Error: Unable to load the slave.\n");
				return -1;
			}
	}else{
		char path[1024][4096];
		//printf("master (PID: %d)\n", getpid());
		int i = 0;
		memset(path, 0, sizeof(path[0][0]) * 1024 * 4096);
		find_path(directory, path, i);
		int all = i;
		i = 0;
		while (strcmp(path[i], "") != 0) {
			struct mail_t send_mail;
			strcpy(send_mail.data.query_word, word);
			strcpy(send_mail.file_path, path[i]);
			int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR, 0666);
			while(send_to_fd(sysfs_fd, &send_mail)==-1);
			close(sysfs_fd);
			struct mail_t recieve_mail;
			sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR, 0666);
			while(receive_from_fd(sysfs_fd, &recieve_mail)==-1);
			close(sysfs_fd);
			//printf("(Master recieve) Word Count: %d; File Path: %s\n", recieve_mail.data.word_count,recieve_mail.file_path);
			total = total + recieve_mail.data.word_count;
			complete++;
			//printf("(Master Send) Query Word: %s; File Path: %s\n", word, path[i]);
			i++;
			if(complete==all)
				break;
		}
		printf("The total number of query word \" %s \" is \" %d \".\n", word, total);
		for( num = 0; num < K; num++) {
			kill(Child_pID[num], SIGTERM);
		}
		free(Child_pID);
		return 0;
	}
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
	char *input;
	input = malloc(sizeof(struct mail_t));
	int ret_val = read(sysfs_fd, input, sizeof(struct mail_t));
	if (ret_val < 0) {
		//printf("The mailbox is empty.\n");
		free(input);
		return -1;
	} else {
		//printf("input: %s\n", input);
		char *count;
		char *path;
		count=strtok(input,"\n");
		path=strtok(NULL,"\n");
		//printf("count: %s; ", count);
		//printf("path: %s\n; ", path);
		mail->data.word_count=atoi(count);
		strcpy(mail->file_path, path);
		//printf("%d\n",mail->data.word_count);
		//printf("%s\n",mail->file_path);
		free(input);
		return 0;
	}
}

void find_path(char* directory, char(*path)[4096],int i)
{
	DIR* dir;
	struct dirent *ent;
	if ((dir = opendir(directory)) != NULL) {
		while (( ent = readdir(dir)) != NULL) {
			if(ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0
			   && strcmp(ent->d_name, "..") != 0) {
				char p[4096] ;
				int last = strlen(directory);
				if(directory[last-1]=='/') {
					directory[last - 1] = '\0';
				}
				sprintf(p, "%s/%s", directory, ent->d_name);
				find_path(p,path,i);
			} else if (ent->d_type == DT_REG) {
				char p[4096] ;
				int last = strlen(directory);
				if(directory[last-1]=='/') {
					directory[last - 1] = '\0';
				}
				sprintf(p, "%s/%s", directory, ent->d_name);
				//printf("%s\n",p);
				int save = 0;
				while (save==0) {
					if(strcmp(path[i],"")==0) {
						strcpy(path[i], p);
						save = 1;
					}
					i++;
				}
			}
		}
		closedir(dir);
	}
}
