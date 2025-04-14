#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

void download_and_extract();
void filter_files();
void combine_files();
void decode_combined();
int is_valid_filename(const char *filename);
char rot13(char c);

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

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

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
