#include <string.h>

#include "game.h"

Entity nilEntity = {};

// TODO: Receive at least a Vector3 for initial pos
Entity *spawnEntity(GameWorld *world) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity *e = &world->entities[i];

        if (!e->active) {
            *e        = {};
            e->speed  = 5.0f;
            e->active = true;

            if (i > world->entityHiSlot) {
                world->entityHiSlot = i;
            }

            return e;
        }
    }

    return &nilEntity;
}

void destroyEntity(Entity *e) {
    memset(e, 0, sizeof(Entity));
}

void worldInit(GameWorld *world) {
    *world = {};
    world->player = spawnEntity(world);
}

void gameTick(GameWorld *world, InputState input, float dt) {
    nilEntity = {};

    if (input.togglePlayer) {
        if (!world->player) {
            world->player = spawnEntity(world);
        } else {
            world->player->active = false;
            world->player         = nullptr;
        }
    }

    if (world->player) {
        float moveX = (float)input.forward - (float)input.backward;
        float moveZ = (float)input.right - (float)input.left;

        world->player->x += moveX * world->player->speed * dt;
        world->player->z += moveZ * world->player->speed * dt;
    }
}
