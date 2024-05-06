#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024

struct Entry {
    char filename[50];
    char type[50];
    char name[50];
    double rating;
};

int main()
{
    key_t key = 12345;
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    char *shm = (char *)shmat(shmid, NULL, 0);
    char *s;
    struct Entry highest_rating = {"", "", "", 0.0}; // Inisialisasi dengan nilai default

    // Membaca isi shared memory dan mencari string
    s = shm;
    while (*s != '\0') {
        // Mencari string yang mengandung ".csv"
        char *newline_pos = strchr(s, '\n');

        if (newline_pos != NULL)
 	{
            // Menyimpan nama file
            *newline_pos = '\0'; // Mengakhiri string di sini

            // Memeriksa apakah baris saat ini adalah nama file
            if (strstr(s, ".csv") != NULL)
 	    {
                strcpy(highest_rating.filename, s);

                // Menentukan jenis berdasarkan nama file
                if (strstr(s, "trashcan") != NULL)
 		{
                    strcpy(highest_rating.type, "Trash Can");
                }
 		else if (strstr(s, "parkinglot") != NULL)
		{
                    strcpy(highest_rating.type, "Parking Lot");
                }
            }
 	    else
	    {
                // Memproses data baris dengan nama dan rating
                char name[50];
                double rating;
                sscanf(s, "%[^,], %lf", name, &rating);

                if (rating > highest_rating.rating)
 		{
                    strcpy(highest_rating.name, name);
                    highest_rating.rating = rating;
                }
            }

            // Lanjutkan mencari setelah ".csv"
            s = newline_pos + 1;
        }
 	else
 	{
            // Jika tidak ditemukan, lanjutkan ke karakter berikutnya
            s++;
        }
    }
    // Menampilkan hasil
    printf("Type: %s\n", highest_rating.type);
    printf("Filename: %s\n", highest_rating.filename);
    printf("-------------------\n");
    printf("Name: %s\n", highest_rating.name);
    printf("Rating: %.1lf\n", highest_rating.rating);

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
