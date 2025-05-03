#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <fcntl.h>

#define SOCKET_PATH "/tmp/image_socket"
#define BUFFER_SIZE 8192

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void write_log(const char *source, const char *action, const char *info) {
    FILE *log = fopen("server/server.log", "a");
    if (!log) return;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);

    fprintf(log, "[%s][%s]: [%s] [%s]\n", source, timestamp, action, info);
    fclose(log);
}

void decrypt_menu() {
    char input_filename[128];
    printf("Enter the encoded filename: ");
    scanf(" %127s", input_filename);
    clear_stdin();

    // path ngambil file input_x
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "client/secrets/%s", input_filename);
    FILE *fp = fopen(full_path, "rb");

    if (!fp) {
        perror("Failed to open input file");
        return;
    }

    char file_data[BUFFER_SIZE];
    size_t bytes_read = fread(file_data, 1, BUFFER_SIZE - 1, fp);
    fclose(fp);

    file_data[bytes_read] = '\0';

    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connect failed");
        close(client_sock);
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "DECRYPT|%s", file_data);
    write(client_sock, buffer, strlen(buffer));

    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read_response = read(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_read_response > 0) {
        buffer[bytes_read_response] = '\0';
        printf("Success! Decrypted image saved as: %s\n", buffer);
        write_log("Client", "DECRYPT", "Text data");
    } else {
        printf("Failed to receive response from server.\n");
    }

    close(client_sock);
}

void download_menu() {
    char filename[128];
    printf("Enter the server filename to download: ");
    scanf(" %127s", filename);
    clear_stdin();

    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connect failed");
        close(client_sock);
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "DOWNLOAD|%s", filename);
    write(client_sock, buffer, strlen(buffer));

    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_received = read(client_sock, buffer, sizeof(buffer));
    if (bytes_received > 0) {
        // path lokasi naruh hasil download
        char full_path[256];
        snprintf(full_path, sizeof(full_path), "client/%s", filename);
        FILE *img = fopen(full_path, "wb");

        if (img) {
            fwrite(buffer, 1, bytes_received, img);
            fclose(img);
            printf("Success! Image saved as %s\n", filename);
            write_log("Client", "DOWNLOAD", filename);
        } else {
            printf("Failed to save downloaded image.\n");
        }
    } else {
        printf("Failed to download file.\n");
    }

    close(client_sock);
}

int main() {
    while (1) {
        printf("\n=========================\n");
        printf("| Image Decoder Client |\n");
        printf("=========================\n");
        printf("1. Send input file to server\n");
        printf("2. Download file from server\n");
        printf("3. Exit\n");
        printf(">> ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_stdin();
            continue;
        }
        clear_stdin();

        if (choice == 1) {
            decrypt_menu();
        } else if (choice == 2) {
            download_menu();
        } else if (choice == 3) {
            write_log("Client", "EXIT", "Client requested to exit");
            break;
        } else {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
