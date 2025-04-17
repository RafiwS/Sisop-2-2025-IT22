```
===================== KELOMPOK IT22 =======================
Ananda Widi Alrafi 5027241067
Raynard Carlent 5027241109
Zahra Hafizhah 5027241121
===========================================================
```

#1 Soal no 1

Pada soal no 1 diberikan beberapa tahapan yang harus dilakukan yaitu :
1. Download dan ekstrak Clues.zip
2. Filter file yang hanya bernama `a.txt`, `1.txt`, dll
3. Gabungkan isi file dengan urutan angka-huruf ke Combined.txt
4. Decode ROT13 dari Combined.txt ke Decoded.txt



```c
int main(int argc, char *argv[]) {
    if (argc == 1) {
        download_and_extract();
    } else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "Filter") == 0) {
            filter_files();
        } else if (strcmp(argv[2], "Combine") == 0) {
            combine_files();
        } else if (strcmp(argv[2], "Decode") == 0) {
            decode_combined();
        } else {
            fprintf(stderr, "Argumen tidak valid. Gunakan: ./action -m [Filter|Combine|Decode]\n");
        }
    } else {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "./action               (download dan extract clue)\n");
        fprintf(stderr, "./action -m Filter     (filter file)\n");
        fprintf(stderr, "./action -m Combine    (gabungkan isi file)\n");
        fprintf(stderr, "./action -m Decode     (decode Rot13)\n");
    }

    return 0;
}
```
1. Bagian ini dibuat untuk membuat output yaitu menginstall file Clues.zip jika ./action tidak memiliki argumen
2. Bagian ini juga dibuat untuk melakukan filtering seperti yang ada di dalam code yaitu
   `./action -m Filter` `./action -m Combine` `./action m Decode`
3. Dan jika argumen tidak ada, maka akan memberikan output `fprintf(stderr, "Argumen tidak valid. Gunakan: ./action -m [Filter|Combine|Decode]\n");`

```c
void download_and_extract() {
    struct stat st = {0};
    if (stat("Clues", &st) == 0) {
        printf("Folder 'Clues' sudah ada. Skip download.\n");
        return;
    }

    printf("Downloading Clues.zip...\n");
    int dl = system("wget -O Clues.zip 'https://drive.google.com/uc?export=download&id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK'");
    if (dl != 0) {
        printf("Gagal download Clues.zip\n");
        return;
    }

    printf("Unzipping Clues.zip...\n");
    int uz = system("unzip Clues.zip > /dev/null");
    if (uz != 0) {
        printf("Gagal unzip Clues.zip\n");
        return;
    }

    remove("Clues.zip");
    printf("Clues.zip berhasil di-download dan diekstrak!\n");
}

int is_valid_filename(const char *filename) {
    return strlen(filename) == 5 &&
        (isdigit(filename[0]) || isalpha(filename[0])) &&
        strcmp(filename + 1, ".txt") == 0;
}
```
1. Pada bagian ini akan memberikan beberapa output yaitu :
   - Pengecekan Folder Clues
   - Proses Download Clues.zip
   - Ekstraksi File Zip
   - Menghapus File Zip
   - Informasi Status
2. Fungsi is_valid_filename() bertugas untuk memvalidasi nama file apakah sesuai dengan kriteria yang diminta oleh soal, yaitu:
`Panjangnya harus 5 karakter (contoh a.txt, 1.txt), karakter pertama harus huruf atau angka, dan diikuti oleh file .txt`

```c
int is_valid_filename(const char *filename) {
    return strlen(filename) == 5 &&
        (isdigit(filename[0]) || isalpha(filename[0])) &&
        strcmp(filename + 1, ".txt") == 0;
}
```
1. File yang valid hanya yang namanya terdiri dari satu huruf atau satu angka diikuti dengan file .txt
- Contoh yang valid seperti berikut : `a.txt, 1.txt, z.txt, 5.txt`
- Contoh yang tidak valid seperti berikut :  `ab.txt, 01.txt, a.b.txt, 1a.txt, c.jpeg, x.doc`

```c
void filter_files() {
    DIR *d = opendir("Clues");
    if (!d) {
        perror("Folder Clues tidak ditemukan");
        return;
    }

    mkdir("Filtered", 0755);
    struct dirent *folder;
    while ((folder = readdir(d)) != NULL) {
        if (folder->d_type == DT_DIR && folder->d_name[0] != '.') {
            char subfolder_path[256];
            snprintf(subfolder_path, sizeof(subfolder_path), "Clues/%s", folder->d_name);
            DIR *sub = opendir(subfolder_path);
            if (!sub) continue;

            struct dirent *file;
            while ((file = readdir(sub)) != NULL) {
                if (file->d_type == DT_REG) {
                    char filepath[300];
                    snprintf(filepath, sizeof(filepath), "%s/%s", subfolder_path, file->d_name);

                    if (is_valid_filename(file->d_name)) {
                        char dest[300];
                        snprintf(dest, sizeof(dest), "Filtered/%s", file->d_name);
                        rename(filepath, dest);
                    } else {
                        remove(filepath);
                    }
                }
            }
            closedir(sub);
        }
    }
    closedir(d);
    printf("File berhasil difilter ke folder Filtered.\n");
}
```
#4 Soal no 4

Pada nomor 4, tugas utamanya adalah :
1. Menulis log berisi nama proses dan status (contoh: RUNNING atau FAILED) ke file debugmon.log.
2. Menampilkan daftar proses milik user tertentu.
3. Membunuh semua proses user yang tidak ada dalam daftar pengecualian, lalu mencatat proses yang dibunuh ke log.
4. Menulis ke log bahwa user "diizinkan" kembali menjalankan proses. Tidak menghidupkan proses secara nyata.
5. Menjalankan daemon yang terus menerus memantau proses user, lalu mencatat ke log bahwa proses-proses itu sedang berjalan.
6. Menghentikan daemon yang sedang berjalan berdasarkan PID yang disimpan.

1. define
 ```c
#define LOG_FILE "debugmon.log"
#define PID_FILE "debugmon.pid"
```
a. "debugmon.log" = Ini mendefinisikan nama file log yang digunakan untuk mencatat aktivitas proses yang dimonitor. Jadi, setiap kali debugmon mencatat informasi (misalnya proses yang sedang berjalan, proses yang dihentikan, dll), ia akan menulis ke dalam file ini. Fungsinya sebagai dokumentasi aktivitas, agar bisa diketahui proses apa saja yang berjalan atau digagalkan oleh debugmon.
b. "debugmon.pid" = Ini mendefinisikan nama file untuk menyimpan PID (Process ID) dari program debugmon saat berjalan sebagai daemon. Fungsinya agar program tahu PID dari dirinya sendiri saat berjalan di background, sehingga nantinya bisa dihentikan secara manual menggunakan perintah kill atau debugmon stop dan dicek apakah sudah berjalan atau belum

2. write_log
```c
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
```
Fungsi write_log digunakan untuk mencatat aktivitas proses ke dalam sebuah file log bernama debugmon.log. Fungsi ini menerima dua parameter: nama proses dan status proses tersebut (misalnya "RUNNING" atau "FAILED"). Dengan memanfaatkan fungsi waktu dari library time.h, write_log mencetak timestamp saat proses tercatat, disertai nama dan status proses. Format log disesuaikan agar mudah dibaca dan bisa digunakan untuk pelacakan riwayat aktivitas proses secara kronologis. Jika file log tidak bisa dibuka, fungsi akan langsung keluar tanpa melakukan apapun.

3. list_user_processes
```c
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
```
Fungsi ini digunakan untuk menampilkan seluruh proses yang sedang dijalankan oleh seorang user tertentu. Fungsi ini menerima parameter berupa username, lalu akan mengambil UID (User ID) dari user tersebut menggunakan fungsi getpwnam. Selanjutnya, fungsi ini membuka direktori /proc yang menyimpan informasi semua proses yang sedang berjalan di sistem. Dengan membaca file /proc/<pid>/status, fungsi mengekstrak nama proses dan UID-nya, dan hanya akan menampilkan proses yang dimiliki oleh user target. Hasil akhirnya adalah daftar PID dan nama proses milik user yang dicetak ke layar.

4. fail_processes
```c
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
```
Fungsi fail_processes memiliki tugas untuk menghentikan atau "membunuh" proses milik user tertentu, kecuali proses-proses yang termasuk dalam daftar pengecualian (whitelist) seperti bash, systemd, Xorg, debugmon, dll. Fungsi ini bekerja dengan membaca UID dari user target, lalu memindai direktori /proc untuk mendapatkan semua proses yang berjalan. Jika proses tersebut dimiliki oleh user target dan tidak masuk daftar pengecualian, maka proses akan dihentikan dengan sinyal SIGKILL. Setiap proses yang berhasil dihentikan akan dicatat di log menggunakan fungsi write_log dengan status FAILED, serta ditampilkan ke layar sebagai notifikasi.

5. revert_processes
```c
void revert_processes(const char *username) {
    write_log("debugmon", "RUNNING");
    printf("User %s diizinkan kembali menjalankan proses.\n", username);
}
```
Fungsi revert_processes tidak secara langsung menghidupkan kembali proses yang sebelumnya dihentikan, melainkan hanya menulis ke log bahwa user sudah "diizinkan" kembali menjalankan proses. Fungsinya bersifat simbolik, sebagai indikator bahwa monitoring atau pembatasan proses telah dihentikan. Ini bisa dianggap sebagai pengesahan manual bahwa user bisa menjalankan proses lagi. Log dicatat dengan nama proses debugmon dan status RUNNING, serta ditampilkan pesan konfirmasi ke layar.

6. daemon_monitor
```c
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
```
Fungsi ini merupakan inti dari daemon pemantau yang berjalan di background untuk memantau proses-proses milik user tertentu. Fungsi ini pertama-tama melakukan fork() untuk menciptakan proses anak (child process), kemudian melakukan daemonisasi dengan memutus hubungan dari terminal, membuat sesi baru, dan menutup input/output standar. Setelah itu, daemon akan terus berjalan dalam loop tak hingga, setiap 5 detik membuka direktori /proc, mencari semua proses milik user yang sesuai UID-nya, dan mencatat status RUNNING dari setiap proses ke dalam log. Dengan kata lain, fungsi ini secara pasif memonitor dan mencatat aktivitas proses milik user target secara berkala.

7. stop_daemon
```c
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
```
Fungsi stop_daemon digunakan untuk menghentikan daemon yang sebelumnya telah dijalankan oleh daemon_monitor. Fungsi ini membaca PID (Process ID) dari file debugmon.pid yang dibuat saat daemon dimulai. Jika file ditemukan dan PID valid, maka daemon akan dihentikan dengan sinyal SIGTERM. Setelah daemon dihentikan, file PID akan dihapus sebagai tanda bahwa daemon sudah tidak aktif. Jika file PID tidak ditemukan atau proses tidak bisa dihentikan, maka akan muncul pesan kesalahan.

8. main
```c
int main(int argc, char *argv[]) {
    if (argc != 3 && !(argc == 2 && strcmp(argv[1], "stop") == 0)) {
        ...
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
```
Fungsi main dalam program debugmon berperan sebagai titik awal eksekusi dan pengatur alur berdasarkan perintah yang diberikan oleh pengguna melalui command line. Pertama, program memeriksa apakah argumen yang diberikan sesuai dengan format yang diharapkan. Jika tidak sesuai, maka akan ditampilkan petunjuk penggunaan program beserta daftar perintah yang valid, yaitu list, fail, revert, daemon, dan stop. Jika perintah yang dimasukkan adalah stop, maka cukup dua argumen yang diperlukan; namun untuk perintah lainnya, dibutuhkan tiga argumen: nama program, perintah, dan nama user. Setelah memverifikasi argumen, program memanggil fungsi yang sesuai dengan perintah yang diberikan. Misalnya, list akan memanggil fungsi list_user_processes, fail memanggil fail_processes, dan seterusnya. Fungsi-fungsi ini menjalankan logika masing-masing untuk memantau atau mengendalikan proses milik user tertentu di sistem. Jika perintah tidak dikenali, maka program akan mengeluarkan pesan error bahwa perintah tersebut tidak diketahui. Secara keseluruhan, fungsi main menjadi pengarah utama bagi fitur-fitur yang dimiliki oleh debugmon.
