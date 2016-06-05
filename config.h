static const char INIT_ANDROID[] = "/init.android";
static const char INIT[] = "/usr/bin/runit-init";

static const char ROOT_FS[] = "ext2";
static const char ROOT_PATH[] = "/newroot";
static const char ROOT_DEV[] = "/dev/sda1";
static const int ROOT_MAJOR = 8;
static const int ROOT_MINOR = 1;

static const char KERNEL_IMAGE[] = "/boot/Image";
static const char *KERNEL_INITRD = NULL;
static const char KERNEL_CMDLINE[] = "root=$ROOT";

static const char MDEV_BIN[] = "/sbin/busybox";
static const char *MDEV_ARGS[] = {"/sbin/mdev", "-s", NULL};
static const unsigned int MDEV_TIMEOUT = 30;
