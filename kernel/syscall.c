/*
 * contains the implementation of all syscalls.
 */

#include "syscall.h"

#include <errno.h>
#include <stdint.h>

#include "elf.h"
#include "process.h"
#include "spike_interface/spike_utils.h"
#include "string.h"
#include "util/functions.h"
#include "util/types.h"

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char *buf, size_t n) {
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process).
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}

ssize_t sys_user_backtrace(long depth) {
  riscv_regs regs = current->trapframe->regs;
  elf_ctx *ctx = get_elf();
  uint64 fp = regs.s0;

  fp = *(uint64 *)(fp - 8);
  while (depth--) {
    const char *name = get_symbol_name(ctx, *(uint64 *)(fp - 8));
    fp = *(uint64 *)(fp - 16);
    sprint("%s\n", name);
    if (0 == strcmp("main", name)) {
      return 0;
    }
  }
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6,
                long a7) {
  switch (a0) {
    case SYS_user_print:
      return sys_user_print((const char *)a1, a2);
    case SYS_user_exit:
      return sys_user_exit(a1);
    case SYS_user_backtrace:
      return sys_user_backtrace(a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}
