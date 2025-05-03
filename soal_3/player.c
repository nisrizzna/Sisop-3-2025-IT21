// player.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER "127.0.0.1"

void send_command(int sock, const char *command) {
    write(sock, command, strlen(command));
    usleep(100000); // delay biar server sempat proses
    char buffer[2048] = {0};
    int valread = read(sock, buffer, sizeof(buffer));
    if (valread > 0) printf("%s\n", buffer);
}

void print_menu() {
    printf("\n--- MAIN MENU ---\n");
    printf("1. Show Player Stats\n");
    printf("2. Shop\n");
    printf("3. View Inventory & Equip\n");
    printf("4. Battle Mode\n");
    printf("5. Exit\n");
    printf("Choose: ");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER, &serv_addr.sin_addr) <= 0) {
        printf("Invalid address / Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    int choice;
    while (1) {
        print_menu();
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                send_command(sock, "SHOW_STATS");
                break;
            case 2:
                send_command(sock, "SHOW_SHOP");
                printf("Enter weapon ID to buy: ");
                int id;
                scanf("%d", &id);
                getchar();
                char cmd[32];
                snprintf(cmd, sizeof(cmd), "BUY_WEAPON %d", id);
                send_command(sock, cmd);
                break;
            case 3:
                send_command(sock, "VIEW_INVENTORY");
                break;
            case 4:
                send_command(sock, "START_BATTLE");
                break;
            case 5:
                send_command(sock, "EXIT");
                close(sock);
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }

    return 0;
}
