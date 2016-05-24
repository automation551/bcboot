#include <unistd.h>
#include <sys/mount.h>

#include "config.h"

#define CHECK(err) do {\
	retval = retval | err;\
	if (retval != 0) goto cleanup;\
	} while (0)

void mdev_exec(char **envp) {
	pid_t pid = fork();
	if (pid == 0) {
		execve(BUSYBOX_BIN, (char * const*)MDEV_ARGV, envp);
	}
	waitpid(pid, NULL, 0);
}

int mdev(char **envp) {
	int retval = 0;

	CHECK(mount("sysfs","/sys","sysfs", MS_RELATIME, ""));

	int i;
	for (i = 0; i < MDEV_TIMEOUT; i++) {
		mdev_exec(envp);
		if (access(ROOT_DEV, R_OK)) break;
		sleep(1);
	}

cleanup:
	umount("/sys");

	return retval;
}

int main(int argc, char **argv, char **envp) {
	int retval = 0;

	CHECK(mdev(envp));
	CHECK(mount(ROOT_DEV, ROOT_PATH, ROOT_FS, 0, ""));
	CHECK(chroot(ROOT_PATH));
	execve(INIT, NULL, envp);

cleanup:
	umount(ROOT_PATH);
	execve(INIT_ANDROID, argv, envp);

	return retval;
}
