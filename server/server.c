#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "thread_module.h" // Inclusion du module de threads

#pragma comment(lib,"ws2_32.lib")

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WinSock initialization failed : %d\n", WSAGetLastError());
        return 1;
    }
    printf("WinSock successfully initialized!\n");
    
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Créer un socket TCP sur IPv4
    if (sockfd == INVALID_SOCKET) { // Si manque de ressources, winsock mal installé etc..
        printf("Socket creation failed : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    // Prépare l'adresse à laquelle le serveur va écouter
    struct sockaddr_in server;
    server.sin_family = AF_INET; // Protocole IPV4
    server.sin_addr.s_addr = INADDR_ANY; // Permet d'écouter sur toutes les interfaces (localhost, IP externe etc..)
    server.sin_port = htons(12345); // Port sur lequel on accepte les connections
    
    if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed : %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    
    if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed : %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    
    printf("Waiting for connections...\n");
    
    // Initialiser le module de threads
    init_thread_module();
    printf("Thread module initialized!\n");
    
    // Boucle principale pour accepter les connexions
    struct sockaddr_in client;
    int client_size = sizeof(client);
    SOCKET client_socket;
    
    while (1) {
        // Accepte une connexion entrante et récupère l'adresse du client
        client_socket = accept(sockfd, (struct sockaddr*)&client, &client_size);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed : %d\n", WSAGetLastError());
            continue; // Continue à accepter d'autres connexions
        }
        
        // Convertir l'adresse IP en format lisible
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("New connection from %s:%d\n", client_ip, ntohs(client.sin_port));
        
        // Ajouter le client à la gestion des threads
        int result = add_client(client_socket);
        if (result < 0) {
            if (result == -1) {
                printf("Maximum number of clients reached. Connection refused.\n");
                const char* msg = "Server is full. Please try again later.";
                send(client_socket, msg, (int)strlen(msg), 0);
            } else {
                printf("Failed to create thread for client.\n");
            }
            closesocket(client_socket);
        } else {
            printf("Client #%d added and thread started.\n", result);
        }
    }
    
    // Nettoyer le module de threads
    cleanup_thread_module();
    
    // Fermer le socket serveur
    closesocket(sockfd);
    WSACleanup();
    return 0;
}