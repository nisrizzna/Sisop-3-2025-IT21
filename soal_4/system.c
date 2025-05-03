#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DUNGEONS 10

// Struktur Dungeon
struct Dungeon {
    char name[50];
    int min_level;
    int atk;
    int hp;
    int def;
    int exp;
    int key; // Key unik untuk dungeon
};

// Daftar dungeon yang telah dibuat
struct Dungeon dungeons[MAX_DUNGEONS];
int dungeon_count = 0; // Menyimpan jumlah dungeon yang telah dibuat

// Daftar nama dungeon
const char *dungeon_names[] = {
    "Demon Castle", "Pyramid Dungeon", "Red Gate Dungeon", "Hunters Guild Dungeon",
    "Busan A-Rank Dungeon", "Insects Dungeon", "Goblins Dungeon", "D-Rank Dungeon",
    "Gwanak Mountain Dungeon", "Hapjeong Subway Station Dungeon"
};

// Fungsi untuk menghasilkan dungeon
void generate_dungeon(struct Dungeon *dungeon) {
    int random_index = rand() % 10; // Random dungeon name index
    snprintf(dungeon->name, sizeof(dungeon->name), "%s", dungeon_names[random_index]);
    
    // Menentukan level minimum secara acak antara 1 dan 5
    dungeon->min_level = rand() % 5 + 1;

    // Menentukan nilai random untuk ATK, HP, DEF, dan EXP
    dungeon->atk = rand() % 51 + 100; // ATK antara 100 - 150
    dungeon->hp = rand() % 51 + 50;   // HP antara 50 - 100
    dungeon->def = rand() % 26 + 25;  // DEF antara 25 - 50
    dungeon->exp = rand() % 151 + 150; // EXP antara 150 - 300

    // Menghasilkan key unik untuk dungeon
    dungeon->key = rand(); // Key unik yang acak
}

// Fungsi untuk menampilkan informasi dungeon yang dihasilkan
void display_dungeon_info(struct Dungeon *dungeon) {
    printf("\nDungeon Generated!\n");
    printf("Name: %s\n", dungeon->name);
    printf("Minimum Level: %d\n", dungeon->min_level);
    printf("ATK: %d\n", dungeon->atk);
    printf("HP: %d\n", dungeon->hp);
    printf("DEF: %d\n", dungeon->def);
    printf("EXP: %d\n", dungeon->exp);
    printf("Key: %d\n", dungeon->key);
}

// Fungsi untuk menampilkan semua dungeon yang sudah dihasilkan
void display_all_dungeons() {
    printf("\n== DUNGEON INFO ==\n");

    if (dungeon_count == 0) {
        printf("No dungeons generated yet.\n");
        return;
    }

    for (int i = 0; i < dungeon_count; i++) {
        printf("[DUNGEON %d]\n", i + 1);
        printf("Name: %s\n", dungeons[i].name);
        printf("Minimum Level: %d\n", dungeons[i].min_level);
        printf("EXP Reward: %d\n", dungeons[i].exp);
        printf("ATK: %d\n", dungeons[i].atk);
        printf("HP: %d\n", dungeons[i].hp);
        printf("DEF: %d\n", dungeons[i].def);
        printf("Key: %d\n", dungeons[i].key);
        printf("\n");
    }
}

int main() {
    srand(time(NULL)); // Seed untuk random

    struct Dungeon new_dungeon;

    // Menu sistem
    int choice;
    do {
        printf("\n== SYSTEM MENU ==\n");
        printf("1. Hunter Info\n");
        printf("2. Dungeon Info\n");
        printf("3. Generate Dungeon\n");
        printf("4. Ban Hunter\n");
        printf("5. Reset Hunter\n");
        printf("6. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 3:
                generate_dungeon(&new_dungeon); // Menghasilkan dungeon baru
                if (dungeon_count < MAX_DUNGEONS) {
                    dungeons[dungeon_count++] = new_dungeon; // Menyimpan dungeon yang baru digenerate
                } else {
                    printf("Dungeon limit reached. Cannot generate more dungeons.\n");
                }
                display_dungeon_info(&new_dungeon); // Menampilkan informasi dungeon
                break;
            case 2:
                display_all_dungeons(); // Menampilkan semua dungeon yang telah digenerate
                break;
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while(choice != 6);

    return 0;
}

