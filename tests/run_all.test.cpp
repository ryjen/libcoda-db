#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

class sqldb;

int main(int argc, char *argv[])
{
    char mysql[BUFSIZ + 1] = {0};
    char sqlite[BUFSIZ + 1] = {0};
    char postgres[BUFSIZ + 1] = {0};
    pid_t pid;
    struct stat st;

    snprintf(mysql, BUFSIZ, "%s-mysql", argv[0]);
    snprintf(sqlite, BUFSIZ, "%s-sqlite", argv[0]);
    snprintf(postgres, BUFSIZ, "%s-postgres", argv[0]);

    if (stat(mysql, &st) == -1) {
        perror("unable to stat file");
        puts(mysql);
        return 1;
    }

    if (!(st.st_mode & S_IXUSR)) {
        printf("%s is not found or not executable", mysql);
        return 1;
    }

    if (stat(sqlite, &st) == -1) {
        perror("unable to stat file");
        puts(sqlite);
        return 1;
    }

    if (!(st.st_mode & S_IXUSR)) {
        printf("%s is not found or not executable", sqlite);
        return 1;
    }

    if (stat(postgres, &st) == -1) {
        perror("unable to stat file");
        puts(postgres);
        return 1;
    }

    if (!(st.st_mode & S_IXUSR)) {
        printf("%s is not found or not executable", postgres);
        return 1;
    }

    pid = fork();

    if (pid == -1) {
        perror("unable to open pid");
        return 0;
    }

    if (pid == 0) {
        execl(mysql, mysql, NULL);
        return 0;
    }

    waitpid(pid, NULL, 0);


    pid = fork();

    if (pid == -1) {
        perror("unable to open pid");
        return 0;
    }

    if (pid == 0) {
        execl(sqlite, sqlite, NULL);
        return 0;
    }

    waitpid(pid, NULL, 0);

    execl(postgres, postgres, NULL);

    return 0;
}