#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "shm_common.h"

void register_hunter(SystemData *sys_data) {
    if (sys_data->num_hunters >= MAX_HUNTERS) {
        printf("Jumlah hunter maksimal tercapai.\n");
        return;
    }

    char username[50];
    printf("Masukkan username baru: ");
    scanf("%s", username);

    // Cek duplikat
    for (int i = 0; i < sys_data->num_hunters; i++) {
        if (strcmp(sys_data->hunters[i].username, username) == 0) {
            printf("Username sudah terdaftar!\n");
            return;
        }
    }

    Hunter *h = &sys_data->hunters[sys_data->num_hunters];
    strcpy(h->username, username);
    h->level = 1;
    h->exp = 0;
    h->atk = 10;
    h->hp = 100;
    h->def = 5;
    h->banned = 0;
    sys_data->num_hunters++;

    printf("Registrasi berhasil untuk %s\n", h->username);
}

void hunter_menu(Hunter *h) {
    printf("\nSelamat datang, Hunter %s!\n", h->username);
    printf("Level: %d | EXP: %d | ATK: %d | HP: %d | DEF: %d\n", 
           h->level, h->exp, h->atk, h->hp, h->def);
}

void login_hunter(SystemData *sys_data) {
    char username[50];
    printf("Masukkan username: ");
    scanf("%s", username);

    for (int i = 0; i < sys_data->num_hunters; i++) {
        Hunter *h = &sys_data->hunters[i];
        if (strcmp(h->username, username) == 0) {
            if (h->banned) {
                printf("Akun ini telah dibanned.\n");
                return;
            }
            hunter_menu(h);
            return;
        }
    }

    printf("Username tidak ditemukan.\n");
}

int main() {
    key_t key = get_system_key();
    int shmid = shmget(key, sizeof(SystemData), 0666);
    if (shmid == -1) {
        perror("shmget gagal. Pastikan program system sudah dijalankan.");
        exit(1);
    }

    SystemData *sys_data = (SystemData *) shmat(shmid, NULL, 0);
    if (sys_data == (void *) -1) {
        perror("shmat gagal");
        exit(1);
    }

    int choice;
    while (1) {
        printf("\n=== MENU HUNTER ===\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Keluar\n");
        printf("Pilih: ");
        scanf("%d", &choice);

        if (choice == 1) {
            register_hunter(sys_data);
        } else if (choice == 2) {
            login_hunter(sys_data);
        } else if (choice == 3) {
            break;
        } else {
            printf("Pilihan tidak valid.\n");
        }
    }

    shmdt(sys_data);
    return 0;
}

