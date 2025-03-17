#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 2000
#define BUFFER_SIZE 2
#define MAX_CLIENTS 2

pthread_mutex_t printMutex;

// Xử lý dữ liệu từ client
void *handleClient(void *arg) {
    int clientSocket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';

        // Dùng mutex để đồng bộ khi in ra màn hình
        pthread_mutex_lock(&printMutex);
        printf("%s ", buffer);
        fflush(stdout);
        pthread_mutex_unlock(&printMutex);
    }

    if (bytesRead == 0) {
        printf("Client %d disconnected\n", clientSocket);
    } else {
        perror("Receive error");
    }

    close(clientSocket);
    return NULL;
}

int main() {
    int serverFd, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    pthread_t threadId[MAX_CLIENTS];
    int clientCount = 0;

    // Khởi tạo mutex
    pthread_mutex_init(&printMutex, NULL);

    // Tạo socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ cho socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối từ client
    if (listen(serverFd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (clientCount < MAX_CLIENTS) {
        // Chấp nhận kết nối từ client
        clientSocket = accept(serverFd, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            perror("Accept failed");
            continue;
        }

        // Tạo thread mới cho client
        if (pthread_create(&threadId[clientCount], NULL, handleClient, &clientSocket) != 0) {
            perror("Thread creation failed");
            close(clientSocket);
            continue;
        }

        clientCount++;
    }

    // Chờ các thread kết thúc
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(threadId[i], NULL);
    }

    // Đóng socket và hủy mutex
    close(serverFd);
    pthread_mutex_destroy(&printMutex);

    return 0;
}
