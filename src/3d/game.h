#ifndef GAME_H
#define GAME_H

#define MAX_ENTITIES 4096
#define ENTITY_IDX_PLAYER 0

struct InputState {
    bool forward;
    bool backward;
    bool left;
    bool right;
};

struct Entity {
    float x, y, z;
    float speed;
    bool active; // NOTE: Defines when it's safe to replace this entity on the array
};

struct GameWorld {
    Entity entities[MAX_ENTITIES];
    int entityCount;
};

void gameInit(GameWorld* world);
void gameTick(GameWorld* world, InputState input, float dt);

#endif
