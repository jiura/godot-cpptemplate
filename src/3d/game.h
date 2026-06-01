#ifndef GAME_H
#define GAME_H

#define MAX_ENTITIES 4096

struct InputState {
    bool forward;
    bool backward;
    bool left;
    bool right;

    bool togglePlayer;
};

struct Entity {
    float x, y, z;
    float speed;
    bool active; // NOTE: Defines when it's safe to replace this entity on the array
};
extern Entity nilEntity;

struct GameWorld {
    Entity entities[MAX_ENTITIES];
    Entity *player;
    int entityHiSlot;
};

void worldInit(GameWorld* world);
void gameTick(GameWorld* world, InputState input, float dt);

#endif
