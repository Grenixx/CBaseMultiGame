#include <stdio.h>
#include <enet/enet.h>
#include "raylib.h"

typedef struct {
    float x, y;
} Player;

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

    InitWindow(800, 600, "Raylib + ENet Demo");
    SetTargetFPS(60);

    Player me = {400, 300};
    Player players[32] = {0};

    ENetEvent event;

    while (!WindowShouldClose()) {
        // Input
        if (IsKeyDown(KEY_RIGHT)) me.x += 5;
        if (IsKeyDown(KEY_LEFT))  me.x -= 5;
        if (IsKeyDown(KEY_DOWN))  me.y += 5;
        if (IsKeyDown(KEY_UP))    me.y -= 5;

        // Envoyer position au serveur
        ENetPacket* packet = enet_packet_create(&me, sizeof(Player), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);

        // Recevoir positions des autres
        while (enet_host_service(client, &event, 0) > 0) {
            if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                Player* p = (Player*)event.packet->data;
                players[0] = *p; // simplification : un seul joueur pour l'exemple
                enet_packet_destroy(event.packet);
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircle((int)me.x, (int)me.y, 20, BLUE);   // toi
        DrawCircle((int)players[0].x, (int)players[0].y, 20, RED); // autre joueur
        EndDrawing();
    }

    CloseWindow();
    enet_host_destroy(client);
    enet_deinitialize();
    return 0;
}
