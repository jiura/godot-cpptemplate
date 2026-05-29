#include "bridge.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

void GameNode::_bind_methods() {
}

GameNode::GameNode() {
}

void GameNode::_ready() {
    world = {};

    visualsCount = 0;

    gameInit(&world);

    // Creating player mesh node
    MeshInstance3D *cube = memnew(MeshInstance3D);

    Ref<BoxMesh> mesh;
    mesh.instantiate();
    cube->set_mesh(mesh);

    add_child(cube);

    // Binding player mesh node to player entity
    Entity *player = &world.entities[ENTITY_IDX_PLAYER];

    VisualBinding binding = {};
    binding.entity        = player;
    binding.node          = cube;

    visuals[visualsCount++] = binding;
}

void GameNode::_physics_process(double dt) {
    // NOTE: This avoid processing physics on the editor
    // if (Engine::get_singleton()->is_editor_hint())
    //     return;

    { // Handling input and updating entities
        InputState input          = {};
        Input     *inputSingleton = Input::get_singleton();

        input.forward  = inputSingleton->is_key_pressed(Key::KEY_W);
        input.backward = inputSingleton->is_key_pressed(Key::KEY_S);
        input.left     = inputSingleton->is_key_pressed(Key::KEY_A);
        input.right    = inputSingleton->is_key_pressed(Key::KEY_D);

        gameTick(&world, input, (float)dt);
    }

    { // Updating visual positions
        // TODO: VisualBindings to entities likely won't 1:1 like this in the future
        for (int i = 0; i < world.entityCount; ++i) {
            Entity *entity = &world.entities[i];
            if (!entity->active) {
                continue;
            }

            VisualBinding *visual = &visuals[i];

            visual->node->set_position(Vector3(entity->x,
                                               entity->y,
                                               entity->z));
        }
    }
}
