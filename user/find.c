#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

//Check if a filename matches the pattern
int match(char *name, char *pattern) {
    if (*pattern == '\0') return *name == '\0';

    if (*pattern == '*') {
        if (match(name, pattern + 1)) return 1;
        if (*name != '\0' && match(name + 1, pattern)) return 1;
        return 0;
    }

    if (*name == '\0' || *name != *pattern) return 0;

    return match(name + 1, pattern + 1);
}

//Search directories recursively
int find(char *path, char *pattern, int type, int first) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    int status = 0;

//Open the directory
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return 1;
    }

//Get information about the directory
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return 1;
    }

//Only search inside directories
    if (st.type != T_DIR) {
        close(fd);
        return first ? 1 : 0;
    }

//Check path length
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return 1;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

//Read entries
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;

//Skip "." and ".."
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;

//Add entry name to path
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';

//Get information about entry
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            status = 1;
            continue;
        }

//Check filename and type
        if (match(p, pattern)) {
            if (type == 0 || (type == 1 && (st.type == T_FILE || st.type == T_DEVICE)) || (type == 2 && st.type == T_DIR)) {
                printf("%s\n", buf);
            }
        }

//Recursion for subdirectories
        if (st.type == T_DIR) {
            if (find(buf, pattern, type, 0) != 0) status = 1;
        }
    }

    close(fd);
    return status;
}

int main(int argc, char *argv[]) {
    int type = 0;

    if (argc != 3 && argc != 5) {
        fprintf(2, "Usage: find <dir> <pattern> [-type f|d]\n");
        exit(1);
    }

    if (argc == 5) {
        if (strcmp(argv[3], "-type") != 0) {
            fprintf(2, "Usage: find <dir> <pattern> [-type f|d]\n");
            exit(1);
        }

        if (strcmp(argv[4], "f") == 0) {
            type = 1;
        } else if (strcmp(argv[4], "d") == 0) {
            type = 2;
        } else {
            fprintf(2, "Usage: find <dir> <pattern> [-type f|d]\n");
            exit(1);
        }
    }

    if (find(argv[1], argv[2], type, 1) != 0) exit(1);

    exit(0);
}