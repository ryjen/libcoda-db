#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

struct sqldb;

int execute_test(const char *path);

int main(int argc, char *argv[])
{
    char mysql[BUFSIZ + 1] = {0};
    char sqlite[BUFSIZ + 1] = {0};
    char postgres[BUFSIZ + 1] = {0};
    char util[BUFSIZ + 1] = {0};

    snprintf(mysql, BUFSIZ, "%s-mysql", argv[0]);
    snprintf(sqlite, BUFSIZ, "%s-sqlite", argv[0]);
    snprintf(postgres, BUFSIZ, "%s-postgres", argv[0]);
    snprintf(util, BUFSIZ, "%s-util", argv[0]);

    if (execute_test(mysql)) {
        return 1;
    }

    if (execute_test(sqlite)) {
        return 1;
    }

    if (execute_test(postgres)) {
        return 1;
    }

    if (execute_test(util)) {
        return 1;
    }

    return 0;
}

int execute_test(const char *path)
{
    pid_t pid = 0;
    int loc = 0;
#ifndef WIN32
    struct stat st;
#endif

    if (!path || !*path) {
        return 1;
    }

#ifndef WIN32
    if (stat(path, &st) == -1) {
        perror("unable to stat file");
        puts(path);
        return 1;
    }

    if (!(st.st_mode & S_IXUSR)) {
        printf("%s is not found or not executable", path);
        return 1;
    }
#endif

    pid = fork();

    if (pid == -1) {
        perror("unable to open pid");
        return 1;
    }

    if (pid == 0) {
        execl(path, path, NULL);
        return 1;
    }

    waitpid(pid, &loc, 0);

    if (WIFEXITED(loc) && WEXITSTATUS(loc) != 0) {
        return 1;
    }

    return 0;
}