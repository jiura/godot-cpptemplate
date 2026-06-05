#include <string.h>

#define MAX_ENTITIES 4096

struct InputState {
    bool forward;
    bool backward;
    bool left;
    bool right;

    bool togglePlayer;

    bool shoot;
};

// Not to be confuse with Godot's own Vector3
struct Vec3 {
    float x, y, z;
};

enum EntityType {
    ENTITY_PLAYER,
    ENTITY_PROJECTILE,
    ENTITY_ENEMY
};

struct Entity {
    Vec3 pos;
    Vec3 vel;

    float moveSpeed;
    float lifetime;

    EntityType type;
    bool       active; // NOTE: Defines when it's safe to replace this entity on the array
};
extern Entity nilEntity;

struct GameWorld {
    Entity  entities[MAX_ENTITIES];
    Entity *player;
    int     entityHiSlot;
};

Entity nilEntity = {};

Entity *spawnPlayer(GameWorld *world) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity *e = &world->entities[i];

        if (!e->active) {
            *e = {};

            e->type      = ENTITY_PLAYER;
            e->moveSpeed = 5.0f;
            e->active    = true;

            if (i > world->entityHiSlot) {
                world->entityHiSlot = i;
            }

            return e;
        }
    }

    return &nilEntity;
}

Entity *spawnProjectile(GameWorld *world,
                        Vec3       pos,
                        Vec3       velocity) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity *e = &world->entities[i];

        if (!e->active) {
            *e = {};

            e->pos      = pos;
            e->vel      = velocity;
            e->lifetime = 2.0f;
            e->type     = ENTITY_PROJECTILE;
            e->active   = true;

            if (i > world->entityHiSlot) {
                world->entityHiSlot = i;
            }

            return e;
        }
    }

    return &nilEntity;
}

Entity *spawnEnemy(GameWorld *world) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity *e = &world->entities[i];

        if (!e->active) {
            *e = {};

            e->pos = (Vec3){15, 0, 0};
            e->type      = ENTITY_ENEMY;
            e->moveSpeed = 5.0f;
            e->active    = true;

            if (i > world->entityHiSlot) {
                world->entityHiSlot = i;
            }

            return e;
        }
    }

    return &nilEntity;
}

// Prefer this over just setting active to false,
// as it avoids leaving trash behind
void destroyEntity(Entity *e) {
    memset(e, 0, sizeof(Entity));
}

void worldInit(GameWorld *world) {
    *world        = {};

    world->player = spawnPlayer(world);
    spawnEnemy(world);
}

void gameTick(GameWorld *world, InputState input, float dt) {
    nilEntity = {};

    /* Handle input */
    if (input.togglePlayer) {
        if (!world->player) {
            world->player = spawnPlayer(world);
        } else {
            world->player->active = false;
            world->player         = nullptr;
        }
    }

    if (world->player) {
        // Position
        float moveX = (float)input.forward - (float)input.backward;
        float moveZ = (float)input.right - (float)input.left;

        world->player->vel.x = moveX * world->player->moveSpeed;
        world->player->vel.z = moveZ * world->player->moveSpeed;

        // Shoot
        if (input.shoot) {
            spawnProjectile(world,
                            world->player->pos,
                            (Vec3){40.0f, 0.0f, 0.0f});
        }
    }

    /* Update entity status */
    for (int i = 0; i <= world->entityHiSlot; ++i) {
        Entity *e = &world->entities[i];

        if (!e->active) {
            continue;
        }

        if (e->type == ENTITY_PROJECTILE) {
            e->lifetime -= dt;

            if (e->lifetime <= 0.0f) {
                destroyEntity(e);
                continue;
            }
        }

        // Position
        e->pos.x += e->vel.x * dt;
        e->pos.y += e->vel.y * dt;
        e->pos.z += e->vel.z * dt;
    }
}
