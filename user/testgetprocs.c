#include "kernel/types.h"
#include "kernel/uproc.h"
#include "user/user.h"

int
main(void) {
  struct uproc processes[2];
  int result;

  result = getprocs(processes, 0);
  printf("max = 0: %d (expected 0)\n", result);

  result = getprocs(processes, -1);
  printf("max = -1: %d (expected -1)\n", result);

  result = getprocs(processes, 1);
  printf("max = 1: %d (expected 1)\n", result);

  result = getprocs(processes, 2);
  printf("max = 2: %d (expected 2)\n", result);

  result = getprocs((struct uproc *)-1, 1);
  printf("invalid pointer: %d (expected -1)\n", result);

  exit(0);
}