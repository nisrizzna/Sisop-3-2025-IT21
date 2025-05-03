#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_ORDER 23

typedef struct {
    char name[50];
    char address[50];
    char type[10]; // "Express" or "Reguler"
    char status[50];
} Order;

Order orders[MAX_ORDER];
pthread_mutex_t lock;

void write_log(const char* agent, const char* type, const char* name, const char* address) {
    FILE *log = fopen("delivery.log", "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d/%02d/%04d %02d:%02d:%02d] [%s] %s package delivered to %s in %s\n",
        t->tm_mday, t->tm_mon+1, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec,
        agent, type, name, address);
    fclose(log);
}

void* express_agent(void* arg) {
    char* agent = (char*) arg;
    while (1) {
        pthread_mutex_lock(&lock);
        int found = 0;
        for (int i = 0; i < MAX_ORDER; i++) {
            if (strcmp(orders[i].type, "Express") == 0 && strcmp(orders[i].status, "Pending") == 0) {
                strcpy(orders[i].status, agent);
                write_log(agent, "Express", orders[i].name, orders[i].address);
                found = 1;
                break;
            }
        }
        pthread_mutex_unlock(&lock);
        if (!found) break;
        usleep(50000);
    }
    return NULL;
}

void init_orders() {
    char* names[MAX_ORDER] = {
        "Acin","Adel","Aisyah","Ayya","Tika","Nayyara","Fio","Aca","Chelsea","Valin","Novi",
        "Dinda","Shafa","Amel","Caca","Alika","Bella","Gina","Livia","Dea","Vivi","Wina","Dira"
    };
    char* addresses[MAX_ORDER] = {
        "Gubeng","Dharmawangsa","Klampis","Semolowaru","Wiyung","Kertajaya","Dukuh","Menur","Ngagel","Ketintang",
        "Wonokromo","Waru","Rungkut","Mulyorejo","Karangmenjangan","Pucang","Tambaksari","Sukolilo","Simokerto",
        "Kenjeran","Sawahan","Sidosermo","Benowo"
    };
    char* types[MAX_ORDER] = {
        "Express","Express","Express","Express","Express","Express","Express","Express","Express",
        "Reguler","Reguler","Reguler","Reguler","Reguler","Reguler","Reguler","Reguler","Reguler",
        "Reguler","Reguler","Reguler","Reguler","Reguler"
    };

    for (int i = 0; i < MAX_ORDER; i++) {
        strcpy(orders[i].name, names[i]);
        strcpy(orders[i].address, addresses[i]);
        strcpy(orders[i].type, types[i]);
        strcpy(orders[i].status, "Pending");
    }
}

int main(int argc, char *argv[]) {
    pthread_mutex_init(&lock, NULL);
    init_orders();

    if (argc == 1) {
        pthread_t tA, tB, tC;
        pthread_create(&tA, NULL, express_agent, "AGENT A");
        pthread_create(&tB, NULL, express_agent, "AGENT B");
        pthread_create(&tC, NULL, express_agent, "AGENT C");
        pthread_join(tA, NULL);
        pthread_join(tB, NULL);
        pthread_join(tC, NULL);
    }
    else if (argc == 3 && strcmp(argv[1], "-deliver") == 0) {
        char* target = argv[2];
        int found = 0;
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_ORDER; i++) {
            if (strcmp(orders[i].name, target) == 0) {
                if (strcmp(orders[i].status, "Pending") != 0) {
                    printf("Paket %s sudah pernah dikirim.\n", target);
                    pthread_mutex_unlock(&lock);
                    return 0;
                }
                strcpy(orders[i].status, argv[0]);
                write_log(argv[0], "Reguler", orders[i].name, orders[i].address);
                printf("Reguler package delivered to %s in %s\n", orders[i].name, orders[i].address);
                found = 1;
                break;
            }
        }
        pthread_mutex_unlock(&lock);
        if (!found) printf("Paket %s tidak ditemukan.\n", target);
    }
    else if (argc == 3 && strcmp(argv[1], "-status") == 0) {
        char* target = argv[2];
        int found = 0;
        for (int i = 0; i < MAX_ORDER; i++) {
            if (strcmp(orders[i].name, target) == 0) {
                printf("Status for %s: %s\n", target, strcmp(orders[i].status, "Pending") == 0 ? "Pending" : 
                    (strstr(orders[i].status, "AGENT") ? 
                    (char*)({ static char tmp[50]; sprintf(tmp,"Delivered by %s",orders[i].status); tmp; }) 
                    : (char*)({ static char tmp[50]; sprintf(tmp,"Delivered by %s",orders[i].status); tmp; })));
                found = 1;
                break;
            }
        }
        if (!found) printf("Paket %s tidak ditemukan.\n", target);
    }
    else if (argc == 2 && strcmp(argv[1], "-list") == 0) {
        for (int i = 0; i < MAX_ORDER; i++) {
            printf("%s - %s (%s)\n", orders[i].name, orders[i].status, orders[i].type);
        }
    }
    else {
        printf("Usage:\n");
        printf("./dispatcher (run express agents)\n");
        printf("./dispatcher -deliver [Name]\n");
        printf("./dispatcher -status [Name]\n");
        printf("./dispatcher -list\n");
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
