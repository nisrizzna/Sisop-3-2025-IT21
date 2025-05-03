// dungeon.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "shop.h"

#define PORT 8080
#define MAX_CLIENTS 1

typedef struct {
    int gold;
    int base_damage;
    int kills;
    int weapon_id;
} Player;

Player player = {
    .gold = 500,
    .base_damage = 10,
    .kills = 0,
    .weapon_id = -1
};

void handle_client(int client_sock) {
    char buffer[1024];
    srand(time(NULL));
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        read(client_sock, buffer, sizeof(buffer));

        if (strncmp(buffer, "SHOW_STATS", 10) == 0) {
            char stats[256];
            Weapon *w = (player.weapon_id >= 0) ? &weapon_list[player.weapon_id] : NULL;
            snprintf(stats, sizeof(stats), "Gold: %d\nBase Damage: %d\nKills: %d\nWeapon: %s\nPassive: %s\n",
                     player.gold, player.base_damage, player.kills,
                     w ? w->name : "None",
                     (w && strlen(w->passive) > 0) ? w->passive : "None");
            write(client_sock, stats, strlen(stats));

        } else if (strncmp(buffer, "SHOW_SHOP", 9) == 0) {
            char shop_info[1024] = "";
            for (int i = 0; i < weapon_count; i++) {
                char line[256];
                snprintf(line, sizeof(line), "%d. %s (Price: %d, Damage: %d, Passive: %s)\n",
                         i, weapon_list[i].name, weapon_list[i].price,
                         weapon_list[i].damage,
                         strlen(weapon_list[i].passive) > 0 ? weapon_list[i].passive : "None");
                strcat(shop_info, line);
            }
            write(client_sock, shop_info, strlen(shop_info));

        } else if (strncmp(buffer, "BUY_WEAPON", 10) == 0) {
            int id = atoi(buffer + 11);
            if (id < 0 || id >= weapon_count) {
                write(client_sock, "Invalid weapon ID\n", 20);
                continue;
            }
            Weapon w = weapon_list[id];
            if (player.gold < w.price) {
                write(client_sock, "Not enough gold\n", 18);
                continue;
            }
            player.gold -= w.price;
            player.weapon_id = id;
            player.base_damage = w.damage;
            write(client_sock, "Weapon purchased!\n", 19);

        } else if (strncmp(buffer, "VIEW_INVENTORY", 14) == 0) {
            char inventory_info[256];
            if (player.weapon_id == -1) {
                strcpy(inventory_info, "You have no weapons equipped.\n");
            } else {
                Weapon w = weapon_list[player.weapon_id];
                snprintf(inventory_info, sizeof(inventory_info),
                         "Weapon: %s (EQUIPPED)\nDamage: %d\nPassive: %s\n",
                         w.name, w.damage, strlen(w.passive) > 0 ? w.passive : "None");
            }
            write(client_sock, inventory_info, strlen(inventory_info));

        } else if (strncmp(buffer, "START_BATTLE", 12) == 0) {
            int enemy_hp = rand() % 51 + 50; // 50–100
            int player_hp = 50;
            char battle_log[1024] = "Battle Start!\n";

            Weapon *w = (player.weapon_id >= 0) ? &weapon_list[player.weapon_id] : NULL;

            while (enemy_hp > 0 && player_hp > 0) {
                int damage = player.base_damage;

                // Passive effect
                if (w && w->passive_chance > 0 && (rand() % 100 < w->passive_chance)) {
                    strcat(battle_log, "Passive Activated: ");
                    strcat(battle_log, w->passive);
                    strcat(battle_log, "!\n");

                    // Contoh efek nyata: double damage
                    damage *= 2;
                }

                enemy_hp -= damage;
                char hit_log[128];
                snprintf(hit_log, sizeof(hit_log), "You hit the enemy for %d damage! Enemy HP left: %d\n",
                         damage, enemy_hp);
                strcat(battle_log, hit_log);
                sleep(1);
            }

            if (enemy_hp <= 0) {
                player.kills++;
                int reward = 100 + rand() % 101;
                player.gold += reward;
                char result[128];
                snprintf(result, sizeof(result), "You defeated the enemy! Total Kills: %d. Gold earned: %d\n",
                         player.kills, reward);
                strcat(battle_log, result);
            }

            write(client_sock, battle_log, strlen(battle_log));

        } else if (strncmp(buffer, "EXIT", 4) == 0) {
            break;
        } else {
            write(client_sock, "Unknown command\n", 17);
        }
    }
    close(client_sock);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    init_weapons();

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while ((client_sock = accept(server_fd, (struct sockaddr *)&address, &addrlen)) >= 0) {
        printf("Client connected!\n");
        handle_client(client_sock);
    }

    close(server_fd);
    return 0;
}
