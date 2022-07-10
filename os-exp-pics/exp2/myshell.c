#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#define max_length 80
#define max_param 16
#define max_history 10
#define prompt "myshell> "
#define exit_flag "exit"
#define history_flag "history"
#define exclam_flag "!"
typedef enum _key_word {
	k_null, k_exit, k_history, k_exclam
}key_word;

typedef struct _cmd_queue {
	char* cmd[max_history];
	int rear;
}cmd_queue;

static key_word check_key_word(char*);
static int check_param(char*, char*t[max_param]);
static int handle_input(char*, cmd_queue*);
static void do_history(cmd_queue*);
static int do_repeat(cmd_queue*, char*);
static void queue_init(cmd_queue*);
static void queue_dele(cmd_queue*);
static void queue_add(cmd_queue*, char*);
static void queue_print(cmd_queue*);

int main(){
	int run = 1;
	char buf[max_length];
	memset(buf, 0, max_length);
	cmd_queue history;
	queue_init(&history);
	while(run) {
	printf("%s", prompt);
	fflush(stdout);
	char* t = fgets(buf, max_length, stdin);
	if(t == NULL) 
	{
	run = 0;
	break;
	}
	if(buf[strlen(buf)-1] == '\n')
	buf[strlen(buf)-1] = '\0';
	if(strlen(buf) <= 0)
	continue;
	run = handle_input(buf, &history);
	}
	queue_dele(&history);
	return 0;
}

static int handle_input(char* line, cmd_queue* obj) {
	int ret = 1;
	char* param[max_param];
	char line_brk[strlen(line)+1];
	strcpy(line_brk, line);
	int back = check_param(line_brk, param);
	key_word key = check_key_word(param[0]);
	switch(key) {
	case k_exit:
	return 0;
	case k_history:
	do_history(obj);
	return 1;
	case k_exclam:
	return do_repeat(obj, param[0]);
	case k_null:
	default:
	break;
	}
	pid_t pid = fork();
	if(pid < 0) {
		fprintf(stderr, "%s\n", "ERROR: fork()");
		return ret;

	}
	else if(pid == 0){
		if (back) 
		{
			close(0);
			close(1);
			close(2);
		}
		execvp(param[0], param);
		perror("error");
		exit(0);
	}
	else{
		queue_add(obj, line);
		if (!back)
		{
			wait(&ret);
		}
		ret = 1;
	}
	return ret;
}

static int check_param(char* param, char* ret[max_param]) {
	int cnt = 0;
	char *p;
	strtok(param, " ");
	ret[cnt++] = param;
	while((p = strtok(NULL, " "))) {
		ret[cnt++] = p;
	}
	if(ret[cnt-1][0] == '&') {
		ret[cnt-1] = NULL;
		return 1;
	} 
	else{
		ret[cnt] = NULL;
		return 0;
	}
}

static key_word check_key_word(char *key) {
	if(!strncmp(key, exit_flag, strlen(exit_flag))) {
		return k_exit;
	}
	if(!strncmp(key, history_flag, strlen(history_flag))) {
		return k_history;
	}
	if(!strncmp(key, exclam_flag, strlen(exclam_flag))) {
		return k_exclam;
	}
	return k_null;
}

static void do_history(cmd_queue* obj) {
	printf("The latest 10 commands are:\n");
	queue_print(obj);
}

static int do_repeat(cmd_queue* obj, char* param) {
	int ret = 1;
	int pos = obj->rear;
	if(param[strlen(exclam_flag)] != '!')
	pos = atoi(param + strlen(exclam_flag));
	if(pos < 0 || pos > max_history || !(obj->cmd[pos]))
	return 1;
	ret = handle_input(obj->cmd[pos], obj);
	return ret;
}

static void queue_init(cmd_queue* obj) {
	int i=0;
	if(obj == NULL)
	return;
	for(i = 0; i < max_history; i++)
	obj->cmd[i] = NULL;
	obj->rear = -1;
}

static void queue_dele(cmd_queue* obj) {
	int i=0;
	for(i = 0; i < max_history; ++i) {
		if(obj->cmd[i])
		{
			free(obj->cmd[i]);
			obj->cmd[i] = NULL;
		}
	}
}

static void queue_add(cmd_queue* obj, char* cmd) {
	if(obj == NULL)
	return;
	int pos = (obj->rear++ + 1) % max_history;
	obj->cmd[pos] = (char*)realloc(obj->cmd[pos], (1+strlen(cmd))*sizeof(char));
	strcpy(obj->cmd[pos], cmd);
}

static void queue_print(cmd_queue* obj) {
	int i=0;
	if(obj->rear == -1)
	return;
	int start = (obj->rear + 1) % max_history;
	while(!(obj->cmd[start]))
	start = (start + 1) % max_history;
	for(i = 0; i < max_history; ++i) {
		int pos = (start + i) % max_history;
		if(obj->cmd[pos])
		printf("cmd[%d]: %s\n", i, obj->cmd[pos]);
		else
		break;
	}
}
