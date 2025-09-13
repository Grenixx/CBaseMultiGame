#include <stdio.h>
#include <enet/enet.h>

#define MAX_CLIENTS 32

typedef struct {
    int id;     
    float x, y; 
    int active; // 1 = joueur connecté, 0 = vide
} PlayerPacket;

typedef struct {
    ENetPeer* peer;
    PlayerPacket player;
    int active;
} Client;

int main() {
    if (enet_initialize() != 0) {
        printf("Failed to initialize ENet.\n");
        return 1;
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    ENetHost* server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);
    if (!server) {
        printf("Failed to create server.\n");
        return 1;
    }

    printf("Server started on port %d\n", address.port);

    Client clients[MAX_CLIENTS] = {0};
    int next_id = 1; // commencer à 1 pour éviter conflit avec valeur par défaut

    ENetEvent event;

    while (1) {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {

                case ENET_EVENT_TYPE_CONNECT: {
                    printf("A new client connected.\n");
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (!clients[i].active) {
                            clients[i].peer = event.peer;
                            clients[i].active = 1;
                            clients[i].player.id = next_id++;
                            clients[i].player.x = 400;
                            clients[i].player.y = 300;
                            clients[i].player.active = 1;
                            break;
                        }
                    }
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE: {
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (clients[i].active && clients[i].peer == event.peer) {
                            PlayerPacket* p = (PlayerPacket*)event.packet->data;
                            clients[i].player.x = p->x;
                            clients[i].player.y = p->y;
                            break;
                        }
                    }

                    // Broadcast toutes les positions
                    PlayerPacket all_players[MAX_CLIENTS];
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        all_players[i] = clients[i].player;
                    }

                    ENetPacket* send_packet = enet_packet_create(
                        all_players,
                        sizeof(all_players),
                        ENET_PACKET_FLAG_RELIABLE
                    );
                    enet_host_broadcast(server, 0, send_packet);
                    enet_packet_destroy(event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("A client disconnected.\n");
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (clients[i].active && clients[i].peer == event.peer) {
                            clients[i].active = 0;
                            clients[i].peer = NULL;
                            clients[i].player.active = 0; // marquer comme inactif
                            break;
                        }
                    }
                    break;
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    return 0;
}
