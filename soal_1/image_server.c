#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/image_socket"
#define LOG_PATH "/home/faizi/sisop/modul_3/soal_1/server/server.log"
#define DATABASE_DIR "/home/faizi/sisop/modul_3/soal_1/server/database/"

#define BUFFER_SIZE 4096

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); 

    umask(0);
    setsid();

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void write_log(const char* source, const char* action, const char* info) {
    FILE *log = fopen(LOG_PATH, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char time_buf[64];
    strftime(time_buf, 64, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log, "[%s][%s]: [%s] [%s]\n", source, time_buf, action, info);
    fclose(log);
}

void generate_filename(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y%m%d_%H%M%S.jpeg", tm_info);
}

int main() {
    daemonize();

    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    unlink(SOCKET_PATH); 

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        write_log("Server", "ERROR", "Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        write_log("Server", "ERROR", "Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        write_log("Server", "ERROR", "Listen failed");
        exit(EXIT_FAILURE);
    }

    write_log("Server", "INFO", "Server started");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            write_log("Server", "ERROR", "Accept failed");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            write_log("Server", "ERROR", "Failed to read data from client");
            close(client_fd);
            continue;
        }

        if (strncmp(buffer, "DECRYPT|", 8) == 0) {
            char filename[64];
            generate_filename(filename, sizeof(filename));

            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s%s", DATABASE_DIR, filename);

            char *data = buffer + 8;
            size_t data_len = bytes_read - 8;

            FILE *img = fopen(filepath, "wb");
            if (img) {
                fwrite(data, 1, data_len, img);
                fclose(img);

                write_log("Server", "SAVE", "Text Data");  
                write(client_fd, filename, strlen(filename));
            } else {
                write_log("Server", "ERROR", "Failed to save image");
                char *err = "ERROR|Save failed";
                write(client_fd, err, strlen(err));
            }
        }
        else if (strncmp(buffer, "DOWNLOAD|", 9) == 0) {
            char *requested_file = buffer + 9;

            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s%s", DATABASE_DIR, requested_file);

            FILE *file = fopen(filepath, "rb");
            if (!file) {
                write_log("Server", "ERROR", "Requested file not found");
                char *err_msg = "ERROR|File not found on server.";
                write(client_fd, err_msg, strlen(err_msg));
            } else {
                char send_buf[BUFFER_SIZE];
                size_t bytes_sent;

                while ((bytes_sent = fread(send_buf, 1, BUFFER_SIZE, file)) > 0) {
                    write(client_fd, send_buf, bytes_sent);
                }

                fclose(file);
                write_log("Server", "UPLOAD", requested_file);
            }
        }
        else if (strncmp(buffer, "EXIT", 4) == 0) {
            write_log("Server", "EXIT", "Client requested to exit");
            close(client_fd);
            continue;
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
