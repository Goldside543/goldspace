#include "syscall_numbers.h"
#include "../fs/fs_syscalls.h"

static int (*syscall_table[])(void) = {
    [SYS_CREATE_FILE] = (int (*)(void))sys_create_file,
    [SYS_WRITE_FILE]  = (int (*)(void))sys_write_file,
    [SYS_READ_FILE]   = (int (*)(void))sys_read_file,
    [SYS_DELETE_FILE] = (int (*)(void))sys_delete_file,
};
