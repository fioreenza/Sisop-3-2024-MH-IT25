#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define SHM_SIZE 1024
#define FILENAME_SIZE 256

int main()
{
    key_t key = 123456;
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    char *shm = (char *)shmat(shmid, NULL, 0);
    char *s;
    // Mendapatkan daftar file dari direktori new-data
    DIR *dir;

    struct dirent *entry;

    if ((dir = opendir("/home/ludwigd/SisOP/Praktikum3/nomor1/new-data")) == NULL)
    {
        perror("opendir");
        exit(1);
    }
    // Menyimpan nama file ke dalam shared memory
    s = shm;

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip directories and special files
        if (entry->d_type == DT_REG)
 	{
            strncpy(s, entry->d_name, FILENAME_SIZE);
            s += FILENAME_SIZE;
        }
    }
    closedir(dir);
    shmdt(shm);
//    shmctl(shmid, IPC_RMID, NULL);

    // Menunggu beberapa saat untuk memastikan proses kedua membaca shared memory setelah proses pertama menulis
    sleep(10);

    // Membuka kembali shared memory untuk proses kedua
    if ((shmid = shmget(key, SHM_SIZE, 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    // Menautkan shared memory ke ruang alamat untuk proses kedua
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Membuat direktori database jika belum ada
    char database_dir[] = "/home/ludwigd/SisOP/Praktikum3/nomor1/microservices/database";
    struct stat st = {0};

    if (stat(database_dir, &st) == -1)
    {
        mkdir(database_dir, 0777); // Izin 0777 untuk membuat direktori dengan izin penuh
    }

    // Membuka file log
    FILE *log_file = fopen("/home/ludwigd/SisOP/Praktikum3/nomor1/microservices/database/db.log", "a");
    if (log_file == NULL)
    {
        perror("fopen");
        exit(1);
    }
    // Membaca daftar file dari shared memory dan memindahkan file
    s = shm;

    while (*s != '\0')
    {
        // Memindahkan file dari new-data ke microservices/database
        char filename[FILENAME_SIZE];
        strncpy(filename, s, FILENAME_SIZE);
        filename[FILENAME_SIZE - 1] = '\0'; // Pastikan string terminasi

        // Konstruksi path lengkap untuk file
        char *source_path = malloc(strlen("/home/ludwigd/SisOP/Praktikum3/nomor1/new-data/") + strlen(filename) + 1);

	// Periksa apakah alokasi berhasil
	if (source_path == NULL)
 	{
	    perror("malloc");
	    exit(1);
	}
        // Konstruksi path lengkap untuk file
	sprintf(source_path, "/home/ludwigd/SisOP/Praktikum3/nomor1/new-data/%s", filename);

	// Konstruksi path lengkap untuk file tujuan
        char *dest_path = malloc(strlen("/home/ludwigd/SisOP/Praktikum3/nomor1/microservices/database/") + strlen(filename) + 1);

	if (dest_path == NULL)
 	{
	    perror("malloc");
	    exit(1);
	}
	// Konstruksi path lengkap untuk file tujuan
	sprintf(dest_path, "/home/ludwigd/SisOP/Praktikum3/nomor1/microservices/database/%s", filename);

        // Memindahkan file
        if (rename(source_path, dest_path) == -1)
	{
            perror("rename");
	    free(source_path); // Hindari kebocoran memori jika gagal memindahkan file
	    free(dest_path);
            // Gagal memindahkan file, lanjut ke file berikutnya
            s += FILENAME_SIZE;
            continue;
        }

	// Menentukan jenis file berdasarkan nama
        char *type;

        if (strstr(filename, "trashcan") != NULL)
 	{
            type = "Trash Can";
        }

 	else if (strstr(filename, "parkinglot") != NULL)
	{
            type = "Parking Lot";
        }
 	else
 	{
            type = "Unknown";
        }

	// Menulis entri log ke file
        time_t current_time;
        struct tm *local_time;
        char time_str[20];
        time(&current_time);
        local_time = localtime(&current_time);
        strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", local_time);
        fprintf(log_file, "[%s] [%s] [%s]\n", time_str, type, filename);
	// Melepaskan memori yang dialokasikan
        free(source_path);
	free(dest_path);
        // Memajukan pointer untuk membaca file berikutnya
        s += FILENAME_SIZE;
    }
    // Menutup file log
    fclose(log_file);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return EXIT_SUCCESS;

}
