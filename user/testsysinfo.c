#include "kernel/types.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

int
main(void) {
  struct sysinfo info;

  if (sysinfo(&info) < 0) {
    printf("sysinfo failed\n");
    exit(1);
  }

  printf("Free memory: %ld bytes\n", info.freemem);
  printf("Processes: %ld\n", info.nproc);
  printf("Open files: %ld\n", info.nopenfile);

  if (info.freemem == 0 || info.nproc == 0) {
    printf("FAIL: invalid system information\n");
    exit(1);
  }

  if (sysinfo((struct sysinfo *)1) != -1) {
    printf("FAIL: invalid pointer was accepted\n");
    exit(1);
  }

  printf("PASS: sysinfo tests\n");
  exit(0);
}