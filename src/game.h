#ifndef GAME_H
#define GAME_H

#include "jvs/array.h"

#define PLAYER_ID 0

struct Player {
    float x;
    float y;
    float z;

    float speed;
};

struct InputState {
    bool forward;
    bool backward;
    bool left;
    bool right;
};

struct Entity {
    float x, y, z;
    float speed;
};

struct GameWorld {
    JVS_ARR(Entity) entities;
};

void gameInit(GameWorld* world);
void gameTick(GameWorld* world, InputState input, float dt);

#endif
