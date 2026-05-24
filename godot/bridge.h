#ifndef BRIDGE_H
#define BRIDGE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

#include "../src/game.h"

using namespace godot;

class GameNode : public Node3D {
    GDCLASS(GameNode, Node3D);

private:
    GameWorld world;

    MeshInstance3D* cube;

protected:
    static void _bind_methods();

public:
    GameNode();

    void _ready() override;
    void _physics_process(double dt) override;
};

#endif
