#include <string.h>

#define MAX_ENTITIES 4096

struct InputState {
    bool up;
    bool down;
    bool left;
    bool right;

    bool togglePlayer;

    bool shoot;
};

// Not to be confuse with Godot's own Vector2
struct Vec2 {
    float x, y;
};

enum EntityType {
    ENTITY_PLAYER,
    ENTITY_PROJECTILE,
    ENTITY_ENEMY
};

struct Entity {
    Vec2 move; // Determines movement intention for this frame

    float moveSpeed;
    float lifetime;

    EntityType type;

    int  id;     // Index on the arrays
    bool active; // Defines when it's safe to replace this entity on the array
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
            e->moveSpeed = 250.0f;
            e->id        = i;
            e->active    = true;

            if (i > world->entityHiSlot) {
                world->entityHiSlot = i;
            }

            return e;
        }
    }

    return &nilEntity;
}

Entity *spawnProjectile(GameWorld *world, Vec2 move) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity *e = &world->entities[i];

        if (!e->active) {
            *e = {};

            e->move      = move;
            e->moveSpeed = 500.0f;
            e->lifetime  = 2.0f;
            e->type      = ENTITY_PROJECTILE;
            e->id        = i;
            e->active    = true;

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

            e->type      = ENTITY_ENEMY;
            e->moveSpeed = 5.0f;
            e->id        = i;
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
    *world = {};

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
        // Movement
        world->player->move.x = (float)input.right - (float)input.left;
        world->player->move.y = (float)input.down - (float)input.up;

        // Shoot
        if (input.shoot) {
            spawnProjectile(world, (Vec2){1.0f, 0.0f});
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
    }
}
