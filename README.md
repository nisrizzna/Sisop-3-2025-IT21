# Laporan Praktikum Sisop Modul 1 Kelompok IT21
# Anggota
1. Nisrina Bilqis - 5027241054
2. Hanif Mawla Faizi - 5027241064
3. Dina Rahmadani - 5027241065

# Soal_1
---

##  Deskripsi Singkat Soal
Soal meminta implementasi sistem client-server dengan konsep RPC menggunakan UNIX Domain Socket, di mana:
- Client dapat mengirim file `.txt` terenkripsi (berisi data gambar dalam hex/encoded).
- Server memproses file tersebut dan menyimpan hasilnya sebagai file `.jpeg`.
- Client juga dapat mengunduh file `.jpeg` dari server berdasarkan nama file yang sebelumnya diterima.
- Semua komunikasi dilakukan lewat socket dan tercatat ke dalam log `server/server.log`.

## 🗂️ Struktur Direktori

```bash
.
├── client/
│   ├── image_client
│   ├── image_client.c
│   └── secrets/
│       ├── input_1.txt ... input_5.txt
├── server/
│   ├── image_server
│   ├── image_server.c
│   ├── database/
│   └── server.log
├── <timestamp>.jpeg (hasil decode)
```
---

### (a) Setup dan 
Setelah mengunduh dan mengekstrak `secrets.zip`, file teks disimpan ke dalam `client/secrets/`.

```bash
unzip secrets.zip -d client/secrets/
```

---

### (b) Daemon Server dengan UNIX Socket
Server dibuat menjadi daemon agar berjalan di background. Menggunakan UNIX Domain Socket (AF_UNIX), komunikasi dilakukan lewat file socket di /tmp/image_socket.

Kode daemonisasi:
```c
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Parent exit

    umask(0);
    setsid();
    chdir("/"); // optional
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
```

Socket setup:
```c
int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
struct sockaddr_un addr = {0};
addr.sun_family = AF_UNIX;
strncpy(addr.sun_path, "/tmp/image_socket", sizeof(addr.sun_path) - 1);

bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
listen(server_fd, 5);
```

---

### (c) Proses DECRYPT (Client → Server)
Client membaca file teks dari client/secrets/, lalu mengirim data ke server dalam format:
```c
[DECRYPT]<Text Data>
```

Server menyimpan hasil sebagai file .jpeg dengan nama berdasarkan timestamp di server/database/.
Kode pada client:
```c
char message[BUFFER_SIZE];
snprintf(message, sizeof(message), "DECRYPT|%s", file_data);
write(sock, message, strlen(message));
```

Kode pada server:
```c
if (strncmp(buffer, "DECRYPT|", 8) == 0) {
    char filename[64];
    generate_filename(filename, sizeof(filename)); // e.g., 20250503_034856.jpeg
    FILE *img = fopen(full_path, "wb");
    fwrite(buffer + 8, 1, strlen(buffer + 8), img);
    write(client_fd, filename, strlen(filename));
}
```

---

### (d) Menu Interaktif Client
Client menampilkan menu yang memungkinkan user mengirim file berkali-kali atau mengunduh file dari server. Setiap loop membuat koneksi socket baru agar tidak terjadi reuse socket error.
Menu tampil seperti:
```md
=========================
| Image Decoder Client |
=========================
1. Send input file to server
2. Download file from server
3. Exit
```

Loop kode:
```c
while (1) {
    printf("1. Send input file...\n");
    scanf("%d", &choice);
    clear_stdin();

    if (choice == 1) decrypt_menu();
    else if (choice == 2) download_menu();
    else break;
}
```

---

### (e) Proses DOWNLOAD (Client Request File)
Client mengirim perintah:
```c
[DOWNLOAD]<filename.jpeg>
```
Server membalas dengan isi file JPEG, lalu client menyimpannya di client/.
Potongan kode server:
```c
if (strncmp(buffer, "DOWNLOAD|", 9) == 0) {
    FILE *img = fopen(filepath, "rb");
    while ((n = fread(buf, 1, BUFFER_SIZE, img)) > 0) {
        write(client_fd, buf, n);
    }
}
```

Potongan kode client:
```c
FILE *img = fopen("client/<filename>", "wb");
while ((n = read(client_sock, buffer, BUFFER_SIZE)) > 0) {
    fwrite(buffer, 1, n, img);
}
```

---

### (f) Penanganan Error
Error seperti:
- File tidak ditemukan
- Socket gagal dibuat
- File tidak bisa dibuka
ditangani dengan perror() dan pengecekan return value.

Log error juga dicatat:
```c
write_log("Server", "ERROR", "File not found on DOWNLOAD");
```

---

### (g) Logging
Semua aktivitas client dan server dicatat ke file log server/server.log dalam format:
```md
[Source][YYYY-MM-DD HH:MM:SS]: [ACTION] [Info]
```

Contoh :
```md
[Client][2025-05-03 04:34:10]: [DECRYPT] [Text data]
[Server][2025-05-03 04:34:10]: [SAVE] [20250503_043410.jpeg]
[Client][2025-05-03 04:34:45]: [DOWNLOAD] [20250503_043410.jpeg]
[Server][2025-05-03 04:34:45]: [UPLOAD] [20250503_043410.jpeg]
```
