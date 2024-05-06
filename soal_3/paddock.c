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
        char* space = strchr(buffer, ' ');
        if (space != NULL) {
            response = fuel(atof(space + 1));
        } else {
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


