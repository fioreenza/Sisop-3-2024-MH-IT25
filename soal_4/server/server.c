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
