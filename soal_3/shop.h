// shop.h
#ifndef SHOP_H
#define SHOP_H

#include <string.h>

#define MAX_WEAPONS 8

typedef struct {
    char name[64];
    int price;
    int damage;
    char passive[64];
    int passive_chance; // in percent
} Weapon;

Weapon weapon_list[MAX_WEAPONS];
int weapon_count = 0;

void init_weapons() {
    weapon_count = 8;

    strcpy(weapon_list[0].name, "Isagi's Adaptability");
    weapon_list[0].price = 300;
    weapon_list[0].damage = 40;
    strcpy(weapon_list[0].passive, "Demon-King");
    weapon_list[0].passive_chance = 50;

    strcpy(weapon_list[1].name, "Chigiri's Speed");
    weapon_list[1].price = 100;
    weapon_list[1].damage = 20;
    strcpy(weapon_list[1].passive, "");
    weapon_list[1].passive_chance = 0;

    strcpy(weapon_list[2].name, "Reo's Chameleon");
    weapon_list[2].price = 150;
    weapon_list[2].damage = 25;
    strcpy(weapon_list[2].passive, "");
    weapon_list[2].passive_chance = 0;

    strcpy(weapon_list[3].name, "Kaiser Impact");
    weapon_list[3].price = 250;
    weapon_list[3].damage = 35;
    strcpy(weapon_list[3].passive, "Finest-Clown");
    weapon_list[3].passive_chance = 30;

    strcpy(weapon_list[4].name, "Barou's Predator Eye");
    weapon_list[4].price = 200;
    weapon_list[4].damage = 30;
    strcpy(weapon_list[4].passive, "Donkey-King");
    weapon_list[4].passive_chance = 20;

    strcpy(weapon_list[5].name, "Nagi's Trapping");
    weapon_list[5].price = 50;
    weapon_list[5].damage = 15;
    strcpy(weapon_list[5].passive, "");
    weapon_list[5].passive_chance = 0;

    strcpy(weapon_list[6].name, "Bachira's Dribbling");
    weapon_list[6].price = 200;
    weapon_list[6].damage = 25;
    strcpy(weapon_list[6].passive, "White-Haired-Elim");
    weapon_list[6].passive_chance = 40;

    strcpy(weapon_list[7].name, "Fists");
    weapon_list[7].price = 0;
    weapon_list[7].damage = 10;
    strcpy(weapon_list[7].passive, "");
    weapon_list[7].passive_chance = 0;
}

#endif
