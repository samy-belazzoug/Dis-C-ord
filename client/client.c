#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Pour strcspn

#pragma comment(lib,"ws2_32.lib")

int main() {
    // Initialisation de WSA
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Échec de l'initialisation de WinSock : %d\n", WSAGetLastError());
        return 1;
    }
    printf("WinSock successfully initialized!\n");
    
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Erreur socket : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    // Infos du serveur (IP,port,etc...)
    struct sockaddr_in server;
    server.sin_family = AF_INET; // Utilise le protocole IPV4
    server.sin_port = htons(12345); // Port du réseau (12345), htons() convertit le nombre en format réseau (big endian)
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Définit l'adresse IP du serveur (localhost)
    
    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Erreur de connexion : %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    
    printf("Connected to server!\n");
    
    char buffer[512] = {0};
    
    // Attendre et afficher le message de bienvenue du serveur
    ZeroMemory(buffer, sizeof(buffer));
    int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        printf("Server: %s\n", buffer);
    }
    
    // Entrer le nom d'utilisateur
    printf("Enter your username: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Supprimer le saut de ligne
    send(sockfd, buffer, strlen(buffer), 0);
    
    // Le serveur va envoyer l'historique des messages
    // Attendre et afficher les messages d'historique
    while (1) {
        ZeroMemory(buffer, sizeof(buffer));
        bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            printf("Server disconnected or error.\n");
            break;
        }
        
        printf("%s\n", buffer);
        
        // Si c'est la fin de l'historique, sortir de cette boucle
        if (strstr(buffer, "--- Fin de l'historique des messages ---") != NULL) {
            break;
        }
    }
    
    // Boucle principale de chat
    while (1) {
        // Mode réception asynchrone
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sockfd, &readSet);
        FD_SET(0, &readSet); // stdin
        
        // Configurer un timeout pour select
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100 ms
        
        // Attendre qu'il y ait des données à lire
        int activity = select(sockfd + 1, &readSet, NULL, NULL, &timeout);
        
        if (activity > 0) {
            // Si des données sont disponibles sur le socket
            if (FD_ISSET(sockfd, &readSet)) {
                ZeroMemory(buffer, sizeof(buffer));
                bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) {
                    printf("Server disconnected or error.\n");
                    break;
                }
                printf("%s\n", buffer);
            }
            
            // Si l'utilisateur a tapé quelque chose
            if (FD_ISSET(0, &readSet)) {
                ZeroMemory(buffer, sizeof(buffer));
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    buffer[strcspn(buffer, "\n")] = 0; // Supprimer le saut de ligne
                    
                    if (strcmp(buffer, "exit") == 0) {
                        printf("Closing connection...\n");
                        break;
                    }
                    
                    send(sockfd, buffer, strlen(buffer), 0);
                }
            }
        }
    }
    
    closesocket(sockfd);
    WSACleanup();
    return 0;
}