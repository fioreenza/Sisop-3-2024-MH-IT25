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
## Soal 2
## Soal 3
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
