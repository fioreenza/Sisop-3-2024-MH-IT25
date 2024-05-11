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
