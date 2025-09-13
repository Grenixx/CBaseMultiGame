#include <stdio.h>
#include <enet/enet.h>
#include "raylib.h"

#define MAX_PLAYERS 32

typedef struct {
    int id;
    float x, y;
    int active; // 1 = connecté, 0 = vide
} PlayerPacket;

int main() {
    if (enet_initialize() != 0) {
        printf("Failed to initialize ENet.\n");
        return 1;
    }

    ENetAddress address;
    ENetHost* client;
    ENetPeer* peer;

    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (!client) return 1;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        printf("No available peers.\n");
        return 1;
    }

    InitWindow(800, 600, "Raylib + ENet Multi Demo");
    SetTargetFPS(60);

    PlayerPacket me = {0, 400, 300, 1};
    PlayerPacket players[MAX_PLAYERS] = {0};

    ENetEvent event;

    while (!WindowShouldClose()) {
        // Input
        if (IsKeyDown(KEY_RIGHT)) me.x += 5;
        if (IsKeyDown(KEY_LEFT))  me.x -= 5;
        if (IsKeyDown(KEY_DOWN))  me.y += 5;
        if (IsKeyDown(KEY_UP))    me.y -= 5;

        // Envoyer sa position au serveur
        ENetPacket* packet = enet_packet_create(&me, sizeof(PlayerPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);

        // Recevoir positions de tous les joueurs
        while (enet_host_service(client, &event, 0) > 0) {
            if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                PlayerPacket* received = (PlayerPacket*)event.packet->data;
                for (int i = 0; i < MAX_PLAYERS; i++) {
                    players[i] = received[i];
                }
                enet_packet_destroy(event.packet);
            }
        }

        // Dessin
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (!players[i].active) continue; // ignorer les slots vides

            if (players[i].id == me.id)
                DrawCircle((int)players[i].x, (int)players[i].y, 20, BLUE); // toi
            else
                DrawCircle((int)players[i].x, (int)players[i].y, 20, RED);  // autres joueurs
        }

        EndDrawing();
    }

    CloseWindow();
    enet_host_destroy(client);
    enet_deinitialize();
    return 0;
}
