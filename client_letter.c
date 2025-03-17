#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 2000

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char sendBuffer[2];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("socketCreationFailed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connectionFailed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        for (char c = 'a'; c <= 'z'; c++) {
            sendBuffer[0] = c;
            sendBuffer[1] = '\0';
            write(clientSocket, sendBuffer, 2);
            usleep(300000);  // Gửi mỗi ký tự sau 300ms
        }
    }

    close(clientSocket);
    return 0;
}
