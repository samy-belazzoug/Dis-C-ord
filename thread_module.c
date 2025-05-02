// test of Synchro
// thread_module.c
#include "thread_module.h"

// Variables globales
static ClientInfo clients[MAX_CLIENTS];
static int client_count = 0;
static CRITICAL_SECTION client_cs; // Section critique pour la liste des clients

// Historique des messages
static ChatMessage message_history[MESSAGE_HISTORY_SIZE];
static int message_count = 0;
static CRITICAL_SECTION history_cs; // Section critique pour l'historique

// Initialiser le module de thread
void init_thread_module() {
    InitializeCriticalSection(&client_cs);
    InitializeCriticalSection(&history_cs);
    client_count = 0;
    message_count = 0;
    
    printf("Module de thread initialisé.\n");
}

// Nettoyer le module de thread
void cleanup_thread_module() {
    // Fermer tous les sockets clients
    EnterCriticalSection(&client_cs);
    
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket != INVALID_SOCKET) {
            closesocket(clients[i].socket);
            clients[i].socket = INVALID_SOCKET;
        }
    }
    
    client_count = 0;
    LeaveCriticalSection(&client_cs);
    
    // Détruire les sections critiques
    DeleteCriticalSection(&client_cs);
    DeleteCriticalSection(&history_cs);
    
    printf("Module de thread nettoyé.\n");
}

// Obtenir l'horodatage actuel
static void get_timestamp(char* timestamp) {
    time_t now = time(NULL);
    struct tm time_info;
    localtime_s(&time_info, &now);
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", &time_info);
}

// Ajouter un message à l'historique
static void add_message_to_history(const char* username, const char* message) {
    EnterCriticalSection(&history_cs);
    
    int index = message_count % MESSAGE_HISTORY_SIZE;
    strncpy_s(message_history[index].username, sizeof(message_history[index].username), username, _TRUNCATE);
    strncpy_s(message_history[index].message, sizeof(message_history[index].message), message, _TRUNCATE);
    get_timestamp(message_history[index].timestamp);
    
    if (message_count < MESSAGE_HISTORY_SIZE) {
        message_count++;
    }
    
    LeaveCriticalSection(&history_cs);
}

// Diffuser un message à tous les clients
void broadcast_message(const char* sender, const char* message, SOCKET exclude_socket) {
    char formatted_message[BUFFER_SIZE + 64];
    char timestamp[26];
    get_timestamp(timestamp);
    
    // Format: [timestamp] username: message
    sprintf_s(formatted_message, sizeof(formatted_message), "[%s] %s: %s", timestamp, sender, message);
    
    // Ajouter le message à l'historique
    add_message_to_history(sender, message);
    
    EnterCriticalSection(&client_cs);
    
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket != exclude_socket && clients[i].socket != INVALID_SOCKET) {
            if (send(clients[i].socket, formatted_message, (int)strlen(formatted_message), 0) == SOCKET_ERROR) {
                printf("Erreur d'envoi au client %d: %d\n", i, WSAGetLastError());
                closesocket(clients[i].socket);
                clients[i].socket = INVALID_SOCKET;
            }
        }
    }
    
    LeaveCriticalSection(&client_cs);
}

// Envoyer l'historique des messages à un client
void send_message_history(SOCKET client_socket) {
    EnterCriticalSection(&history_cs);
    
    int count = message_count < MESSAGE_HISTORY_SIZE ? message_count : MESSAGE_HISTORY_SIZE;
    int start = message_count < MESSAGE_HISTORY_SIZE ? 0 : message_count % MESSAGE_HISTORY_SIZE;
    
    char buffer[BUFFER_SIZE + 64];
    for (int i = 0; i < count; i++) {
        int index = (start + i) % MESSAGE_HISTORY_SIZE;
        sprintf_s(buffer, sizeof(buffer), "[%s] %s: %s", 
                 message_history[index].timestamp,
                 message_history[index].username, 
                 message_history[index].message);
        
        send(client_socket, buffer, (int)strlen(buffer), 0);
        Sleep(50); // Petit délai pour éviter de submerger le client
    }
    
    LeaveCriticalSection(&history_cs);
}

// Ajouter un client à la gestion des threads
int add_client(SOCKET client_socket) {
    EnterCriticalSection(&client_cs);
    
    if (client_count >= MAX_CLIENTS) {
        LeaveCriticalSection(&client_cs);
        return -1; // Nombre maximum de clients atteint
    }
    
    // Ajouter le client à la liste
    clients[client_count].socket = client_socket;
    clients[client_count].id = client_count;
    strcpy_s(clients[client_count].username, sizeof(clients[client_count].username), "Anonymous");
    
    int new_client_id = client_count;
    client_count++;
    
    LeaveCriticalSection(&client_cs);
    
    // Créer un thread pour gérer ce client
    HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, handle_client, &clients[new_client_id], 0, NULL);
    if (thread) {
        CloseHandle(thread); // Libérer les ressources du handle (le thread continue de s'exécuter)
        return new_client_id;
    } else {
        printf("Échec de la création du thread: %d\n", GetLastError());
        return -2; // Échec de la création du thread
    }
}

// Fonction pour gérer un client (exécutée dans un thread séparé)
unsigned __stdcall handle_client(void* arg) {
    ClientInfo* client = (ClientInfo*)arg;
    SOCKET client_socket = client->socket;
    int client_id = client->id;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    // Envoyer un message de bienvenue
    const char* welcome_msg = "Bienvenue sur MyDiscord! Veuillez entrer votre nom d'utilisateur:";
    send(client_socket, welcome_msg, (int)strlen(welcome_msg), 0);
    
    // Recevoir le nom d'utilisateur
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        strncpy_s(client->username, sizeof(client->username), buffer, _TRUNCATE);
        
        // Annoncer l'arrivée d'un nouvel utilisateur
        char announce[BUFFER_SIZE];
        sprintf_s(announce, sizeof(announce), "*** %s a rejoint le chat ***", client->username);
        printf("%s\n", announce);
        broadcast_message("Serveur", announce, INVALID_SOCKET);
        
        // Envoyer l'historique des messages
        const char* history_msg = "--- Début de l'historique des messages ---";
        send(client_socket, history_msg, (int)strlen(history_msg), 0);
        send_message_history(client_socket);
        const char* end_history_msg = "--- Fin de l'historique des messages ---";
        send(client_socket, end_history_msg, (int)strlen(end_history_msg), 0);
    }
    
    // Boucle principale de réception des messages
    while (1) {
        ZeroMemory(buffer, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            // Client déconnecté ou erreur
            break;
        }
        
        buffer[bytes_received] = '\0';
        
        // Traiter le message reçu
        printf("Message de %s: %s\n", client->username, buffer);
        broadcast_message(client->username, buffer, INVALID_SOCKET);
    }
    
    // Client déconnecté
    EnterCriticalSection(&client_cs);
    
    // Marquer le socket comme invalide
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == client_socket) {
            printf("Client %s déconnecté\n", clients[i].username);
            
            // Annoncer le départ
            char announce[BUFFER_SIZE];
            sprintf_s(announce, sizeof(announce), "*** %s a quitté le chat ***", clients[i].username);
            broadcast_message("Serveur", announce, INVALID_SOCKET);
            
            closesocket(clients[i].socket);
            clients[i].socket = INVALID_SOCKET;
            
            // Réorganiser le tableau si nécessaire
            if (i < client_count - 1) {
                clients[i] = clients[client_count - 1];
            }
            client_count--;
            break;
        }
    }
    
    LeaveCriticalSection(&client_cs);
    return 0;
}