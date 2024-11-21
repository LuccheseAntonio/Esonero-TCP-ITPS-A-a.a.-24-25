#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h"

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);

    while (1) {
        // Read user input
        printf("Enter request (e.g., n 8) or 'q' to quit: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) break;

        if (buffer[0] == 'q') break;

        // Send request to server
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive response from server
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';
        printf("Generated password: %s\n", buffer);
    }

    close(client_socket);
    return 0;
}