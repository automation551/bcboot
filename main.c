#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <stdio.h>

#include "config.h"
#include "utils.h"

#define CHECK(err) do {\
	retval = retval | err;\
	if (retval != 0) goto cleanup;\
} while (0)

#define FAIL(err) do { int e = err; if (e) return e; } while (0);

int create_dev(unsigned int major, unsigned int minor, const char *dev) {
	return mknod(dev, S_IFBLK | 0770, makedev(major, minor));
}

int main(int argc, char **argv, char **envp) {
	int retval = 0;

	for (int i = 0, ret = 0; i < 30; i++) {
		if (create_dev(ROOT_MAJOR, ROOT_MINOR, ROOT_DEV) &&
				access(ROOT_DEV, R_OK)) {
			goto cleanup;
		} else {
			sleep(1);
		}
	}
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
	//FAIL(execve(INIT_ANDROID, argv, envp));

	return retval;
}
