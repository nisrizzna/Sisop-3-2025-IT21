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
├── 20250503_XXXXXX.jpeg (hasil decode)
```
---

