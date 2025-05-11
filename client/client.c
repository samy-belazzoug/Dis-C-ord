#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib,"ws2_32.lib")

int main() {
    
    //Initialisation de WSA
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

    //Infos du serveur (IP,port,etc...)

    struct sockaddr_in server;
    server.sin_family = AF_INET; //Utilise le protocole IPV4
    server.sin_port = htons(12345); //Port du réseau (12345), htons() convertit le nombre en format réseau (big endian)
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); //Définit l'adresse IP du serveur (localhost), inet_addr() convertit une chaine de caractères IP en entier utilisable

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Erreur de connexion : %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    char buffer[512] = {0};


    //CONNEXION/CREATION COMPTE
    


    while (1) {
    printf("You : ");
    fgets(buffer, sizeof(buffer), stdin); // lire entrée utilisateur

    // Supprimer le '\n' ajouté par fgets
    buffer[strcspn(buffer, "\n")] = 0;

    if (strcmp(buffer, "exit") == 0) {
        printf("Closing connection...\n");
        break;
    }

    send(sockfd, buffer, strlen(buffer), 0);

    ZeroMemory(buffer, sizeof(buffer)); // vide le buffer pour recevoir

    int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        printf("Server disconnected or error.\n");
        break;
    }
    
    printf("Server : %s\n", buffer);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}