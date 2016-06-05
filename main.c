#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <stdio.h>

#include <sys/types.h>
#include <fcntl.h>

#include <sys/wait.h>

#include "config.h"
#include "utils.h"

#define CHECK(err) do {\
	printf(#err " \n");\
	retval = retval | err;\
	if (retval != 0) goto cleanup;\
} while (0)

#define FAIL(err) do { int e = err;\
	printf(#err " \n");\
       	if (e) return e; } while (0);

void mdev_exec(char **envp) {
	pid_t pid = fork();
	if (pid == 0) {
		execve(MDEV_BIN, MDEV_ARGS, envp);
	}
	waitpid(pid, NULL, 0);
}

int mdev(char **envp) {
	int retval = 0;

	CHECK(mount("sysfs","/sys","sysfs", MS_RELATIME, ""));

	int i;
	for (i = 0; i < MDEV_TIMEOUT; i++) {
		mdev_exec(envp);
		if (access(ROOT_DEV, R_OK) == 0) break;
		sleep(1);
	}

cleanup:
	umount("/sys");

	return retval;
}

int open_write(const char *fn)
{ return open(fn,O_WRONLY | O_NDELAY); }

int main(int argc, char **argv, char **envp) {
	int retval = 0;

	/* console */
	int ttyfd;
	if ((ttyfd =open_write("/dev/console")) != 1) {
		dup2(ttyfd, 0); dup2(ttyfd, 1); dup2(ttyfd, 2);
		if (ttyfd > 2) close(ttyfd);
	}

	printf("Booting!\n");

	CHECK(mdev(envp));
	CHECK(mount(ROOT_DEV, ROOT_PATH, ROOT_FS, 0, ""));

	// if kernel doesn't exists
	CHECK(chdir(ROOT_PATH));
	CHECK(chroot(ROOT_PATH));

	execve(INIT, NULL, envp);
	// else
	//CHECK(kexec(ROOT_PATH));

cleanup:
	chdir("/");
	umount(ROOT_DEV);
	remove(ROOT_DEV);
	FAIL(execve(INIT_ANDROID, argv, envp));

	return retval;
}
