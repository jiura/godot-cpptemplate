#include "game.h"

void gameInit(GameWorld *world) {
    world->entityCount = 0;

    Entity player = {};
    player.speed = 5.0f;
    player.active = true;

    world->entities[world->entityCount++] = player;
}

void gameTick(GameWorld *world, InputState input, float dt) {
    Entity *player = &world->entities[ENTITY_IDX_PLAYER];

    float moveX = (float)input.forward - (float)input.backward;
    float moveZ = (float)input.right - (float)input.left;

    player->x += moveX * player->speed * dt;
    player->z += moveZ * player->speed * dt;
}
