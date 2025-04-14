#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define LOG_FILE "debugmon.log"
#define PID_FILE "debugmon.pid"

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
        printf("User '%s' tidak ditemukan.\n", username);
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

void fail_processes(const char *username) {
    DIR *proc = opendir("/proc");
    struct dirent *entry;
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        printf("User '%s' tidak ditemukan.\n", username);
        return;
    }
    uid_t target_uid = pwd->pw_uid;
    const char *excluded[] = {
        "Xorg", "X", "Xwayland", "gnome-shell", "plasmashell",
        "mutter", "kwin_x11", "kwin_wayland", "ksmserver", "kdeinit",
        "systemd", "sddm", "gdm", "lightdm", "gnome-session",
        "xdg-desktop-portal", "polkitd", "dbus-daemon",
        "bash", "zsh", "fish", "login", "agetty", "sshd",
        "konsole", "gnome-terminal", "xterm", "alacritty", "tilix",
        "debugmon", NULL
    };
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
                int skip = 0;
                for (int i = 0; excluded[i] != NULL; i++) {
                    if (strcmp(name, excluded[i]) == 0) {
                        skip = 1;
                        break;
                    }
                }
                if (!skip) {
                    kill(pid, SIGKILL);
                    write_log(name, "FAILED");
                    printf("Killed PID %ld (%s)\n", pid, name);
                }
            }
        }
    }
    closedir(proc);
}

void revert_processes(const char *username) {
    write_log("debugmon", "RUNNING");
    printf("User %s diizinkan kembali menjalankan proses.\n", username);
}

void daemon_monitor(const char *username) {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) {
        FILE *f = fopen(PID_FILE, "w");
        if (f) {
            fprintf(f, "%d\n", pid);
            fclose(f);
        }
        exit(EXIT_SUCCESS);
    }
    setsid();
    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    struct passwd *pwd = getpwnam(username);
    if (!pwd) exit(EXIT_FAILURE);
    uid_t target_uid = pwd->pw_uid;
    while (1) {
        DIR *proc = opendir("/proc");
        struct dirent *entry;
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
                    write_log(name, "RUNNING");
                }
            }
        }
        closedir(proc);
        sleep(5);
    }
}

void stop_daemon() {
    FILE *f = fopen(PID_FILE, "r");
    if (!f) {
        printf("Daemon tidak berjalan.\n");
        return;
    }
    pid_t pid;
    fscanf(f, "%d", &pid);
    fclose(f);
    if (kill(pid, SIGTERM) == 0) {
        remove(PID_FILE);
        printf("Daemon dihentikan.\n");
    } else {
        perror("Gagal menghentikan daemon");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 && !(argc == 2 && strcmp(argv[1], "stop") == 0)) {
        printf("Usage: %s <command> <username>\n", argv[0]);
        printf("Commands: list | fail | revert | daemon | stop\n");
        return 1;
    }

    const char *cmd = argv[1];
    const char *username = argc == 3 ? argv[2] : NULL;

    if (strcmp(cmd, "list") == 0) {
        list_user_processes(username);
    } else if (strcmp(cmd, "daemon") == 0) {
        daemon_monitor(username);
    } else if (strcmp(cmd, "stop") == 0) {
        stop_daemon();
    } else if (strcmp(cmd, "fail") == 0) {
        fail_processes(username);
    } else if (strcmp(cmd, "revert") == 0) {
        revert_processes(username);
    } else {
        printf("Unknown command: %s\n", cmd);
        return 1;
    }

    return 0;
}
