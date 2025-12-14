# tugas-besar-struktur-data
Aplikasi Konsol Pemutar Musik

## Nama Anggota Kelompok
- Safira Angraeni		            	(103052400068)
- Talitha Iftitah Gian Nabila		  (103052400081)
- Muhammad Dimas Arteta Dirurasto	(103052400052)

## Deskripsi Aplikasi
Aplikasi ini memungkinkan pengguna untuk mengelola daftar lagu,
memutar lagu, serta melakukan navigasi melalui menu interaktif.
Aplikasi ini dibuat sebagai Tugas Besar Mata Kuliah Struktur Data 
dan memiliki dua peran utama, yaitu **Admin** dan **User**, dengan 
fitur yang berbeda.

## Fitur Aplikasi
### Fitur Admin
- Menambahkan lagu ke dalam library
- Menampilkan seluruh lagu dalam library
- Mengedit data lagu (judul, genre, artis, durasi)
- Menghapus lagu dari library (Penghapusan lagu otomatis
  tersinkronisasi ke seluruh playlist pengguna)
### Fitur User
- Mencari lagu berdasarkan ID atau judul (menggunakan Binary Search Tree)
- Memutar, menjeda, dan melanjutkan lagu
- Navigasi lagu next dan previous
- Membuat dan mengelola playlist
- Menampilkan isi playlist
- Melihat dan memutar ulang lagu dari history
- Menandai lagu favorit
- Mendapatkan rekomendasi lagu berdasarkan genre favorit

## Struktur Data yang Digunakan
- **Doubly Linked List (DLL)**  
  Digunakan untuk menyimpan library lagu dan playlist pengguna, serta mendukung navigasi next dan previous.

- **Binary Search Tree (BST)**  
  Digunakan untuk mempercepat pencarian lagu berdasarkan ID atau judul lagu.

- **Stack**  
  Digunakan untuk menyimpan riwayat lagu yang telah diputar dengan konsep LIFO (Last In First Out).

- **Multi Linked List (MLL)**  
  Digunakan untuk menghubungkan data lagu pada library dengan playlist pengguna tanpa duplikasi data.

## Cara Menjalankan
### Persyaratan
- Visual Studio Code
- Compiler C++ (contoh: `g++`)
- Extension C/C++ di Visual Studio Code
- Sistem Operasi Windows / Linux / MacOS
## Menjalankan Program di Visual Studio Code
1. Buka folder project menggunakan **Visual Studio Code**
2. Pastikan seluruh file source (`.cpp`) berada dalam satu folder
3. Buka **Terminal** di VS Code (`Ctrl + ``)
4. Jalankan perintah kompilasi berikut:  g++ *.cpp -o music-player
5. Setelah kompilasi berhasil, jalankan program dengan perintah: **./music-player**
