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

