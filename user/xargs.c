#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"


int run(char *args[]) {
    int pid;
    int status;

    pid = fork();

    if (pid < 0) {
        fprintf(2, "xargs: fork failed\n");
        return 1;
    }

    if (pid == 0) {
        exec(args[0], args);
        fprintf(2, "xargs: exec failed\n");
        exit(1);
    }

    if (wait(&status) < 0) {
        fprintf(2, "xargs: wait failed\n");
        return 1;
    }

    return status == 0 ? 0 : 1;
}


int main(int argc, char *argv[]) {
    char buf[512];
    char *args[MAXARG + 1];
    char ch;
    int limit = 0;
    int start = 1;
    int base = 0;
    int count = 0;
    int length = 0;
    int inword = 0;
    int failed = 0;
    int n;
    int i;

//Check arguments
    if (argc < 2) {
        fprintf(2, "Usage: xargs [-n N] cmd [args...]\n");
        exit(1);
    }

//Check for the optional -n argument
    if (strcmp(argv[1], "-n") == 0) {
        if (argc < 4) {
            fprintf(2, "Usage: xargs [-n N] cmd [args...]\n");
            exit(1);
        }

        if (argv[2][0] == '\0') {
            fprintf(2, "Usage: xargs [-n N] cmd [args...]\n");
            exit(1);
        }

        for (i = 0; argv[2][i] != '\0'; i++) {
            if (argv[2][i] < '0' || argv[2][i] > '9') {
                fprintf(2, "Usage: xargs [-n N] cmd [args...]\n");
                exit(1);
            }
        }

        limit = atoi(argv[2]);

        if (limit < 1) {
            fprintf(2, "Usage: xargs [-n N] cmd [args...]\n");
            exit(1);
        }

        start = 3;
    }

//Copy the command and its existing arguments
    for (i = start; i < argc; i++) {
        if (base >= MAXARG) {
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
        }

        args[base++] = argv[i];
    }

    count = base;

//Read input one character at a time
    while ((n = read(0, &ch, 1)) == 1) {
        int separator = (ch == ' ' || ch == '\t' || ch == '\n');

//Finish a word when a separator is reached
        if (separator) {
            if (inword) {
                if (length >= sizeof(buf)) {
                    fprintf(2, "xargs: input too long\n");
                    exit(1);
                }

                buf[length++] = '\0';
                inword = 0;
            }

//Without -n, execute once per non-empty line
            if (limit == 0 && ch == '\n' && count > base) {
                args[count] = 0;

                if (run(args) != 0) failed = 1;

                count = base;
                length = 0;
            }

//With -n, execute after collecting N words
            if (limit > 0 && count - base >= limit) {
                args[count] = 0;

                if (run(args) != 0) failed = 1;

                count = base;
                length = 0;
            }

            continue;
        }

//Start a new word
        if (!inword) {
            if (count >= MAXARG) {
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }

            args[count++] = &buf[length];
            inword = 1;
        }

//Add character to buffer
        if (length >= sizeof(buf) - 1) {
            fprintf(2, "xargs: input too long\n");
            exit(1);
        }

        buf[length++] = ch;
    }

//Check for read errors
    if (n < 0) {
        fprintf(2, "xargs: read failed\n");
        exit(1);
    }

//Finish the last word if input ends without a separator
    if (inword) {
        buf[length++] = '\0';
    }

//Execute any remaining words
    if (count > base) {
        args[count] = 0;

        if (run(args) != 0) failed = 1;
    }

    if (failed) exit(1);

    exit(0);
}