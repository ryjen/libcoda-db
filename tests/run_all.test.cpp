#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

struct sqldb;

int execute_test(const char *path, char *argv[]);

int main(int argc, char *argv[])
{
    char mysql[BUFSIZ + 1] = {0};
    char sqlite[BUFSIZ + 1] = {0};
    char postgres[BUFSIZ + 1] = {0};
    char util[BUFSIZ + 1] = {0};

    char buf[BUFSIZ + 1] = {0};
    strncpy(buf, argv[0], BUFSIZ);

    buf[strlen(buf) - 4] = 0;

    snprintf(mysql, BUFSIZ, "%s_mysql", buf);
    snprintf(sqlite, BUFSIZ, "%s_sqlite", buf);
    snprintf(postgres, BUFSIZ, "%s_postgres", buf);
    snprintf(util, BUFSIZ, "%s", buf);

    if (execute_test(mysql, argv)) {
        return 1;
    }

    if (execute_test(sqlite, argv)) {
        return 1;
    }

    if (execute_test(postgres, argv)) {
        return 1;
    }

    if (execute_test(util, argv)) {
        return 1;
    }

    return 0;
}

int execute_test(const char *path, char *argv[])
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
        execvp(path, argv);
        return 1;
    }

    waitpid(pid, &loc, 0);

    if (WIFEXITED(loc) && WEXITSTATUS(loc) != 0) {
        return 1;
    }

    return 0;
}