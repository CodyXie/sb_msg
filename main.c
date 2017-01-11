#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "log.h"
#include "sb_msg_def.h"
#include "sb_msg.h"
#include "sb_msg_api.h"

static void child(int argc, char* argv[]) {
    // create null terminated argv_child array
    char* argv_child[argc + 1];
    memcpy(argv_child, argv, argc * sizeof(char *));
    argv_child[argc] = NULL;

    if (execvp(argv_child[0], argv_child)) {
        loge("executing %s failed: %s", argv_child[0],
                strerror(errno));
    }
}

static int parent(int argc, char* argv[], pid_t pid, int *chld_sts)
{
	int status = 0;
	int rc = 0;
	int ret = 0;
	bool found_child = false;

	do {
		ret = waitpid(pid, &status, WNOHANG);
		if (ret < 0) {
			rc = errno;
			loge("logwrap", "waitpid failed with %s", strerror(errno));
			goto err_waitpid;
		}
		if (ret >= 0) {
			found_child = true;
			break;
		}

		if (chld_sts != NULL) {
			*chld_sts = status;
		} else {
			if (WIFEXITED(status))
				rc = WEXITSTATUS(status);
			else
				rc = -ECHILD;
		}
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status)) {
				loge("alinkd %d terminated by exit(%d)", getpid(), WEXITSTATUS(status));
			}
		} else {
			if (WIFSIGNALED(status)) {
				loge("%d terminated by signal %d", getpid(), WTERMSIG(status));
			} else if (WIFSTOPPED(status)) {
				loge("%d stopped by signal %d", getpid(), WSTOPSIG(status));
			}
		}
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
err_waitpid:
	return rc;
}


int fork_execvp_ext(int argc, char* argv[], int *status, bool ignore_int_quit)
{
	pid_t pid;
	struct sigaction intact;
	struct sigaction quitact;
	sigset_t blockset;
	sigset_t oldset;
	int rc = 0;

	sigemptyset(&blockset);
	sigaddset(&blockset, SIGINT);
	sigaddset(&blockset, SIGQUIT);
	pthread_sigmask(SIG_BLOCK, &blockset, &oldset);

	pid = fork();
	if (pid < 0) {
		rc = -1;
		loge("fork_execvp_ext failed!");
		goto err_fork;
	} else if (pid == 0) {
		pthread_sigmask(SIG_SETMASK, &oldset, NULL);
		child(argc, argv);
	} else {
		if (ignore_int_quit) {
			struct sigaction ignact;

			memset(&ignact, 0, sizeof(ignact));
			ignact.sa_handler = SIG_IGN;
			sigaction(SIGINT, &ignact, &intact);
			sigaction(SIGQUIT, &ignact, &quitact);
		}
		rc = parent(argc, argv, pid, status);
	}

	if (ignore_int_quit) {
		sigaction(SIGINT, &intact, NULL);
		sigaction(SIGQUIT, &quitact, NULL);
	}

err_waitpid:
err_fork:
	pthread_sigmask(SIG_SETMASK, &oldset, NULL);

	return rc;
}


static int alink_handler(int argc, char** argv);
static int alink_main(int argc, char **argv);
int main(int argc, char** argv)
{
	int c;
	int flag = 0;

	while ((c = getopt(argc, argv, "ch")) != -1) {
		logd("alink getopt %c", c);
		switch (c) {
		case 'h':
			flag = 'h';
			break;
		case 'c':
			flag = 'c';
			break;
		case '?':
			loge("invalid option.");
			break;
		default:
			break;
		}
	}

	if (flag == 'c') {
		alink_main(argc, argv);
	} else if (flag == 'h') {
		alink_handler(argc, argv);
	}

	exit(EXIT_FAILURE);
}

static void handle_alink_msg(SB_MSG *msg)
{
	logd("handle_alink_msg");
	assert(msg != NULL);
	logd("handle_alink_msg msg_id %d", msg->msg_id);
	sb_free_message(&msg);
}

static int alink_handler(int argc, char** argv)
{
	SB_MSG *msg = NULL;
	int msgqid;
	int ret;

	msgqid = sb_create_queue(SB_MOD_ALINK);
	if (msgqid < 0) {
		loge("sb_create_queue failed");
		exit(EXIT_FAILURE);
	}

	msgqid = sb_create_queue(SB_MOD_CONTROL);
	if (msgqid < 0) {
		loge("sb_create_queue failed");
		exit(EXIT_FAILURE);
	}

	while (1) {
		msg = sb_get_message(sb_get_queue(SB_MOD_ALINK), MSG_GET_WAIT_FOREVER, &ret);
		if ( ret < 0 || !msg) {
			logw("sb_get_message failed");
			continue;
		}

		handle_alink_msg(msg);
		sb_send_message_ext(SB_MOD_ALINK, SB_MOD_CONTROL, MSG_ID_SB_CONTROL_STA_RSP, NULL, 0);
	}

	exit(EXIT_FAILURE);
}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
static int alink_main(int argc, char **argv)
{
	int ret = -1;
	int keyPressed = 0;
	int msgqid;
	SB_MSG *msg;

	int status = 0;
	char *alinkd_argv[3] = {
		"/tmp/msg/alinkd",
		"-h",
		NULL
	};

	init_log_wrapper("alinkd");

	msgqid = sb_create_queue(SB_MOD_CONTROL);
	if (msgqid < 0) {
		loge("sb_create_queue failed");
		exit(EXIT_FAILURE);
	}

	msgqid = sb_create_queue(SB_MOD_ALINK);
	if (msgqid < 0) {
		loge("sb_create_queue failed");
		exit(EXIT_FAILURE);
	}

	fork_execvp_ext(ARRAY_SIZE(alinkd_argv), alinkd_argv, &status, true);

	 while (1) {
		sb_send_message_ext(SB_MOD_CONTROL, SB_MOD_ALINK, MSG_ID_SB_CONTROL_STA_REQ, NULL, 0);
		msg = sb_get_message(sb_get_queue(SB_MOD_CONTROL), MSG_GET_WAIT_FOREVER, &ret);
		if ( ret < 0 || !msg) {
			logw("sb_get_message failed");
			continue;
		}

		logi ("get response %d", msg->msg_id);
		sb_free_message(&msg);
		sleep(1);
	}	
	return 0;
}

