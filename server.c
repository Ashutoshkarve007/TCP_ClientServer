// server.c: TCP server handling SELECT, EXECUTE, READ commands
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")  // Link with the Winsock library

#define PORT 5000
#define BUFSIZE 128

int main() {
    WSADATA wsa;
    SOCKET listenSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    int clientLen = sizeof(clientAddr);
    char buffer[BUFSIZE];
    int res;

    printf("Starting server on port %d...\n", PORT);
    // Initialize Winsock (version 2.2)
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create a TCP listening socket
    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        printf("socket() failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind to any local IP address on PORT
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    if (bind(listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("bind() failed: %ld\n", WSAGetLastError());
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(listenSock, 1) == SOCKET_ERROR) {
        printf("listen() failed: %ld\n", WSAGetLastError());
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept one client
    clientSock = accept(listenSock, (SOCKADDR*)&clientAddr, &clientLen);
    if (clientSock == INVALID_SOCKET) {
        printf("accept() failed: %ld\n", WSAGetLastError());
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    printf("Client connected.\n");

    // Variables to hold the selected solenoid and valve position
    char selectedSolenoid[16] = "";
    int duration = 0;
    char valvePosition[16] = "CLOSE";  // Valve starts CLOSED

    // Communication loop
    while (1) {
        memset(buffer, 0, BUFSIZE);
        res = recv(clientSock, buffer, BUFSIZE-1, 0);
        if (res <= 0) {
            printf("Connection closed or recv error (%d)\n", WSAGetLastError());
            break;
        }
        buffer[res] = '\0';
        printf("Received: %s\n", buffer);

        // Handle commands case-insensitively
        if (_strnicmp(buffer, "SELECT", 6) == 0) {
            // Expected format: SELECT <SOLENOID> <DURATION>
            char cmd[8], sol[16];
            int dur;
            if (sscanf(buffer, "%s %s %d", cmd, sol, &dur) == 3) {
                strcpy(selectedSolenoid, sol);
                duration = dur;
                const char *resp = "Solenoid selected for execution.\n";
                send(clientSock, resp, (int)strlen(resp), 0);
            } else {
                const char *resp = "Invalid SELECT format (usage: SELECT <SOLENOID> <DURATION>).\n";
                send(clientSock, resp, (int)strlen(resp), 0);
            }
        }
        else if (_strnicmp(buffer, "EXECUTE", 7) == 0) {
            if (selectedSolenoid[0] != '\0') {
                // Simulate execution (could use Sleep(duration*1000) for real delay)
                printf("Executing %s solenoid for %d seconds...\n", selectedSolenoid, duration);
                // (In a real application, perform hardware action or a timed wait here.)
                // Update valve position based on which solenoid was selected
                if (_stricmp(selectedSolenoid, "OPEN") == 0) {
                    strcpy(valvePosition, "OPEN");
                } else {
                    strcpy(valvePosition, "CLOSE");
                }
                const char *resp = "Solenoid executed. Valve moved.\n";
                send(clientSock, resp, (int)strlen(resp), 0);
                // Clear selection
                selectedSolenoid[0] = '\0';
            } else {
                const char *resp = "Error: No solenoid selected. Use SELECT before EXECUTE.\n";
                send(clientSock, resp, (int)strlen(resp), 0);
            }
        }
        else if (_strnicmp(buffer, "READ", 4) == 0) {
            // Send current valve position
            char msg[64];
            sprintf(msg, "Valve position: %s\n", valvePosition);
            send(clientSock, msg, (int)strlen(msg), 0);
        }
        else {
            const char *resp = "Unknown command.\n";
            send(clientSock, resp, (int)strlen(resp), 0);
        }
    }

    // Cleanup
    closesocket(clientSock);
    closesocket(listenSock);
    WSACleanup();
    return 0;
}
