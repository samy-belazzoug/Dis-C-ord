// thread_module.h
#ifndef THREAD_MODULE_H
#define THREAD_MODULE_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <process.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 512
#define MESSAGE_HISTORY_SIZE 100

// Structure pour stocker les messages
typedef struct {
    char username[32];
    char message[BUFFER_SIZE];
    char timestamp[26];
} ChatMessage;

// Structure pour stocker les informations d'un client
typedef struct {
    SOCKET socket;
    int id;
    char username[32];
} ClientInfo;

// Initialiser le module de thread
void init_thread_module();

// Nettoyer le module de thread
void cleanup_thread_module();

// Fonction pour gérer un client (à exécuter dans un thread)
unsigned __stdcall handle_client(void* arg);

// Ajouter un client à la gestion des threads
int add_client(SOCKET client_socket);

// Diffuser un message à tous les clients
void broadcast_message(const char* sender, const char* message, SOCKET exclude_socket);

// Envoyer l'historique des messages
void send_message_history(SOCKET client_socket);

#endif // THREAD_MODULE_H