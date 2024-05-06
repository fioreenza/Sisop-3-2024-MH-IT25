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
