#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "protocol.h"

// Password generation functions
void generate_numeric(char *password, int length) {
    for (int i = 0; i < length; i++) {
        password[i] = '0' + rand() % 10;
    }
    password[length] = '\0';
}

void generate_alpha(char *password, int length) {
    for (int i = 0; i < length; i++) {
        password[i] = 'a' + rand() % 26;
    }
    password[length] = '\0';
}

void generate_mixed(char *password, int length) {
    for (int i = 0; i < length; i++) {
        if (rand() % 2)
            password[i] = 'a' + rand() % 26;
        else
            password[i] = '0' + rand() % 10;
    }
    password[length] = '\0';
}

void generate_secure(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    int charset_size = strlen(charset);
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % charset_size];
    }
    password[length] = '\0';
}

int main() {
    srand(time(NULL));
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024], password[MAX_PASSWORD_LENGTH + 1];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, QLEN) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%d\n", SERVER_IP, SERVER_PORT);

    while (1) {
        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while (1) {
            // Read client request
            int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) break;

            buffer[bytes_received] = '\0';

            // Parse request
            char type;
            int length;
            if (sscanf(buffer, "%c %d", &type, &length) != 2 || length < MIN_PASSWORD_LENGTH || length > MAX_PASSWORD_LENGTH) {
                const char *error_msg = "Invalid request\n";
                send(client_socket, error_msg, strlen(error_msg), 0);
                continue;
            }

            // Generate password
            switch (type) {
                case 'n': generate_numeric(password, length); break;
                case 'a': generate_alpha(password, length); break;
                case 'm': generate_mixed(password, length); break;
                case 's': generate_secure(password, length); break;
                default:
                    send(client_socket, "Invalid type\n", 13, 0);
                    continue;
            }

            // Send password to client
            send(client_socket, password, strlen(password), 0);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}