# Laporan Resmi SisOp Modul 3 Kelompok IT25

**Anggota kelompok IT25**:
* Fiorenza Adelia Nalle (5027231053)
* Tio Axellino Irin (5027231065)
* Wira Samudra Siregar (5027231041)

# Deskripsi Praktikum Modul 3

Pada Praktikum Modul 3 ini, kami diberikan tugas untuk mengerjakan Soal Shift Modul 3. Berikut adalah pembagian pengerjaan Soal Shift Modul 3:
* Soal 1 yang dikerjakan oleh Tio Axellino Irin (5027231065) 
* Soal 2 yang dikerjakan oleh Fiorenza Adelia Nalle (5027231053)
* Soal 3 yang dikerjakan oleh Fiorenza Adelia Nalle (5027231053)
* Soal 4 yang dikerjakan oleh Wira Samudra Siregar (5027231041)

# Proses dan Hasil Praktikum Modul 2

## Soal 1

**oleh Tio Axellino Irin (5027231065)**

### Deskripsi Soal 1

Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa. Programnya berbentuk microservice sebagai berikut:

a. Dalam **auth.c** pastikan file yang masuk ke folder **new-entry** adalah file csv dan berakhiran  **trashcan** dan **parkinglot**. Jika bukan, program akan secara langsung akan delete file tersebut. 

Contoh dari nama file yang akan diautentikasi:

* belobog_trashcan.csv

* osaka_parkinglot.csv

b. Format data (Kolom)  yang berada dalam file csv adalah seperti berikut:

![image](https://github.com/fioreenza/Sisop-3-2024-MH-IT25/assets/144349814/86243a49-4395-42dc-8b54-5a2168a74c48)

atau

![image](https://github.com/fioreenza/Sisop-3-2024-MH-IT25/assets/144349814/0c1d57cc-b3ca-4e42-b1b9-b21e12e1c1ec)

c. File csv yang lolos tahap autentikasi akan dikirim ke shared memory. 

d. Dalam **rate.c**, proses akan mengambil data csv dari shared memory dan akan memberikan output Tempat Sampah dan Parkiran dengan Rating Terbaik dari data tersebut.

![image](https://github.com/fioreenza/Sisop-3-2024-MH-IT25/assets/144349814/2cc5e160-3111-4898-a735-048d5ce8ee22)

e. Pada **db.c**, proses bisa memindahkan file dari **new-data** ke folder **microservices/database**, **WAJIB MENGGUNAKAN SHARED MEMORY**.

f. Log semua file yang masuk ke folder **microservices/database** ke dalam file **db.log** dengan contoh format sebagai berikut:

[DD/MM/YY hh:mm:ss] [type] [filename]

ex : `[07/04/2024 08:34:50] [Trash Can] [belobog_trashcan.csv]`

Contoh direktori awal:

    .
    ├── auth.c
    ├── microservices
    │   ├── database
    │   │   └── db.log
    │   ├── db.c
    │   └── rate.c
    └── new-data
        ├── belobog_trashcan.csv
        ├── ikn.csv
        └── osaka_parkinglot.csv

Contoh direktori akhir setelah dijalankan auth.c dan db.c:

    .
    ├── auth.c
    ├── microservices
    │   ├── database
    │   │   ├── belobog_trashcan.csv
    │   │   ├── db.log
    │   │   └── osaka_parkinglot.csv
    │   ├── db.c
    │   └── rate.c
    └── new-data

### Penyelesaian Soal 1

* **auth.c**

        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <unistd.h>
        #include <sys/types.h>
        #include <sys/ipc.h>
        #include <sys/shm.h>
        #include <dirent.h>

        #define SHM_SIZE 1024

        // Fungsi untuk menghapus file yang tidak valid
        void deleteInvalidFile(char* filename) {
            printf("Deleting invalid file: %s\n", filename);
            remove(filename);
        }

        int main() {
            // Buat shared memory
            key_t key = 12345;
            int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
            char *shm = (char *)shmat(shmid, NULL, 0);
        //    char *values = (char *)malloc(SHM_SIZE);
        //    if (values == NULL) {
        //        perror("Error allocating memory");
        //        return EXIT_FAILURE;
        //    }

            // Variabel untuk menyimpan hasil printf sebelum fclose
            char printf_buffer[4096];
            printf_buffer[0] = '\0'; // Nolkan buffer

            // Cek setiap file yang masuk ke folder new-entry
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir ("new-data")) != NULL) {
                while ((ent = readdir (dir)) != NULL) {
                    char* filename = ent->d_name;
                    // Cek apakah file csv dan berakhiran trashcan atau parkinglot
                    if (strstr(filename, ".csv") != NULL && (strstr(filename, "trashcan") != NULL || strstr(filename, "parkinglot") != NULL)) {
                        // Copy nama file ke shared memory
                        strncpy(printf_buffer, filename, SHM_SIZE);
                        shm[SHM_SIZE - 1] = '\0';
                strcat(printf_buffer, "\n"); // Baris kosong sebagai pemisah
                // Buka file csv
                char filepath[100];
                        sprintf(filepath, "new-data/%s", filename);
                        FILE *file = fopen(filepath, "r");
                        if (file == NULL) {
                            perror("Error opening file");
                            return EXIT_FAILURE;
                        }

                        // Salin isi file ke shared memory
                        char line[100];
                        while (fgets(line, sizeof(line), file) != NULL) {
                            strcat(printf_buffer, line);
        //		    strcat(printf_buffer, line);
                        }
                strcat(shm, printf_buffer);
        //                strcat(shm, "\n"); // Baris kosong ke buffer printf
        //		printf("%s\n", values);
                        fclose(file);
        //		printf("%s\n", value);
        //		shmdt(shm);
                    } else {
                        // Jika tidak sesuai, hapus file tersebut
                        char filepath[100];
                        sprintf(filepath, "new-data/%s", filename);
                        deleteInvalidFile(filepath);
                    }
                }
                closedir (dir);
            } else {
                perror ("Could not open directory");
                return EXIT_FAILURE;
            }

            // Detach shared memory
            printf("%s", shm);
            shmdt(shm);
        //    shmctl(shmid, IPC_RMID, NULL);

            return EXIT_SUCCESS;
        }

### Kendala Pengerjaan Soal 2


### Screenshot Hasil Pengerjaan Soal 2


## Soal 2
**oleh Fiorenza Adelia Nalle (5027231053)**
### Deskripsi Soal 2
Max Verstappen 🏎️ seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.
(Wajib menerapkan konsep pipes dan fork seperti yang dijelaskan di modul Sisop. Gunakan 2 pipes dengan diagram seperti di modul 3).
Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program parent process dan child process, ada input dari user berupa 2 string. Contoh input: tiga tujuh. 
Pada parent process, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah. Contoh: tiga tujuh menjadi 21. 
Pada child process, program akan mengubah hasil angka yang telah diperoleh dari parent process menjadi kalimat. Contoh: `21` menjadi “dua puluh satu”.
Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah argumen untuk menjalankan program : 
perkalian	: ./kalkulator -kali
penjumlahan	: ./kalkulator -tambah
pengurangan	: ./kalkulator -kurang
pembagian	: ./kalkulator -bagi
Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah.
Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada child process) kalimat akan di print dengan contoh format : 
perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”
penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”
pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”
pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”
Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama histori.log. Pada parent process, lakukan pembuatan file log berdasarkan data yang dikirim dari child process. 
Format: [date] [type] [message]
Type: KALI, TAMBAH, KURANG, BAGI
Ex:
[10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.
[10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.
[10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.

### Penyelesaian Soal 2
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <sys/wait.h>
	#include <time.h>
	
	#define MAX_LENGTH 100
	
	void numberToWords(int num, char *words) {
	    char *units[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan", "sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
	    char *tens[] = {"", "", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};
	
	    if (num < 20) {
	        strcpy(words, units[num]);
	    } else if (num < 100) {
	        strcpy(words, tens[num / 10]);
	        if (num % 10 != 0) {
	            strcat(words, " ");
	            strcat(words, units[num % 10]);
	        }
	    }
	}
	
	int word_to_number(const char* word) {
	    if (strcmp(word, "nol") == 0) {
	        return 0;
	    }
	    const char* numbers[] = {"satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
	    for (int i = 0; i < 9 ; ++i) {
	        if (strcmp(word, numbers[i]) == 0) {
	            return i + 1;
	        }
	    }
	    return -1;
	}
	
	void writeLog(const char *type, const char *message) {
	    FILE *log_file = fopen("histori.log", "a");
	    if (log_file == NULL) {
	        perror("histori.log");
	        exit(EXIT_FAILURE);
	    }
	    time_t now;
	    struct tm *local_time;
	    time(&now);
	    local_time = localtime(&now);
	    fprintf(log_file, "[%02d/%02d/%02d %02d:%02d:%02d] [%s] %s\n", local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year % 100, local_time->tm_hour, local_time->tm_min, local_time->tm_sec, type, message);
	    fclose(log_file);
	}
	
	int main(int argc, char *argv[]) {
	    if (argc != 4) {
	        fprintf(stderr, "Usage: %s <operation> <num1> <num2>\n", argv[0]);
	        return EXIT_FAILURE;
	    }
	
	    char *operation = argv[1];
	    char *num1_word = argv[2];
	    char *num2_word = argv[3];
	
	    int fd[2];
	    if (pipe(fd) == -1) {
	        perror("Pipe failed");
	        exit(EXIT_FAILURE);
	    }
	
	    pid_t pid = fork();
	    if (pid == -1) {
	        perror("Fork failed");
	        exit(EXIT_FAILURE);
	    }
	
	    if (pid == 0) { 
	        close(fd[0]); 
	
	        int num1, num2, result;
	        char result_word[MAX_LENGTH];
	        char message[MAX_LENGTH * 2];
	
	        num1 = word_to_number(num1_word);
	    	num2 = word_to_number(num2_word);
	    	
	    	if (num1 == -1 && num2 == -1) {
	    	   fprintf(stderr, "Input tidak valid.\n");
	    	   return EXIT_FAILURE;
		}
	
	        if (strcmp(operation, "-kali") == 0) {
	            result = num1 * num2;
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil perkalian %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("KALI", message);
	        } else if (strcmp(operation, "-tambah") == 0) {
	            result = num1 + num2;
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil penjumlahan %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("TAMBAH", message);
	        } else if (strcmp(operation, "-kurang") == 0) {
	            result = num1 - num2;
	            if (result < 0) {
	                printf("ERROR.");
	                return EXIT_FAILURE;
	            }
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil pengurangan %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("KURANG", message);
	        } else if (strcmp(operation, "-bagi") == 0) {
	            result = num1/num2 ;
	            if (num2 == 0) {
	        	printf("ERROR.");
	            	return EXIT_FAILURE;
	    	    }
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil pembagian %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("BAGI", message);
	        } else {
	            fprintf(stderr, "Operasi tidak valid: %s\n", operation);
	            return EXIT_FAILURE;
	        }
	
	        write(fd[1], message, strlen(message) + 1);
	        close(fd[1]); 
	        exit(EXIT_SUCCESS);
	    } else { 
	        close(fd[1]);
	
	        wait(NULL);
	
	        char message[MAX_LENGTH * 2];
	   
	        read(fd[0], message, sizeof(message));
	        close(fd[0]); 
	
	        printf("%s\n", message);
	    }
	
	    return EXIT_SUCCESS;
	}


### Penjelasan Soal 2

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <sys/wait.h>
	#include <time.h>
	
	#define MAX_LENGTH 100
Bagian ini sebagai header untuk menambahkan library dan mendefinisikan panjang maksimal dari sebuah string.
	
	void numberToWords(int num, char *words) {
	    char *units[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan", "sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
	    char *tens[] = {"", "", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};
	
	    if (num < 20) {
	        strcpy(words, units[num]);
	    } else if (num < 100) {
	        strcpy(words, tens[num / 10]);
	        if (num % 10 != 0) {
	            strcat(words, " ");
	            strcat(words, units[num % 10]);
	        }
	    }
	}
Fungsi numberToWords mengubah angka menjadi kata-kata dalam bahasa Indonesia. Jika angka kurang dari 20, kata-kata diambil dari array units. Jika angka antara 20 dan 99, kata-kata diambil dari array tens untuk puluhan dan units untuk satuan, lalu digabungkan. Hasilnya disimpan dalam parameter words.
	
	int word_to_number(const char* word) {
	    if (strcmp(word, "nol") == 0) {
	        return 0;
	    }
	    const char* numbers[] = {"satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
	    for (int i = 0; i < 9 ; ++i) {
	        if (strcmp(word, numbers[i]) == 0) {
	            return i + 1;
	        }
	    }
	    return -1;
	}
Fungsi word_to_number mengonversi kata-kata bahasa Indonesia menjadi angka. Jika kata adalah "nol", itu mengembalikan 0. Jika kata adalah salah satu dari "satu" hingga "sembilan", itu mengembalikan nilai yang sesuai (1 hingga 9). Jika merupakan kata lain, program akan mengembalikan -1.
  
	void writeLog(const char *type, const char *message) {
	    FILE *log_file = fopen("histori.log", "a");
	    if (log_file == NULL) {
	        perror("histori.log");
	        exit(EXIT_FAILURE);
	    }
	    time_t now;
	    struct tm *local_time;
	    time(&now);
	    local_time = localtime(&now);
	    fprintf(log_file, "[%02d/%02d/%02d %02d:%02d:%02d] [%s] %s\n", local_time->tm_mday, local_time->tm_mon + 1, local_time->tm_year % 100, local_time->tm_hour, local_time->tm_min, local_time->tm_sec, type, message);
	    fclose(log_file);
	}
Fungsi writeLog digunakan untuk mencatat pesan log ke dalam file "histori.log". Fungsi ini pertama-tama membuka file log untuk ditambahkan ("a"). Kemudian, ia mengambil waktu saat ini dan memformat pesan log dengan informasi tanggal, waktu, jenis operasi, dan pesan yang diberikan. Pesan log tersebut kemudian ditulis ke dalam file log, dan file log ditutup setelah selesai. Jika ada masalah dalam membuka file log, program akan keluar dengan status kesalahan.

	int main(int argc, char *argv[]) {
	    if (argc != 4) {
	        fprintf(stderr, "Usage: %s <operation> <num1> <num2>\n", argv[0]);
	        return EXIT_FAILURE;
	    }
Program memeriksa jumlah argumen yang diberikan. Jika tidak sesuai (tidak ada operasi atau angka yang diberikan), program mencetak pesan penggunaan dan keluar dengan status kesalahan.
	
	    char *operation = argv[1];
	    char *num1_word = argv[2];
	    char *num2_word = argv[3];
Di sini, program mengambil operasi (seperti "-kali", "-tambah", dll.) dan angka-angka yang akan dioperasikan dari argumen baris perintah.
	
	    int fd[2];
	    if (pipe(fd) == -1) {
	        perror("Pipe failed");
	        exit(EXIT_FAILURE);
	    }
Program membuat sebuah pipe menggunakan pipe() untuk menghubungkan proses induk dengan proses anak. Jika pemanggilan pipe() mengembalikan nilai -1, itu berarti gagal membuat pipe. 
	
	    pid_t pid = fork();
	    if (pid == -1) {
	        perror("Fork failed");
	        exit(EXIT_FAILURE);
	    }
Proses induk membuat proses anak dengan memanggil fungsi fork(). Jika fork() mengembalikan nilai -1, itu berarti gagal menciptakan proses anak. Dalam hal ini, pesan kesalahan dicetak menggunakan perror() dan program keluar dengan status kesalahan.
	
	    if (pid == 0) { 

	        close(fd[0]); 
	
	        int num1, num2, result;
	        char result_word[MAX_LENGTH];
	        char message[MAX_LENGTH * 2];
	
	        num1 = word_to_number(num1_word);
	    	num2 = word_to_number(num2_word);
	    	
	    	if (num1 == -1 && num2 == -1) {
	    	   fprintf(stderr, "Input tidak valid.\n");
	    	   return EXIT_FAILURE;
		}
Proses anak menutup ujung bacaan dari pipe (fd[0]). Variabel num1 dan num2 digunakan untuk menyimpan angka yang diperoleh dari fungsi word_to_number, yang mengonversi kata-kata menjadi angka. Jika kedua angka yang diberikan tidak valid (tidak ditemukan dalam kumpulan kata-kata yang dapat diubah menjadi angka), maka program mencetak pesan kesalahan. 
	
	        if (strcmp(operation, "-kali") == 0) {
	            result = num1 * num2;
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil perkalian %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("KALI", message);
	        } else if (strcmp(operation, "-tambah") == 0) {
	            result = num1 + num2;
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil penjumlahan %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("TAMBAH", message);
	        } else if (strcmp(operation, "-kurang") == 0) {
	            result = num1 - num2;
	            if (result < 0) {
	                printf("ERROR.");
	                return EXIT_FAILURE;
	            }
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil pengurangan %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("KURANG", message);
	        } else if (strcmp(operation, "-bagi") == 0) {
	            result = num1/num2 ;
	            if (num2 == 0) {
	        	printf("ERROR.");
	            	return EXIT_FAILURE;
	    	    }
	            numberToWords(result, result_word);
	            snprintf(message, sizeof(message), "hasil pembagian %s dan %s adalah %s.", num1_word, num2_word, result_word);
	            writeLog("BAGI", message);
	        } else {
	            fprintf(stderr, "Operasi tidak valid: %s\n", operation);
	            return EXIT_FAILURE;
	        }
Kemudian, berdasarkan operasi yang diberikan (-kali, -tambah, -kurang, atau -bagi), dilakukan operasi aritmatika yang sesuai. Setelah operasi aritmatika dilakukan, hasilnya disimpan dalam variabel result. Fungsi numberToWords() kemudian dipanggil untuk mengonversi hasil tersebut menjadi kata-kata dalam bahasa Indonesia, yang disimpan dalam variabel result_word. Pesan hasil operasi dibuat menggunakan snprintf() dengan format yang sesuai. Pesan tersebut kemudian dicatat dalam file log menggunakan fungsi writeLog() dengan menentukan jenis operasi yang dilakukan.
	
	        write(fd[1], message, strlen(message) + 1);
	        close(fd[1]); 
	        exit(EXIT_SUCCESS);
Pesan hasil operasi ditulis ke dalam ujung penulisan pipe (fd[1]) menggunakan fungsi write(). Setelah penulisan selesai, ujung penulisan pipe ditutup untuk memberitahu proses induk bahwa semua data telah ditulis. Terakhir, proses anak keluar dengan status keluaran yang sukses (EXIT_SUCCESS).

	    } else { 
	        close(fd[1]);
	
	        wait(NULL);
	
	        char message[MAX_LENGTH * 2];
	   
	        read(fd[0], message, sizeof(message));
	        close(fd[0]); 
	
	        printf("%s\n", message);
	    }
	
	    return EXIT_SUCCESS;
	}
Proses induk menutup ujung penulisan dari pipe (fd[1]) karena proses induk tidak akan menulis ke dalam pipe. Proses induk menunggu proses anak selesai menggunakan wait(NULL). Setelah itu, proses induk membaca pesan hasil operasi dari pipe menggunakan read() dan mencetaknya ke layar. Akhirnya, ujung pembacaan dari pipe ditutup (fd[0]).

### Hasil Pengerjaan Soal 2

<img width="722" alt="Screenshot 2024-05-11 at 20 58 04" src="https://github.com/fioreenza/Sisop-3-2024-MH-IT25/assets/147926732/9c560390-3007-4aea-9bc0-060b604ddc11">

<img width="1202" alt="Screenshot 2024-05-11 at 20 58 44" src="https://github.com/fioreenza/Sisop-3-2024-MH-IT25/assets/147926732/4b0c624f-540f-4cd5-81fb-de6af906ea62">



## Soal 3
**oleh Fiorenza Adelia Nalle (5027231053)**
### Deskripsi Soal 1
Shall Leglerg🥶 dan Carloss Signs 😎 adalah seorang pembalap F1 untuk tim Ferrari 🥵. Mobil F1 memiliki banyak pengaturan, seperti penghematan ERS, Fuel, Tire Wear dan lainnya. Pada minggu ini ada race di sirkuit Silverstone. Malangnya, seluruh tim Ferrari diracun oleh Super Max Max pada hari sabtu sehingga seluruh kru tim Ferrari tidak bisa membantu Shall Leglerg🥶 dan Carloss Signs 😎 dalam race. Namun, kru Ferrari telah menyiapkan program yang bisa membantu mereka dalam menyelesaikan race secara optimal. Program yang dibuat bisa mengatur pengaturan - pengaturan dalam mobil F1 yang digunakan dalam balapan. Programnya ber ketentuan sebagai berikut:
Pada program actions.c, program akan berisi function function yang bisa di call oleh paddock.c
Action berisikan sebagai berikut:
Gap [Jarak dengan driver di depan (float)]: Jika Jarak antara Driver dengan Musuh di depan adalah < 3.5s maka return Gogogo, jika jarak > 3.5s dan 10s return Push, dan jika jarak > 10s maka return Stay out of trouble.
Fuel [Sisa Bensin% (string/int/float)]: Jika bensin lebih dari 80% maka return Push Push Push, jika bensin di antara 50% dan 80% maka return You can go, dan jika bensin kurang dari 50% return Conserve Fuel.
Tire [Sisa Ban (int)]: Jika pemakaian ban lebih dari 80 maka return Go Push Go Push, jika pemakaian ban diantara 50 dan 80 return Good Tire Wear, jika pemakaian di antara 30 dan 50 return Conserve Your Tire, dan jika pemakaian ban kurang dari 30 maka return Box Box Box.
Tire Change [Tipe ban saat ini (string)]: Jika tipe ban adalah Soft return Mediums Ready, dan jika tipe ban Medium return Box for Softs.

Contoh:
[Driver] : [Fuel] [55%]
[Paddock]: [You can go]

Pada paddock.c program berjalan secara daemon di background, bisa terhubung dengan driver.c melalui socket RPC.
Program paddock.c dapat call function yang berada di dalam actions.c.
Program paddock.c tidak keluar/terminate saat terjadi error dan akan log semua percakapan antara paddock.c dan driver.c di dalam file race.log

Format log:
[Source] [DD/MM/YY hh:mm:ss]: [Command] [Additional-info]
ex :
[Driver] [07/04/2024 08:34:50]: [Fuel] [55%]
[Paddock] [07/04/2024 08:34:51]: [Fuel] [You can go]

Program driver.c bisa terhubung dengan paddock.c dan bisa mengirimkan pesan dan menerima pesan serta menampilan pesan tersebut dari paddock.c sesuai dengan perintah atau function call yang diberikan.
Jika bisa digunakan antar device/os (non local) akan diberi nilai tambahan.
untuk mengaktifkan RPC call dari driver.c, bisa digunakan in-program CLI atau Argv (bebas) yang penting bisa send command seperti poin B dan menampilkan balasan dari paddock.c
		
ex:
Argv: 
./driver -c Fuel -i 55% 
in-program CLI:
Command: Fuel
Info: 55%

### Penyelesaian Soal 3

**actions.c**

	#include <stdio.h>
	#include <string.h>
	
	char* gap(float distance) {
	    if (distance < 3.5)
	        return "Gogogo";
	    else if (distance >= 3.5 && distance <= 10)
	        return "Push";
	    else
	        return "Stay out of trouble";
	}
	
	char* fuel(float fuel_percentage) {
	    if (fuel_percentage > 80)
	        return "Push Push Push";
	    else if (fuel_percentage >= 50 && fuel_percentage <= 80)
	        return "You can go";
	    else
	        return "Conserve Fuel";
	}
	
	char* tire(int tire_usage) {
	    if (tire_usage > 80)
	        return "Go Push Go Push";
	    else if (tire_usage >= 50 && tire_usage <= 80)
	        return "Good Tire Wear";
	    else if (tire_usage >= 30 && tire_usage < 50)
	        return "Conserve Your Tire";
	    else
	        return "Box Box Box";
	}
	
	char* tire_change(char* current_tire) {
	    if (strcmp(current_tire, "Soft") == 0)
	        return "Mediums Ready";
	    else if (strcmp(current_tire, "Medium") == 0)
	        return "Box for Softs";
	    else
	        return "Invalid tire type";
	}

**paddock.c**

	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <time.h>
	#include <errno.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include "actions.c"
	
	#define PORT 8080
	#define LOG_FILE "race.log"
	
	void log_message(const char* source, const char* command, const char* info) {
	    FILE *log_file = fopen(LOG_FILE, "a");
	    if (log_file == NULL) {
	        perror("Error opening log file");
	        exit(EXIT_FAILURE);
	    }
	
	    time_t raw_time;
	    struct tm *time_info;
	    char time_str[20];
	
	    time(&raw_time);
	    time_info = localtime(&raw_time);
	    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", time_info);
	
	    fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, time_str, command, info);
	    fclose(log_file);
	}
	
	void handle_client(int client_socket) {
	    char buffer[1024] = {0};
	    int valread;
	
	    valread = read(client_socket, buffer, 1024);
	    if (valread == -1) {
	        fprintf(stderr, "Error reading from client: %s\n", strerror(errno));
	        close(client_socket);
	        return;
	    }
	
	    char* response = "Invalid command";
	    char* token = strtok(buffer, " ");
	    char* info = strtok(NULL, "\n"); 
	   if (token != NULL) {
	        if (strcmp(token, "Gap") == 0) {
	            float distance = atof(info);
	            response = gap(distance);
	        } else if (strcmp(token, "Fuel") == 0) {
	            float fuel_percentage = atof(info);
	            response = fuel(fuel_percentage);
	        } else if (strcmp(token, "Tire") == 0) {
	            int tire_usage = atoi(info);
	            response = tire(tire_usage);
	        } else if (strcmp(token, "TireChange") == 0) {
	            response = tire_change(info);
	        }
	    }
	
	    log_message("Driver", token, info); 
	
	    send(client_socket, response, strlen(response), 0);
	
	    log_message("Paddock",token, response);
	
	    close(client_socket);
	}
	
	int main() {
	    int server_fd, client_socket;
	    struct sockaddr_in address;
	    int opt = 1;
	    int addrlen = sizeof(address);
	
	    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
	        perror("socket failed");
	        exit(EXIT_FAILURE);
	    }
	
	    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
	        perror("setsockopt");
	        exit(EXIT_FAILURE);
	    }
	
	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = INADDR_ANY;
	    address.sin_port = htons(PORT);
	
	    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
	        perror("bind failed");
	        exit(EXIT_FAILURE);
	    }
	
	    if (listen(server_fd, 3) < 0) {
	        perror("listen");
	        exit(EXIT_FAILURE);
	    }
	
	    printf("Paddock is listening on port %d\n", PORT);
	
	    while (1) {
	        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
	            perror("accept");
	            continue;
	        }
	        pid_t child_pid = fork();
	        if (child_pid == 0) {
	            close(server_fd);
	            handle_client(client_socket);
	            close(client_socket);
	            exit(EXIT_SUCCESS);
	        } else if (child_pid < 0) {
	            perror("fork");
	        } else {
	            close(client_socket);
	        }
	    }
	
	    return 0;
	}

**driver.c**

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <string.h>
	
	#define PORT 8080
	
	void send_command(int sockfd, const char* command, const char* info) {
	    char buffer[1024] = {0};
	    sprintf(buffer, "%s %s", command, info);
	    send(sockfd, buffer, strlen(buffer), 0);
	    printf("[Driver]: [%s] [%s]\n", command, info);
	}
	
	void receive_response(int sockfd) {
	    char buffer[1024] = {0};
	    read(sockfd, buffer, 1024);
	    printf("[Paddock]: [%s]\n", buffer);
	}
	
	int main(int argc, char *argv[]) {
	    if (argc != 5) {
	        printf("Usage: %s -c [Command] -i [Info]\n", argv[0]);
	        return 1;
	    }
	
	    int client_socket;
	    struct sockaddr_in server_address;
	
	    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	        perror("socket creation failed");
	        return 1;
	    }
	
	    memset(&server_address, '0', sizeof(server_address));
	
	    server_address.sin_family = AF_INET;
	    server_address.sin_port = htons(PORT);
	
	    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
	        perror("invalid address / address not supported");
	        return 1;
	    }
	
	    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
	        perror("connection failed");
	        return 1;
	    }
	
	    char* command = argv[2];
	    char* info = argv[4];
	
	    send_command(client_socket, command, info);
	
	    receive_response(client_socket);
	
	    close(client_socket);
	
	    return 0;
	}

### Penjelasan actions.c

	#include <stdio.h>
	#include <string.h>
	
	char* gap(float distance) {
	    if (distance < 3.5)
	        return "Gogogo";
	    else if (distance >= 3.5 && distance <= 10)
	        return "Push";
	    else
	        return "Stay out of trouble";
	}
Fungsi ini memberikan respons kepada driver berdasarkan gap jaraknya. Jika jaraknya kurang dari 3.5, fungsi akan mengembalikan "Gogogo". Jika jaraknya antara 3.5 dan 10, fungsi akan mengembalikan "Push". Jika jaraknya lebih dari 10, fungsi akan mengembalikan "Stay out of trouble".
	
	char* fuel(float fuel_percentage) {
	    if (fuel_percentage > 80)
	        return "Push Push Push";
	    else if (fuel_percentage >= 50 && fuel_percentage <= 80)
	        return "You can go";
	    else
	        return "Conserve Fuel";
	}
Fungsi ini memberikan respons kepada driver berdasarkan persentase bahan bakar yang tersisa (fuel_percentage). Jika persentase bahan bakarnya lebih dari 80, fungsi akan mengembalikan "Push Push Push". Jika persentase bahan bakarnya antara 50 dan 80, fungsi akan mengembalikan "You can go". Jika persentase bahan bakarnya kurang dari 50, fungsi akan mengembalikan "Conserve Fuel".
	
	char* tire(int tire_usage) {
	    if (tire_usage > 80)
	        return "Go Push Go Push";
	    else if (tire_usage >= 50 && tire_usage <= 80)
	        return "Good Tire Wear";
	    else if (tire_usage >= 30 && tire_usage < 50)
	        return "Conserve Your Tire";
	    else
	        return "Box Box Box";
	}
Fungsi ini memberikan respons kepada driver berdasarkan penggunaan ban (tire_usage). Jika penggunaan ban lebih dari 80, fungsi akan mengembalikan "Go Push Go Push". Jika penggunaan ban antara 50 dan 80, fungsi akan mengembalikan "Good Tire Wear". Jika penggunaan ban antara 30 dan 50, fungsi akan mengembalikan "Conserve Your Tire". Jika penggunaan ban kurang dari 30, fungsi akan mengembalikan "Box Box Box".

	char* tire_change(char* current_tire) {
	    if (strcmp(current_tire, "Soft") == 0)
	        return "Mediums Ready";
	    else if (strcmp(current_tire, "Medium") == 0)
	        return "Box for Softs";
	    else
	        return "Invalid tire type";
	}
Fungsi ini memberikan respons kepada driver berdasarkan jenis ban yang saat ini digunakan (current_tire). Jika driver saat ini menggunakan ban "Soft", fungsi akan mengembalikan "Mediums Ready". Jika driver saat ini menggunakan ban "Medium", fungsi akan mengembalikan "Box for Softs". Jika jenis ban tidak dikenali, fungsi akan mengembalikan "Invalid tire type".

### Penjelasan paddock.c (server)

	#include <unistd.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <time.h>
	#include <errno.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include "actions.c"
	
	#define PORT 8080
	#define LOG_FILE "race.log"

Bagian ini adalah library yang diperlukan dan meng-include file "actions.c", yang berisi definisi fungsi-fungsi seperti `gap()`, `fuel()`, `tire()`, dan `tire_change()`. Kemudian, ada dua definisi konstan: `PORT` (port yang akan digunakan oleh server) dan `LOG_FILE` (nama file log).

	void log_message(const char* source, const char* command, const char* info) {
	    FILE *log_file = fopen(LOG_FILE, "a");
	    if (log_file == NULL) {
	        perror("Error opening log file");
	        exit(EXIT_FAILURE);
	    }
Fungsi log_message menerima tiga argumen: source (sumber pesan), command (perintah), dan info (informasi tambahan). Pertama, fungsi membuka file log dengan menggunakan fungsi fopen(). Jika gagal membuka file, pesan kesalahan akan dicetak menggunakan perror() dan program akan keluar dengan status kegagalan (EXIT_FAILURE).

	    time_t raw_time;
	    struct tm *time_info;
	    char time_str[20];
	    time(&raw_time);
	    time_info = localtime(&raw_time);
	    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", time_info);
Selanjutnya, waktu saat ini diambil menggunakan fungsi time(), dan struktur tm untuk informasi waktu lokal diperoleh menggunakan localtime(). Waktu yang diperoleh diformat ke dalam string time_str menggunakan strftime(), yang akan menampilkan tanggal dan waktu dalam format yang ditentukan ("%d/%m/%Y %H:%M:%S").

	    fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, time_str, command, info);
	    fclose(log_file);
	}
Terakhir, pesan log ditulis ke dalam file log menggunakan fprintf(). Setelah penulisan selesai, file log ditutup dengan fclose().

	void handle_client(int client_socket) {
	    char buffer[1024] = {0};
	    int valread;
Fungsi handle_client menerima satu argumen, yaitu client_socket, yang merupakan soket yang terhubung dengan driver.c. Dalam fungsi ini, kita mendeklarasikan sebuah buffer karakter buffer yang akan digunakan untuk membaca pesan dari client. valread digunakan untuk menyimpan jumlah byte yang dibaca dari soket client.

	    valread = read(client_socket, buffer, 1024);
	    if (valread == -1) {
	        fprintf(stderr, "Error reading from client: %s\n", strerror(errno));
	        close(client_socket);
	        return;
	    }
Kode di atas membaca pesan dari client ke dalam buffer yang telah disediakan. Jika ada kesalahan dalam membaca pesan, pesan kesalahan akan dicetak di stderr menggunakan fprintf, dan koneksi dengan client akan ditutup.
	
	    char* response = "Invalid command";
	    char* token = strtok(buffer, " ");
	    char* info = strtok(NULL, "\n"); 
	   if (token != NULL) {
	        if (strcmp(token, "Gap") == 0) {
	            float distance = atof(info);
	            response = gap(distance);
	        } else if (strcmp(token, "Fuel") == 0) {
	            float fuel_percentage = atof(info);
	            response = fuel(fuel_percentage);
	        } else if (strcmp(token, "Tire") == 0) {
	            int tire_usage = atoi(info);
	            response = tire(tire_usage);
	        } else if (strcmp(token, "TireChange") == 0) {
	            response = tire_change(info);
	        }
	    }
Setelah pesan dibaca, pesan tersebut diproses. Pesan tersebut dipisahkan menjadi dua bagian menggunakan fungsi strtok(). Bagian pertama disimpan dalam variabel token, dan bagian kedua disimpan dalam variabel info. Kemudian, berdasarkan nilai token, fungsi yang sesuai dipanggil (misalnya, gap(), fuel(), tire(), atau tire_change()), dan responsnya disimpan dalam variabel response.
	
	    log_message("Driver", token, info); 
	
	    send(client_socket, response, strlen(response), 0);
	
	    log_message("Paddock", token, response);
	
	    close(client_socket);
	}
Setelah pemrosesan pesan, pesan log driver dicatat yang berisi token (gap, fuel, tire, tire_change) dan info yang diterima dari client. Setelah mendapatkan respons dari fungsi yang sesuai, respons tersebut dikirimkan kembali ke client melalui soket. Setelah mengirim respons ke client, pesan log kedua yaitu paddock dicatat. Pesan log paddock berisi token (gap, fuel, tire, tire_change) dan respons yang didapat dari actions.c.

	int main() {
	    int server_fd, client_socket;
	    struct sockaddr_in address;
	    int opt = 1;
	    int addrlen = sizeof(address);
Variabel yang akan digunakan dideklarasikan. Server_fd adalah file descriptor untuk socket server, client_socket adalah file descriptor untuk socket client yang terhubung, address adalah struktur untuk menyimpan alamat server, opt adalah variabel untuk mengatur opsi soket, dan addrlen adalah panjang dari struktur alamat.
	
	    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
	        perror("socket failed");
	        exit(EXIT_FAILURE);
	    }
Kode di atas membuat soket menggunakan fungsi socket(). Soket tersebut menggunakan protokol AF_INET dan tipe soket stream untuk koneksi TCP. Jika pembuatan soket gagal, pesan kesalahan akan dicetak menggunakan perror() dan program akan keluar dengan status kegagalan (EXIT_FAILURE).

	    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
	        perror("setsockopt");
	        exit(EXIT_FAILURE);
	    }
Selanjutnya, kode mengatur opsi soket menggunakan setsockopt(). Dalam hal ini, menggunakan opsi SO_REUSEADDR dan SO_REUSEPORT untuk memungkinkan penggunaan kembali alamat dan port yang sama setelah server ditutup. Jika pengaturan opsi gagal, pesan kesalahan akan dicetak dan program akan keluar dengan status kegagalan.

	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = INADDR_ANY;
	    address.sin_port = htons(PORT);
	    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
	        perror("bind failed");
	        exit(EXIT_FAILURE);
	    }
Alamat server ditentukan dalam struktur address. Ini menggunakan alamat IPv4, dengan IP server yang ditetapkan sebagai INADDR_ANY, yang berarti server akan menerima koneksi dari semua antarmuka jaringan yang tersedia. Port yang digunakan oleh server ditentukan sebagai PORT, yang kemungkinan telah didefinisikan sebelumnya. Soket server diikat ke alamat yang ditentukan menggunakan fungsi bind(). Jika proses ini gagal, pesan kesalahan akan dicetak dan program akan keluar dengan status kegagalan.
	
	    if (listen(server_fd, 3) < 0) {
	        perror("listen");
	        exit(EXIT_FAILURE);
	    }
	    printf("Paddock is listening on port %d\n", PORT);
Selanjutnya, server mendengarkan koneksi masuk menggunakan fungsi listen(). Argumen kedua adalah jumlah maksimum koneksi yang dapat ditangani oleh server secara bersamaan. Jika mendengarkan gagal, pesan kesalahan akan dicetak dan program akan keluar dengan status kegagalan. Setelah semua persiapan selesai, pesan ini akan dicetak ke terminal untuk memberi tahu bahwa server siap mendengarkan koneksi pada port yang ditentukan.
	
	    while (1) {
	        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
	            perror("accept");
	            continue;
	        }
	        pid_t child_pid = fork();
	        if (child_pid == 0) {
	            close(server_fd);
	            handle_client(client_socket);
	            close(client_socket);
	            exit(EXIT_SUCCESS);
	        } else if (child_pid < 0) {
	            perror("fork");
	        } else {
	            close(client_socket);
	        }
	    }
	
	    return 0;
	}
Server kemudian masuk ke dalam loop utama, yang bertugas untuk menerima dan mengelola koneksi dari client. Fungsi accept() digunakan untuk menerima koneksi masuk. Jika koneksi berhasil diterima, soket client baru disimpan dalam client_socket. Setelah koneksi diterima, proses fork digunakan untuk menangani koneksi tersebut dalam proses anak. Jika fork berhasil, proses anak akan menutup soket server, menangani koneksi menggunakan fungsi handle_client, menutup soket client, dan kemudian keluar dengan status keberhasilan. Proses induk akan menutup soket client dan kembali ke awal loop untuk menerima koneksi baru.

### Penjelasan driver.c (client)

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <string.h>
	
	#define PORT 8080
Bagian ini untuk mengimport library yang diperlukan. Selain itu, sebuah konstanta bernama PORT didefinisikan untuk menentukan nomor port yang akan digunakan.
	
	void send_command(int sockfd, const char* command, const char* info) {
	    char buffer[1024] = {0};
	    sprintf(buffer, "%s %s", command, info);
	    send(sockfd, buffer, strlen(buffer), 0);
	    printf("[Driver]: [%s] [%s]\n", command, info);
	}
Fungsi ini mengirimkan perintah dan informasi tambahan ke server melalui soket. Perintah dan informasi disatukan dalam sebuah buffer menggunakan sprintf, lalu buffer tersebut dikirim melalui soket menggunakan fungsi send. Selain itu, fungsi ini juga mencetak perintah dan informasi ke terminal menggunakan printf.
	
	void receive_response(int sockfd) {
	    char buffer[1024] = {0};
	    read(sockfd, buffer, 1024);
	    printf("[Paddock]: [%s]\n", buffer);
	}
Fungsi receive_response bertanggung jawab untuk menerima respons dari server. Respons diterima melalui soket menggunakan fungsi read() dan disimpan dalam buffer. Setelah itu, respons tersebut dicetak ke terminal menggunakan printf.
	
	int main(int argc, char *argv[]) {
	    if (argc != 5) {
	        printf("Usage: %s -c [Command] -i [Info]\n", argv[0]);
	        return 1;
	    }
Bagian ini untuk memeriksa jumlah argumen yang diberikan saat menjalankan program. Jika jumlahnya tidak sama dengan lima (termasuk nama program itu sendiri), program mencetak cara penggunaan yang benar dan keluar dengan kode kesalahan.
	
	    int client_socket;
	    struct sockaddr_in server_address;
	
	    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	        perror("socket creation failed");
	        return 1;
	    }
Soket client dan struktur alamat server dideklarasikan. Soket client kemudian dibuat menggunakan fungsi socket(). Jika pembuatan soket gagal, pesan kesalahan dicetak dan program keluar dengan kode kesalahan. 

	    memset(&server_address, '0', sizeof(server_address));
	
	    server_address.sin_family = AF_INET;
	    server_address.sin_port = htons(PORT);
	
	    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
	        perror("invalid address / address not supported");
	        return 1;
	    }
Kode tersebut menginisialisasi alamat server untuk koneksi client-server. Pertama, memori struktur server_address dikosongkan. Selanjutnya, jenis alamat ditetapkan sebagai IPv4 dan nomor port server diatur. Alamat IP server, dalam hal ini alamat loopback 127.0.0.1, dikonversi dari format string menjadi bentuk biner menggunakan inet_pton(). Jika konversi gagal, program mencetak pesan kesalahan dan keluar dengan kode kesalahan.

	    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
	        perror("connection failed");
	        return 1;
	    }
Kemudian, koneksi ke server dilakukan menggunakan fungsi connect(). Jika koneksi gagal, pesan kesalahan dicetak dan program keluar dengan kode kesalahan.
	
	    char* command = argv[2];
	    char* info = argv[4];
	
	    send_command(client_socket, command, info);
	
	    receive_response(client_socket);
	
	    close(client_socket);
	
	    return 0;
	} 
Setelah koneksi berhasil dibuat, argumen dari baris perintah (argv) yang diberikan oleh pengguna diambil. Perintah dan informasi dari argumen tersebut kemudian dikirimkan ke server menggunakan fungsi send_command. Setelah itu, respons dari server diterima menggunakan fungsi receive_response. Terakhir, soket client ditutup dan program keluar dengan kode berhasil.

### Hasil Pengerjaan Soal 3


## Soal 4
**oleh Wira Samudra Siregar (5027231041)**
### Deskripsi Soal 1
4. Lewis Hamilton 🏎 seorang wibu akut dan sering melewatkan beberapa episode yang karena sibuk menjadi asisten. Maka dari itu dia membuat list anime yang sedang ongoing (biar tidak lupa) dan yang completed (anime lama tapi pengen ditonton aja). Tapi setelah Lewis pikir-pikir malah kepikiran untuk membuat list anime. Jadi dia membuat file (harap diunduh) dan ingin menggunakan socket yang baru saja dipelajarinya untuk melakukan CRUD pada list animenya. 

   a. Client dan server terhubung melalui socket. 

   b. client.c di dalam folder client dan server.c di dalam folder server

   c. Client berfungsi sebagai pengirim pesan dan dapat menerima pesan dari server.

   d. Server berfungsi sebagai penerima pesan dari client dan hanya menampilkan pesan perintah client saja.  

   e. Server digunakan untuk membaca myanimelist.csv. Dimana terjadi pengiriman data antara client ke server dan server ke client.

     * Menampilkan seluruh judul

     * Menampilkan judul berdasarkan genre

     * Menampilkan judul berdasarkan hari

     * Menampilkan status berdasarkan berdasarkan judul

     * Menambahkan anime ke dalam file myanimelist.csv

     * Melakukan edit anime berdasarkan judul

     * Melakukan delete berdasarkan judul

     * Selain command yang diberikan akan menampilkan tulisan “Invalid Command”

   f. Karena Lewis juga ingin track anime yang ditambah, diubah, dan dihapus. Maka dia membuat server dapat mencatat anime yang dihapus dalam sebuah log yang diberi nama change.log.

     * Format: [date] [type] [massage]

     * Type: ADD, EDIT, DEL

     * Ex:

       1. [29/03/24] [ADD] Kanokari ditambahkan.

       2. [29/03/24] [EDIT] Kamis,Comedy,Kanokari,completed diubah menjadi Jumat,Action,Naruto,completed.

       3. [29/03/24] [DEL] Naruto berhasil dihapus.

   g. Koneksi antara client dan server tidak akan terputus jika ada kesalahan input dari client, cuma terputus jika user mengirim pesan “exit”. Program exit dilakukan pada sisi client.

   h. Hasil akhir:

     soal_4/

       ├── change.log

       ├── client/

       │   └── client.c

       ├── myanimelist.csv

       └── server/
 
           └── server.c


### Penyelesaian Soal 1
### Client.c
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    #define PORT 8080

    int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, 1024);
        printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
    }

### Server.c
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define PORT 8080
    void logChange(char *type, char *message) {
    FILE *fp = fopen("change.log", "a");
    if (fp == NULL) {
    printf("Failed to open change.log\n");
    return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%02d/%02d/%02d] [%s] %s\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, type, message);
    fclose(fp);
    }

    int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
    }

    while (1) {
    memset(buffer, 0, sizeof(buffer));
    valread = read(new_socket, buffer, 1024);

    if (strcmp(buffer, "exit") == 0) {
        break;
    }

    printf("Received: %s\n", buffer);

    FILE *fp = fopen("myanimelist.csv", "r");
    if (fp == NULL) {
        perror("Failed to open myanimelist.csv");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    char response[1024] = "";

      if (strcmp(buffer, "showall") == 0) {
            while (fgets(line, sizeof(line), fp)) {
                char *title = strtok(line, ",");
                char *genre = strtok(NULL, ",");
                char *status = strtok(NULL, ",");
                sprintf(response + strlen(response), "%s, %s, %s\n", title, genre, status);
            }
        } else if (strncmp(buffer, "genre", 5) == 0) {
            char *genre = buffer + 6;
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, genre) != NULL) {
                    char *title = strtok(line, ",");
                    char *currGenre = strtok(NULL, ",");
                    char *status = strtok(NULL, ",");
                    sprintf(response + strlen(response), "%s, %s, %s\n", title, currGenre,status);
                }
            }
        } else if (strncmp(buffer, "day", 3) == 0) {
    char *day = buffer + 4;
    while (fgets(line, sizeof(line), fp)) {
        char *currDay = strtok(line, ",");
        char *title = strtok(NULL, ",");
        if (strcmp(currDay, day) == 0) {
            sprintf(response + strlen(response), "%s\n", title);
        }
    }
    } else if (strncmp(buffer, "status", 6) == 0) {
    char *title = buffer + 7;
    rewind(fp); // Move file pointer to the beginning of the file
    while (fgets(line, sizeof(line), fp)) {
        char *currTitle = strtok(line, ",");
        char *currGenre = strtok(NULL, ",");
        char *status = strtok(NULL, ",");
        if (strcmp(currTitle, title) == 0) {
            sprintf(response, "%s, %s\n", title, status);
            break;
        }
    }
    } else if (strncmp(buffer, "add", 3) == 0) {
        char *anime = buffer + 4;
        FILE *fw = fopen("myanimelist.csv", "a");
        fprintf(fw, "%s\n", anime);
        fclose(fw);
        strcat(response, "anime berhasil ditambahkan.");
        logChange("ADD", anime);
    } else if (strncmp(buffer, "edit", 4) == 0) {
        char *oldTitle = buffer + 5;
        char *newData = strchr(oldTitle, ',') + 1;
        *strchr(oldTitle, ',') = '\0';

        FILE *tempFile = fopen("temp.csv", "w");
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, oldTitle, strlen(oldTitle)) == 0) {
                fprintf(tempFile, "%s,%s", oldTitle, newData);
                logChange("EDIT", oldTitle);
                logChange("EDIT", newData);
            } else {
                fprintf(tempFile, "\n%s", line);
            }
        }
        fclose(fp);
        fclose(tempFile);
        remove("myanimelist.csv");
        rename("temp.csv", "myanimelist.csv");
        strcat(response, "anime berhasil diedit.");
    } else if (strncmp(buffer, "delete", 6) == 0) {
        char *title = buffer + 7;
        FILE *tempFile = fopen("temp.csv", "w");
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, title, strlen(title)) != 0) {
                fprintf(tempFile, "%s", line);
            } else {
                logChange("DEL", title);
            }
        }
        fclose(fp);
        fclose(tempFile);
        remove("myanimelist.csv");
        rename("temp.csv", "myanimelist.csv");
        strcat(response, "anime berhasil dihapus.");
    } else {
        strcat(response, "Invalid Command");
    }

    fclose(fp);
    send(new_socket, response, strlen(response), 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
    }

### Penjelasan
### Client.c
    #define PORT 8080

* Baris ini mendefinisikan PORT dengan nilai 8080, yang merupakan nomor port tempat server.

      int main(int argc, char const *argv[]) {
      int sock = 0, valread;
      struct sockaddr_in serv_addr;
      char buffer[1024] = {0};
  
* Awal dari fungsi `main()`. Fungsi ini mendeklarasikan tiga variabel: `sock` untuk menyimpan deskriptor file socket, `valread` untuk menyimpan jumlah byte yang dibaca dari socket, dan `buffer` untuk menyimpan data yang dikirim atau diterima. Struktur `sockaddr_in` `serv_addr` digunakan untuk menyimpan informasi alamat server.

      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("\n Socket creation error \n");
      return -1;
      }
  
* Membuat socket baru dengan tipe `SOCK_STREAM` (TCP) dalam keluarga alamat `AF_INET` (IPv4). Jika pembuatan socket gagal, ia akan mencetak pesan error dan keluar dari program.

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(PORT);
  
*  menetapkan alamat dari struktur alamat server `serv_addr` menjadi `AF_INET` (IPv4) dan nomor port menjadi `PORT` (8080) menggunakan fungsi `htons()`, yang mengonversi nomor port dari urutan byte host ke urutan byte jaringan.

       if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
       printf("\nInvalid address/ Address not supported \n");
       return -1;
       }
       if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\nConnection Failed \n");
       return -1;
       }

* Mengonversi string alamat IP "127.0.0.1" (localhost) menjadi bentuk biner menggunakan fungsi `inet_pton()` dan menyimpannya di bidang `sin_addr` dari struktur `serv_addr`. Jika konversi gagal, ia akan mencetak pesan error dan keluar dari program. Lalu Baris selanjutnya mencoba membuat koneksi dengan server menggunakan fungsi `connect()`. Ia melewatkan deskriptor file socket `sock`, pointer ke struktur `serv_addr` yang berisi informasi alamat server, dan ukuran struktur `serv_addr`. Jika koneksi gagal, ia akan mencetak pesan error dan keluar dari program.

      while (1) {
      memset(buffer, 0, sizeof(buffer));
      printf("You: ");
      fgets(buffer, sizeof(buffer), stdin);
      buffer[strcspn(buffer, "\n")] = 0;
      send(sock, buffer, strlen(buffer), 0);
      if (strcmp(buffer, "exit") == 0) {
          break;
      }
      memset(buffer, 0, sizeof(buffer));
      valread = read(sock, buffer, 1024);
      printf("Server: %s\n", buffer);
      }
      close(sock);
      return 0;
      }
  
* `memset(buffer, 0, sizeof(buffer))` menghapus array buffer dengan menetapkan semua elemennya ke nol. `printf("You: ")` meminta pengguna untuk memasukkan pesan. `fgets(buffer, sizeof(buffer), stdin)` membaca baris input dari pengguna dan menyimpannya di array buffer. `buffer[strcspn(buffer, "\n")] = 0` menghapus karakter newline (\n) dari akhir string input. `send(sock, buffer, strlen(buffer), 0)` mengirimkan pesan pengguna ke server melalui socket sock.`if (strcmp(buffer, "exit") == 0) { break; }` memeriksa apakah pengguna memasukkan "exit" dan keluar dari loop jika benar. `memset(buffer, 0, sizeof(buffer))` menghapus array buffer lagi. `valread = read(sock, buffer, 1024)` membaca data dari socket server ke dalam array buffer, dan menyimpan jumlah byte yang dibaca di valread. `printf("Server: %s\n", buffer)` mencetak pesan yang diterima dari server.

### Server.c
    void logChange(char *type, char *message) {
    FILE *fp = fopen("change.log", "a");
    if (fp == NULL) {
        printf("Failed to open change.log\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%02d/%02d/%02d] [%s] %s\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, type, message);
    fclose(fp);
    }
    
* Fungsi `logChange` digunakan untuk mencatat perubahan yang dilakukan pada file `myanimelist.csv`. Fungsi ini membuka file `change.log` dalam mode `append` ("a"), yang berarti setiap pemanggilannya akan menambahkan data baru ke akhir file. Jika pembukaan file gagal, pesan error akan dicetak dan fungsi akan keluar.
Kemudian, fungsi mendapatkan waktu saat ini menggunakan fungsi `time(NULL)` dan `localtime(&now)` dan memformat tanggal dan waktu ke dalam bentuk yang dapat dibaca. Lalu, fungsi menulis baris baru ke file `change.log` dengan format "[Tanggal] [Tipe] Pesan" menggunakan `fprintf`.

      if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
      }

      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
      }

      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(PORT);

      if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("bind failed");
      exit(EXIT_FAILURE);
      }

      if (listen(server_fd, 3) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
      }

      if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
      }
  
      while (1) {
      memset(buffer, 0, sizeof(buffer));
      valread = read(new_socket, buffer, 1024);

      if (strcmp(buffer, "exit") == 0) {
        break;
      }

      printf("Received: %s\n", buffer);

      FILE *fp = fopen("myanimelist.csv", "r");
      if (fp == NULL) {
        perror("Failed to open myanimelist.csv");
        exit(EXIT_FAILURE);
      }

* Program ini merupakan implementasi server TCP/IP yang mengelola daftar anime pada file "myanimelist.csv". Pertama, ia membuat socket baru dengan fungsi `socket` dan mengatur opsi socket agar dapat digunakan kembali setelah ditutup dengan `setsockopt`. Kemudian, ia mengisi struktur `address` dengan informasi alamat dan port server menggunakan `AF_INET` untuk IPv4 dan `INADDR_ANY` untuk menerima koneksi dari semua antarmuka jaringan. Selanjutnya, ia mengikat socket dengan alamat dan port tersebut menggunakan `bind`, dan kemudian mulai mendengarkan koneksi masuk dengan `listen`. Terakhir, ia menerima koneksi dari klien menggunakan `accept` dan menyimpan deskriptor file socket baru yang terhubung dengan klien dalam variabel `new_socket`. Setiap fungsi dalam proses ini dilengkapi dengan pengecekan error yang akan mencetak pesan kesalahan dan mengakhiri program jika terjadi kegagalan. Dalam loop utama, program membaca data dari klien menggunakan `read()`, memeriksa apakah data tersebut berisi perintah "exit", lalu membuka file "myanimelist.csv". Selanjutnya, program memproses perintah yang diterima dari klien, seperti menampilkan daftar anime berdasarkan genre atau hari, melihat status anime, menambah anime baru, mengedit data anime, atau menghapus anime dari file. Semua perubahan pada file dicatat dalam "change.log". Jika terjadi kesalahan saat membuka file, program akan mencetak pesan error dan mengakhiri eksekusi.

      if (strcmp(buffer, "showall") == 0) {
            while (fgets(line, sizeof(line), fp)) {
                char *title = strtok(line, ",");
                char *genre = strtok(NULL, ",");
                char *status = strtok(NULL, ",");
                sprintf(response + strlen(response), "%s, %s, %s\n", title, genre, status);
            }
        }

* Akan dieksekusi jika perintah yang diterima dari klien adalah "showall". Di dalam blok ini, terdapat sebuah loop `while` yang membaca setiap baris dari file "myanimelist.csv" menggunakan `fgets`.

      else if (strncmp(buffer, "genre", 5) == 0) {
      char *genre = buffer + 6;
      while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, genre) != NULL) {
                    char *title = strtok(line, ",");
                    char *currGenre = strtok(NULL, ",");
                    char *status = strtok(NULL, ",");
                    sprintf(response + strlen(response), "%s, %s, %s\n", title, currGenre,status);
                }
            }
        }

*  akan dieksekusi jika perintah yang diterima dari klien dimulai dengan `genre`. Variabel genre dibuat dengan mengabaikan 6 karakter pertama dari `buffer` (misalnya, jika `buffer` adalah "genre:Comedy", maka `genre` akan berisi "Comedy"). 


       else if (strncmp(buffer, "day", 3) == 0) {
       char *day = buffer + 4;
       while (fgets(line, sizeof(line), fp)) {
        char *currDay = strtok(line, ",");
        char *title = strtok(NULL, ",");
        if (strcmp(currDay, day) == 0) {
            sprintf(response + strlen(response), "%s\n", title);
           }
         }
       }

* Akan dieksekusi jika perintah yang diterima dari klien dimulai dengan `day`. Variabel day dibuat dengan mengabaikan 4 karakter pertama dari `buffer` (misalnya, jika `buffer` adalah "day:Monday", maka `day` akan berisi "Rabu").
  
       else if (strncmp(buffer, "status", 6) == 0) {
       char *title = buffer + 7;
       rewind(fp); // Move file pointer to the beginning of the file
       while (fgets(line, sizeof(line), fp)) {
        char *currTitle = strtok(line, ",");
        char *currGenre = strtok(NULL, ",");
        char *status = strtok(NULL, ",");
        if (strcmp(currTitle, title) == 0) {
            sprintf(response, "%s, %s\n", title, status);
            break;
          }
        }
      }

* Akan dieksekusi jika perintah yang diterima dari klien dimulai dengan `status`. Variabel title dibuat dengan mengabaikan 7 karakter pertama dari `buffer` (misalnya, jika `buffer` adalah "status:Naruto", maka `title` akan berisi "Naruto").

      else if (strncmp(buffer, "add", 3) == 0) {
        char *anime = buffer + 4;
        FILE *fw = fopen("myanimelist.csv", "a");
        fprintf(fw, "%s\n", anime);
        fclose(fw);
        strcat(response, "anime berhasil ditambahkan.");
        logChange("ADD", anime);
      }

* Akan dieksekusi jika perintah yang diterima dari klien dimulai dengan `add`. Variabel anime dibuat dengan mengabaikan 4 karakter pertama dari `buffer` (misalnya, jika `buffer` adalah "add:Naruto,Comedy,Watching", maka `anime` akan berisi "Naruto,Comedy,Watching").

        else if (strncmp(buffer, "edit", 4) == 0) {
        char *oldTitle = buffer + 5;
        char *newData = strchr(oldTitle, ',') + 1;
        *strchr(oldTitle, ',') = '\0';

        FILE *tempFile = fopen("temp.csv", "w");
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, oldTitle, strlen(oldTitle)) == 0) {
                fprintf(tempFile, "%s,%s", oldTitle, newData);
                logChange("EDIT", oldTitle);
                logChange("EDIT", newData);
            } else {
                fprintf(tempFile, "\n%s", line);
            }
        }
        fclose(fp);
        fclose(tempFile);
        remove("myanimelist.csv");
        rename("temp.csv", "myanimelist.csv");
        strcat(response, "anime berhasil diedit.");
      }

* Akan dieksekusi jika perintah yang diterima dari klien dimulai dengan `edit`. Variabel oldTitle dibuat dengan mengabaikan 5 karakter pertama dari `buffer` (misalnya, jika `buffer` adalah "edit:Naruto,Comedy,Watching", maka `oldTitle` akan berisi "Naruto").

        else if (strncmp(buffer, "delete", 6) == 0) {
        char *title = buffer + 7;
        FILE *tempFile = fopen("temp.csv", "w");
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, title, strlen(title)) != 0) {
                fprintf(tempFile, "%s", line);
            } else {
                logChange("DEL", title);
            }
        }
        fclose(fp);
        fclose(tempFile);
        remove("myanimelist.csv");
        rename("temp.csv", "myanimelist.csv");
        strcat(response, "anime berhasil dihapus.");
        }

* Akan dieksekusi jika perintah yang diterima dari klien dimulai dengan `delete`. Variabel title dibuat dengan mengabaikan 7 karakter pertama dari `buffer` (misalnya, jika `buffer` adalah "delete:Naruto", maka `title` akan berisi "Naruto").

        else {
        strcat(response, "Invalid Command");
       }

* Jika perintah yang diterima dari klien tidak dikenali atau tidak sesuai dengan perintah yang didefinisikan sebelumnya, maka kode else akan dieksekusi. Di dalam kode ini akan menjalankan "Invalid Command".
