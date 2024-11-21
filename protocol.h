#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SERVER_IP "127.0.0.1" // Default server IP
#define SERVER_PORT 8080      // Default server port
#define QLEN 5                // Max queue length for clients

#define MIN_PASSWORD_LENGTH 6
#define MAX_PASSWORD_LENGTH 32

// Function declarations for password generation
void generate_numeric(char *password, int length);
void generate_alpha(char *password, int length);
void generate_mixed(char *password, int length);
void generate_secure(char *password, int length);

#endif