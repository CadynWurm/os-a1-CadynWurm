#include "kernel/types.h"
#include "user/user.h"

//Check whether the mask contains only digits
int valid_mask(char *s) {
    int i;

    if (s[0] == '\0') return 0;

    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return 0;
    }

    return 1;
}

int
main(int argc, char *argv[]) {
    int mask;

    if (argc < 3 || !valid_mask(argv[1])) {
        fprintf(2, "Usage: trace <mask> <cmd> [args...]\n");
        exit(1);
    }

    mask = atoi(argv[1]);

    if (trace(mask) < 0) {
        fprintf(2, "trace: system call failed\n");
        exit(1);
    }

    exec(argv[2], &argv[2]);

    fprintf(2, "trace: exec failed\n");
    exit(1);
}

