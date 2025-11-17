#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#ifdef _WIN32
  #define CLEAR_CMD "cls"
#else
  #define CLEAR_CMD "clear"
#endif

struct Buku {
    int id;
    char judul[100];
    char pengarang[50];
    int stok;
};

struct Peminjaman {
    char namaPeminjam[50];
    struct Buku buku;
    char tanggal[20];
};

struct Akun {
    char username[30];
    char password[30];
};

void sleep_ms(int ms) {
#ifdef _WIN32;
#else
    usleep(ms * 1000);
#endif
}

/* clear screen */
void clear_screen() {
    system(CLEAR_CMD);
}

/* simple loading animation */
void loading(const char *msg) {
    printf("\n%s ", msg ? msg : "");
    fflush(stdout);
    for (int i = 0; i < 6; i++) {
        printf(".");
        fflush(stdout);
        sleep_ms(150);
    }
    printf("\n");
}

int contains_case_insensitive(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    size_t hlen = strlen(haystack), nlen = strlen(needle);
    if (nlen == 0) return 1;
    for (size_t i = 0; i + nlen <= hlen; i++) {
        size_t j = 0;
        for (; j < nlen; j++) {
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) break;
        }
        if (j == nlen) return 1;
    }
    return 0;
}

void tampilkanMenuBuku(struct Buku daftar[], int n) {
    printf("\n=============================================\n");
    printf("              DAFTAR BUKU TERSEDIA           \n");
    printf("=============================================\n");
    for (int i = 0; i < n; i++) {
        printf("%d. %-30s | %-15s | Stok: %d\n",
               daftar[i].id, daftar[i].judul, daftar[i].pengarang, daftar[i].stok);
    }
    printf("=============================================\n");
}

void simpanStruk(struct Peminjaman p) {
    FILE *file = fopen("struk_peminjaman.txt", "a");
    if (!file) {
        printf("Gagal membuat file struk!\n");
        return;
    }

    fprintf(file, "===== STRUK PEMINJAMAN =====\n");
    fprintf(file, "Nama Peminjam : %s\n", p.namaPeminjam);
    fprintf(file, "Judul Buku    : %s\n", p.buku.judul);
    fprintf(file, "Pengarang     : %s\n", p.buku.pengarang);
    fprintf(file, "Tanggal Pinjam: %s\n", p.tanggal);
    fprintf(file, "=============================\n\n");

    fclose(file);
}

void bukaFileStruk() {
#ifdef _WIN32
#elif __APPLE__
#else
    if (system("xdg-open struk_peminjaman.txt") != 0) {
        FILE *f = fopen("struk_peminjaman.txt", "r");
        if (!f) { printf("Belum ada riwayat peminjaman.\n"); return; }
        char line[512];
        printf("\n--- Isi struk_peminjaman.txt ---\n");
        while (fgets(line, sizeof(line), f)) fputs(line, stdout);
        fclose(f);
        printf("--- akhir file ---\n");
    }
#endif
}

void registerAkun() {
    struct Akun akun;
    FILE *file = fopen("akun.txt", "a");
    if (!file) {
        printf("Gagal membuka file akun!\n");
        return;
    }

    printf("\n===== REGISTER AKUN BARU =====\n");
    printf("Masukkan Username : ");
    scanf("%29s", akun.username);
    printf("Masukkan Password : ");
    scanf("%29s", akun.password);

    fprintf(file, "%s %s\n", akun.username, akun.password);
    fclose(file);

    printf("\nAkun berhasil dibuat! Silakan login.\n");
}

int login() {
    struct Akun akun;
    char userInput[30], passInput[30];
    FILE *file = fopen("akun.txt", "r");

    if (!file) {
        printf("Belum ada akun! Silakan register dahulu.\n");
        return 0;
    }

    printf("\n===== LOGIN =====\n");
    printf("Username : ");
    scanf("%29s", userInput);
    printf("Password : ");
    scanf("%29s", passInput);

    while (fscanf(file, "%29s %29s", akun.username, akun.password) != EOF) {
        if (strcmp(akun.username, userInput) == 0 &&
            strcmp(akun.password, passInput) == 0) {
            fclose(file);
            loading("Memverifikasi akun");
            printf("\nLogin berhasil! Selamat datang, %s.\n", userInput);
            return 1;
        }
    }

    fclose(file);
    printf("\nLogin gagal! Username atau password salah.\n");
    return 0;
}

void cariBuku(struct Buku daftar[], int n) {
    char keyword[100];
    printf("\nMasukkan judul buku yang dicari: ");
    scanf(" %[^\n]", keyword);

    printf("\nHasil pencarian:\n");
    int ditemukan = 0;
    for (int i = 0; i < n; i++) {
        if (contains_case_insensitive(daftar[i].judul, keyword)) {
            printf("%d. %s | %s | Stok: %d\n",
                   daftar[i].id, daftar[i].judul, daftar[i].pengarang, daftar[i].stok);
            ditemukan = 1;
        }
    }

    if (!ditemukan) {
        printf("Buku tidak ditemukan!\n");
    }
}

int main() {
    clear_screen();

    struct Buku daftarBuku[5] = {
        {1, "Buku", "penemu", 5},
        {2, "Buku", "penemu", 3},
        {3, "Buku", "penemu", 4},
        {4, "Buku", "penemu", 2},
        {5, "Buku", "penemu", 6}
    };

    int pilihanMenu = 0, idDipilih = 0;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    struct Peminjaman peminjaman;
    sprintf(peminjaman.tanggal, "%02d-%02d-%04d",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    printf("====================================\n");
    printf("          PERPUSTAKAAN DIGITAL      \n");
    printf("====================================\n");
    printf("1. Register Akun Baru\n");
    printf("2. Login\n");
    printf("Pilih Menu: ");
    if (scanf("%d", &pilihanMenu) != 1) {
        printf("Input tidak valid.\n");
        return 0;
    }

    if (pilihanMenu == 1) registerAkun();
    if (!login()) return 0;

    while (1) {
        printf("\n============ MENU UTAMA ============\n");
        printf("1. Lihat Daftar Buku\n");
        printf("2. Cari Buku\n");
        printf("3. Pinjam Buku\n");
        printf("4. Lihat Riwayat Peminjaman\n");
        printf("5. Keluar\n");
        printf("====================================\n");
        printf("Pilihan: ");
        if (scanf("%d", &pilihanMenu) != 1) {
            printf("Input tidak valid. Keluar.\n");
            break;
        }

        clear_screen();

        if (pilihanMenu == 1) {
            tampilkanMenuBuku(daftarBuku, 5);
        }
        else if (pilihanMenu == 2) {
            cariBuku(daftarBuku, 5);
        }
        else if (pilihanMenu == 3) {
            tampilkanMenuBuku(daftarBuku, 5);

            printf("\nMasukkan nama peminjam: ");
            scanf(" %[^\n]", peminjaman.namaPeminjam);

            printf("Pilih ID buku: ");
            if (scanf("%d", &idDipilih) != 1) {
                printf("Input tidak valid!\n");
                continue;
            }

            if (idDipilih < 1 || idDipilih > 5) {
                printf("ID buku tidak valid!\n");
                continue;
            }

            if (daftarBuku[idDipilih - 1].stok <= 0) {
                printf("Stok buku habis!\n");
                continue;
            }

            printf("\nAnda yakin ingin meminjam buku \"%s\"? (y/n): ",
                   daftarBuku[idDipilih - 1].judul);

            char konfirm = 'n';
            scanf(" %c", &konfirm);

            if (konfirm == 'n' || konfirm == 'N') {
                printf("Peminjaman dibatalkan.\n");
                continue;
            }

            peminjaman.buku = daftarBuku[idDipilih - 1];
            daftarBuku[idDipilih - 1].stok--;

            printf("\n===== STRUK PEMINJAMAN =====\n");
            printf("Nama Peminjam : %s\n", peminjaman.namaPeminjam);
            printf("Judul Buku    : %s\n", peminjaman.buku.judul);
            printf("Pengarang     : %s\n", peminjaman.buku.pengarang);
            printf("Tanggal Pinjam: %s\n", peminjaman.tanggal);
            printf("=============================\n");

            simpanStruk(peminjaman);
            printf("\nStruk disimpan ke 'struk_peminjaman.txt'\n");
        }
        else if (pilihanMenu == 4) {
            bukaFileStruk();
        }
        else if (pilihanMenu == 5) {
            printf("\nTerima kasih telah menggunakan sistem perpustakaan!\n");
            break;
        }
        else {
            printf("Pilihan tidak dikenal.\n");
        }
    }

    return 0;
}
