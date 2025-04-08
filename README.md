```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define LOG_FILE "debugmon.log"
#define PID_FILE "debugmon.pid"
#define ERROR_LOG "debugmon_error.log"

void write_log(const char *process_name, const char *status) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d:%02d:%04d]-%02d:%02d:%02d_%s_%s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec,
            process_name, status);
    fclose(log);
}

void list_user_processes(const char *username) {
    DIR *proc = opendir("/proc");
    struct dirent *entry;
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        printf("User not found.\n");
        return;
    }
    uid_t target_uid = pwd->pw_uid;

    printf("PID\tCOMMAND\n");

    while ((entry = readdir(proc)) != NULL) {
        if (entry->d_type == DT_DIR) {
            long pid = strtol(entry->d_name, NULL, 10);
            if (pid <= 0) continue;

            char status_path[256];
            snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);
            FILE *status = fopen(status_path, "r");
            if (!status) continue;

            char line[256], name[256] = "";
            uid_t uid = -1;
            while (fgets(line, sizeof(line), status)) {
                if (strncmp(line, "Name:", 5) == 0)
                    sscanf(line, "Name:\t%255s", name);
                else if (strncmp(line, "Uid:", 4) == 0)
                    sscanf(line, "Uid:\t%d", &uid);
            }
            fclose(status);

            if (uid == target_uid)
                printf("%ld\t%s\n", pid, name);
        }
    }

    closedir(proc);
}

void daemon_mode(const char *username) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Running in daemon mode... PID: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) exit(EXIT_FAILURE);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        chdir(cwd);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    FILE *pidfile = fopen(PID_FILE, "w");
    if (!pidfile) {
        int errfd = open(ERROR_LOG, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (errfd >= 0) {
            dprintf(errfd, "Gagal membuka debugmon.pid\n");
            close(errfd);
        }
        exit(EXIT_FAILURE);
    }

    fprintf(pidfile, "%d", getpid());
    fclose(pidfile);

    while (1) {
        write_log("debugmon_daemon", "RUNNING");
        sleep(5);
    }
}

void stop_daemon(const char *username) {
    FILE *pidfile = fopen(PID_FILE, "r");
    if (!pidfile) {
        printf("Daemon not running.\n");
        return;
    }

    int pid;
    fscanf(pidfile, "%d", &pid);
    fclose(pidfile);

    if (kill(pid, SIGTERM) == 0) {
        printf("Daemon stopped.\n");
        remove(PID_FILE);
    } else {
        perror("Failed to stop daemon");
    }
}

void fail_processes(const char *username) {
    DIR *proc = opendir("/proc");
    struct dirent *entry;
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        printf("User not found.\n");
        return;
    }
    uid_t target_uid = pwd->pw_uid;

    while ((entry = readdir(proc)) != NULL) {
        if (entry->d_type == DT_DIR) {
            long pid = strtol(entry->d_name, NULL, 10);
            if (pid <= 0) continue;

            char status_path[256];
            snprintf(status_path, sizeof(status_path), "/proc/%ld/status", pid);
            FILE *status = fopen(status_path, "r");
            if (!status) continue;

            char line[256], name[256] = "";
            uid_t uid = -1;
            while (fgets(line, sizeof(line), status)) {
                if (strncmp(line, "Name:", 5) == 0)
                    sscanf(line, "Name:\t%255s", name);
                else if (strncmp(line, "Uid:", 4) == 0)
                    sscanf(line, "Uid:\t%d", &uid);
            }
            fclose(status);

            if (uid == target_uid) {
                kill(pid, SIGKILL);
                write_log(name, "FAILED");
            }
        }
    }

    closedir(proc);
}

void revert_processes(const char *username) {
    write_log("debugmon", "RUNNING");
    printf("Proses user %s diizinkan kembali.\n", username);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <command> <username>\n", argv[0]);
        printf("Commands: list | daemon | stop | fail | revert\n");
        return 1;
    }

    char *cmd = argv[1];
    char *username = argv[2];

    if (strcmp(cmd, "list") == 0) {
        list_user_processes(username);
    } else if (strcmp(cmd, "daemon") == 0) {
        daemon_mode(username);
    } else if (strcmp(cmd, "stop") == 0) {
        stop_daemon(username);
    } else if (strcmp(cmd, "fail") == 0) {
        fail_processes(username);
    } else if (strcmp(cmd, "revert") == 0) {
        revert_processes(username);
    } else {
        printf("Unknown command.\n");
    }

    return 0;
}
```
