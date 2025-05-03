#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void log_delivery(const char *name, const char *address, const char *type) {
    const char *agent = getenv("USER");
    if (!agent) agent = "Unknown";

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", t);

    FILE *f = fopen("delivery.log", "a");
    if (f) {
        fprintf(f, "[%s] [%s] %s package delivered to %s in %s\n",
                time_str, agent, type, name, address);
        fclose(f);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: delivery_agent <name> <address> <type>\n");
        return 1;
    }

    const char *name = argv[1];
    const char *address = argv[2];
    const char *type = argv[3];

    printf("%s package delivered to %s in %s\n", type, name, address);
    log_delivery(name, address, type);

    return 0;
}


