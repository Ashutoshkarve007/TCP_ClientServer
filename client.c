// client.c: TCP client sending SELECT/EXECUTE/READ commands
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000
#define BUFSIZE 128

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char buffer[BUFSIZE];
    char response[BUFSIZE];
    int res;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create TCP socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("socket() failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set server address (localhost:5000)
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);  // IPv4 localhost

    // Connect to server
    if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("connect() failed: %ld\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Connected to server at 127.0.0.1:%d\n", PORT);

    printf("Enter commands (SELECT, EXECUTE, READ). Type 'exit' to quit.\n");
    while (1) {
        // Read a line from stdin
        printf("> ");
        fflush(stdout);
        if (fgets(buffer, BUFSIZE, stdin) == NULL) break;
        // Remove trailing newline characters
        buffer[strcspn(buffer, "\r\n")] = 0;
        if (strcmp(buffer, "exit") == 0) break;

        // Send command to server
        send(sock, buffer, (int)strlen(buffer), 0);

        // Receive and print response
        res = recv(sock, response, BUFSIZE-1, 0);
        if (res > 0) {
            response[res] = '\0';
            printf("%s", response);
        } else {
            printf("Disconnected from server.\n");
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
