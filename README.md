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
2. Contoh yang valid seperti berikut : `a.txt, 1.txt, z.txt, 5.txt`
Contoh yang tidak valid seperti berikut :  `ab.txt, 01.txt, a.b.txt, 1a.txt, c.jpeg, x.doc`

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
