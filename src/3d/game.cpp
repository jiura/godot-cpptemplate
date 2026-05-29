#include <string.h>

#include "game.h"

Entity nilEntity = {};

// TODO: Receive at least a Vector3 for initial pos
Entity *spawnEntity(GameWorld *world) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity *e = &world->entities[i];

        if (!e->active) {
            *e = {};
            e->speed = 5.0f;
            e->active = true;
            
            return e;
        }
    }

    return &nilEntity;
}

void destroyEntity(Entity *e) {
    memset(e, 0, sizeof(Entity));
}

void gameInit(GameWorld *world) {
    world->entityCount = 0;

    Entity player = {};
    player.speed  = 5.0f;
    player.active = true;

    world->entities[world->entityCount++] = player;
}

void gameTick(GameWorld *world, InputState input, float dt) {
    nilEntity = {};

    Entity *player = &world->entities[ENTITY_IDX_PLAYER];

    float moveX = (float)input.forward - (float)input.backward;
    float moveZ = (float)input.right - (float)input.left;

    player->x += moveX * player->speed * dt;
    player->z += moveZ * player->speed * dt;
}
