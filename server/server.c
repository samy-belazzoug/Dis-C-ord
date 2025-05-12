#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#pragma comment(lib,"ws2_32.lib")

int main() {
    
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WinSock initialization failed : %d\n", WSAGetLastError());
        return 1;
    }
    printf("WinSock successfully initialized!\n");

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Créer un socket TCP sur IPv4
    if (sockfd == INVALID_SOCKET) { //Si manque de ressources, winsock mal installé etc..
        printf("Socket creation failed : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //Prépare l'adresse à laquelle le serveur va écouter
    struct sockaddr_in server;
    server.sin_family = AF_INET; //Protocole IPV4
    server.sin_addr.s_addr = INADDR_ANY; //Permet d'écouter sur toutes les interfaces (localhost, IP externe etc..)
    server.sin_port = htons(12345); //Port sur lequel on accepte les connections

    if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed : %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    listen(sockfd, SOMAXCONN);

    printf("Waiting for connexion...\n");
    
    //Accepte une connexion entrante et récupère l'adresse du client
    struct sockaddr_in client;
    int client_size = sizeof(client); //Indique la taille de la struct (obligatoire pour accept())
    SOCKET client_socket = accept(sockfd, (struct sockaddr*)&client,
    &client_size); //Accepte une connexion et remplit la struct client avec les infos du client (IP + port).

    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed : %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Client connected!");

    char buffer[512] = {0};

    
    while (1) {
    ZeroMemory(buffer, sizeof(buffer)); // remet le buffer à zéro à chaque tour

    int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        printf("Client disconnected or error.\n");
        break; // quitte la boucle si erreur ou déconnexion
    }
    
    printf("Client : %s\n", buffer);

    const char *response = "Message received !";
    send(client_socket, response, strlen(response), 0);
    }
    
    closesocket(client_socket);
    closesocket(sockfd);
    WSACleanup();
    return 0;
}