#include "game.h"
#include "jvs/array.h"

void gameInit(GameWorld *world) {
    Entity player = {.speed = 5.0f};

    jvs_arrPushBack(world->entities, player);
}

void gameTick(GameWorld *world, InputState input, float dt) {
    Entity *player = &world->entities[PLAYER_ID];

    float moveX = (float)input.forward - (float)input.backward;
    float moveZ = (float)input.right - (float)input.left;

    player->x += moveX * player->speed * dt;
    player->z += moveZ * player->speed * dt;
}
