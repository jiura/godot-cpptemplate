#include "bridge.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>

void GameNode::_bind_methods() {
}

GameNode::GameNode() {
}

void GameNode::_ready() {
    world.entities = NULL;

    gameInit(&world);

    if (has_node("Cube")) {
        cube = get_node<MeshInstance3D>("Cube");
    } else {
        cube = nullptr;
    }
}

void GameNode::_physics_process(double dt) {
    // NOTE: This avoid processing physics on the editor
    // if (Engine::get_singleton()->is_editor_hint())
    //     return;

    InputState input          = {};
    Input     *inputSingleton = Input::get_singleton();

    input.forward  = inputSingleton->is_key_pressed(Key::KEY_W);
    input.backward = inputSingleton->is_key_pressed(Key::KEY_S);
    input.left     = inputSingleton->is_key_pressed(Key::KEY_A);
    input.right    = inputSingleton->is_key_pressed(Key::KEY_D);

    gameTick(&world, input, (float)dt);

    Entity *player = &world.entities[PLAYER_ID];

    if (cube) {
        cube->set_position(Vector3(
            player->x,
            player->y,
            player->z));
    }
}
