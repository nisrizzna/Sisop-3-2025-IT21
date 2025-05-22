# Laporan Praktikum Sisop Modul 3 Kelompok IT21
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

# Soal_2

# Deskripsi Program:
Program ini merupakan sistem simulasi pengiriman barang yang terdiri dari dua jenis layanan yaitu Express dan Reguler. Program menggunakan multithreading untuk memproses pengiriman Express secara otomatis melalui 3 agen ekspedisi (AGENT A, AGENT B, dan AGENT C) yang berjalan secara paralel. Untuk pengiriman Reguler, pengguna dapat mengirim paket secara manual melalui parameter command line.

Seluruh proses pengiriman akan dicatat ke dalam file log bernama ```delivery.log``` beserta informasi waktu, agen pengirim, jenis layanan, nama penerima, dan alamat tujuan.

# Struktur Data
``` typedef struct {
    char name[50];
    char address[50];
    char type[10];    // "Express" or "Reguler"
    char status[50];  // "Pending" / "AGENT A" / argv[0] dll
} Order;
 ```

# Format Log
``` [dd/mm/yyyy hh:mm:ss] [AGENT] [TYPE] package delivered to [NAME] in [ADDRESS] ```

# Cara Menjalankan Program:
Menjalankan Express Delivery (Multithreading):
```./dispatcher```
Melakukan Reguler Delivery:
```./dispatcher -deliver [Name]```
Cek Status Paket:
```./dispatcher -status [Name]```
Menampilkan Daftar Semua Order:
```./dispatcher -list```

# Output
![image](https://github.com/user-attachments/assets/331f765d-4604-431f-8a51-5359e6bcbd81)
![image](https://github.com/user-attachments/assets/50d39e1c-c6b3-4d95-ada9-2981caac4202)

# Kesimpulan:
Program Delivery Management System berhasil menerapkan konsep multithreading untuk memproses pengiriman secara paralel dan pengiriman manual berbasis command line. Fitur logging memastikan seluruh aktivitas tercatat dengan detail waktu, agen, jenis pengiriman, nama, dan alamat.

Program ini mampu mendemonstrasikan kontrol sinkronisasi dengan mutex lock untuk menghindari race condition saat memodifikasi data status pengiriman di array struct secara bersama-sama oleh banyak thread.


# Soal_3

# 🥅 Blue Lock: Goal Attack

> Sistem client-server sederhana berbasis C untuk simulasi pertarungan menggunakan konsep battle arena dan senjata dengan efek pasif.

## 📂 Struktur File

| File        | Deskripsi                                                                                                                    |
| ----------- | ---------------------------------------------------------------------------------------------------------------------------- |
| `dungeon.c` | File server. Menangani koneksi client, penyimpanan data player, dan logika utama seperti battle, shop, dan inventory.        |
| `player.c`  | File client. Menyediakan antarmuka interaktif untuk player, mengirim perintah ke server, dan menampilkan respon.             |
| `shop.h`    | Header file. Mendefinisikan struct `Weapon`, variabel `weapon_list`, dan fungsi `init_weapons()` untuk inisialisasi senjata. |

---

## 🧩 Penjelasan Bagian Kode

### `shop.h`

```c
typedef struct {
    char name[64];
    int price;
    int damage;
    char passive[64];
    int passive_chance;
} Weapon;
```

* **Struct Weapon**: merepresentasikan senjata dalam game, lengkap dengan harga, damage, nama efek pasif, dan peluang efek pasif muncul (%).
* **`weapon_list` dan `weapon_count`**: array global berisi semua senjata yang tersedia.
* **`init_weapons()`**: mengisi `weapon_list` dengan senjata-senjata awal (contohnya senjata bertema karakter Blue Lock seperti *Isagi's Adaptability*).

---

### `dungeon.c` (SERVER)

#### Struct Player

```c
typedef struct {
    int gold;
    int base_damage;
    int kills;
    int weapon_id;
} Player;
```

* Menyimpan status player yang sedang aktif dalam koneksi: emas, damage dasar, jumlah kill, dan senjata yang dipakai.

#### Fungsi `handle_client()`

* Fungsi utama yang menangani seluruh siklus permainan per client (perintah, shop, battle, dll).
* Perintah yang didukung:

  * `"SHOW_STATS"` – Menampilkan statistik player saat ini.
  * `"SHOW_SHOP"` – Menampilkan daftar senjata.
  * `"BUY_WEAPON <id>"` – Membeli dan meng-equip senjata berdasarkan ID.
  * `"VIEW_INVENTORY"` – Menampilkan senjata yang sedang digunakan.
  * `"START_BATTLE"` – Memulai pertarungan dengan musuh acak.

#### Logika Battle

```c
if (w->passive_chance > 0 && (rand() % 100 < w->passive_chance)) {
    // Efek pasif diaktifkan
}
```

* Passive senjata memiliki efek unik seperti tambahan damage, heal, atau debuff musuh.
* HP player dan musuh dikelola dalam satu loop, menunggu input `"attack"` atau `"run"` dari client.

---

### `player.c` (CLIENT)

#### Fungsi `send_command()`

* Mengirim perintah string ke server dan menerima respon balik.

```c
write(sock, command, strlen(command));
read(sock, buffer, sizeof(buffer));
```

#### Fungsi `print_menu()`

* Menampilkan tampilan utama dengan pilihan aksi seperti lihat stats, masuk ke shop, battle, dsb.

#### Loop Utama

* Meminta input user berupa angka untuk memilih aksi.
* Jika masuk ke battle, client akan masuk ke loop `fgets()` untuk menerima perintah `"attack"` atau `"run"` dan mencetak hasil dari server.

---

## 💡 Catatan Teknis

* Komunikasi client-server menggunakan `socket TCP`.
* Server bersifat blocking dan satu client ditangani satu per satu.
* Warna terminal menggunakan ANSI escape code (misal: `\033[1;34m` untuk biru tebal).

# REVISI (Tampilan Baru)
## Main Menu
![image](https://github.com/user-attachments/assets/32c1355d-089a-4484-b955-c122d75db99f)
## Player Stats
![image](https://github.com/user-attachments/assets/3d80fedb-56bc-46be-b718-d8d71f72e14f)
## Shop
![image](https://github.com/user-attachments/assets/1608b8ca-f0d2-4102-9f66-feabe83c0c1c)
## Inventory
![image](https://github.com/user-attachments/assets/badc361a-2833-4e58-bae0-24f0f54f65b2)
## Battle
![image](https://github.com/user-attachments/assets/e0ce79bc-743f-450e-9d33-ebb562e96d33)

# Soal_4

# Program ini terdiri dari dua bagian:

1. # Program Hunter
    Sistem registrasi, login, dan tampilan profil hunter menggunakan shared memory untuk menyimpan data hunter.
2. # Program Dungeon System
   Sistem pembuatan dungeon secara acak dan penyimpanan daftar dungeon.
   Kedua program ini digunakan dalam simulasi game berbasis CLI (Command Line Interface) dengan konsep manajemen user (hunter) dan dungeon.

# Struktur Data
```Struct Hunter
typedef struct {
    char username[50];
    int level;
    int exp;
    int atk;
    int hp;
    int def;
    int banned;
} Hunter;
```
# Struct SystemData

```typedef struct {
    int num_hunters;
    Hunter hunters[MAX_HUNTERS];
} SystemData;
```

# Struct Dungeon

```struct Dungeon {
    char name[50];
    int min_level;
    int atk;
    int hp;
    int def;
    int exp;
    int key;
};
```

 # Mekanisme Shared Memory
Key shared memory diambil menggunakan:

```key_t key = get_system_key();
Kemudian diakses menggunakan:
```

```int shmid = shmget(key, sizeof(SystemData), 0666);
SystemData *sys_data = (SystemData *) shmat(shmid, NULL, 0);
Data hunter disimpan di dalam sys_data.
```
#  Mekanisme Random Dungeon
Dungeon dihasilkan menggunakan rand():
```
int random_index = rand() % 10;
dungeon->atk = rand() % 51 + 100;
dungeon->hp = rand() % 51 + 50;
Nama dungeon diambil dari array string dungeon_names.
```
# Output
![image](https://github.com/user-attachments/assets/d1a28418-7e5c-4709-abec-c20c2d4ac958)
![image](https://github.com/user-attachments/assets/bc3b54ca-3ddc-4a27-9901-adf1857973fa)


