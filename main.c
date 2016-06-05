#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "config.h"
#include "utils.h"

#define CHECK(err) do {\
	retval = retval | err;\
	if (retval != 0) goto cleanup;\
	} while (0)

int create_dev(unsigned int major, unsigned int minor, const char *dev) {
	return mknod(dev, S_IFBLK | 0770, makedev(major, minor));
}

int main(int argc, char **argv, char **envp) {
	int retval = 0;

	CHECK(create_dev(ROOT_MAJOR, ROOT_MINOR, ROOT_DEV));
	CHECK(mount(ROOT_DEV, ROOT_PATH, ROOT_FS, 0, ""));

	// if kernel doesn't exists
	CHECK(chroot(ROOT_PATH));
	execve(INIT, NULL, envp);
	// else
	//CHECK(kexec(ROOT_PATH));

cleanup:

	umount(ROOT_PATH);
	remove(ROOT_DEV);
	execve(INIT_ANDROID, argv, envp);

	return retval;
}
