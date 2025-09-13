#include <stdio.h>
#include <enet/enet.h>

#define MAX_CLIENTS 32

typedef struct {
    float x, y; // position du joueur
} Player;

int main() {
    if (enet_initialize() != 0) {
        printf("Failed to initialize ENet.\n");
        return 1;
    }

    ENetAddress address;
    ENetHost* server;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);
    if (!server) {
        printf("Failed to create server.\n");
        return 1;
    }

    printf("Server started on port %d\n", address.port);

    Player players[MAX_CLIENTS] = {0};

    ENetEvent event;
    while (1) {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch(event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected.\n");
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    // Recevoir position du joueur
                    Player* p = (Player*)event.packet->data;
                    int client_id = event.peer->incomingPeerID; // ID client simple
                    players[client_id] = *p;

                    // Ne pas réutiliser event.packet directement
                    ENetPacket* send_packet = enet_packet_create(event.packet->data,
                                                                event.packet->dataLength,
                                                                ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast(server, 0, send_packet);
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("Client disconnected.\n");
                    break;
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    return 0;
}
