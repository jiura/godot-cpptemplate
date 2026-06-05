#include "../../src/3d/game.cpp"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>
#include <godot_cpp/classes/box_mesh.hpp>

#define ACTION_FORWARD "move-forward"
#define ACTION_BACKWARD "move-backward"
#define ACTION_LEFT "move-left"
#define ACTION_RIGHT "move-right"
#define ACTION_TOGGLE_PLAYER "toggle-player"
#define ACTION_SHOOT "shoot"

using namespace godot;

struct EntityNode {
    MeshInstance3D *visual;
};

class GameNode : public Node3D {
    GDCLASS(GameNode, Node3D);

private:
    GameWorld world;

    EntityNode nodes[MAX_ENTITIES];

protected:
    static void _bind_methods();

public:
    GameNode();

    void _ready() override;
    void _process(double dt) override;
    void _physics_process(double dt) override;
};

void GameNode::_bind_methods() {
}

GameNode::GameNode() {
}

MeshInstance3D *createVisualNode(EntityType type) {
    MeshInstance3D *visual = memnew(MeshInstance3D);

    switch (type) {
        case ENTITY_PLAYER: {
            Ref<CylinderMesh> mesh;
            mesh.instantiate();
            visual->set_mesh(mesh);
            break;
        }

        case ENTITY_PROJECTILE: {
            Ref<SphereMesh> mesh;
            mesh.instantiate();
            visual->set_mesh(mesh);
            break;
        }

        case ENTITY_ENEMY: {
            Ref<BoxMesh> mesh;
            mesh.instantiate();
            visual->set_mesh(mesh);
            break;
        }

        default:
            return nullptr;
    }

    return visual;
}

// Init game
void GameNode::_ready() {
    memset(nodes, 0, sizeof(nodes));

    { // Define input actions
        InputMap *im = InputMap::get_singleton();

        im->add_action(ACTION_FORWARD);
        Ref<InputEventKey> w;
        w.instantiate();
        w->set_physical_keycode(Key::KEY_W);
        im->action_add_event(ACTION_FORWARD, w);

        im->add_action(ACTION_BACKWARD);
        Ref<InputEventKey> s;
        s.instantiate();
        s->set_physical_keycode(Key::KEY_S);
        im->action_add_event(ACTION_BACKWARD, s);

        im->add_action(ACTION_LEFT);
        Ref<InputEventKey> a;
        a.instantiate();
        a->set_physical_keycode(Key::KEY_A);
        im->action_add_event(ACTION_LEFT, a);

        im->add_action(ACTION_RIGHT);
        Ref<InputEventKey> d;
        d.instantiate();
        d->set_physical_keycode(Key::KEY_D);
        im->action_add_event(ACTION_RIGHT, d);

        im->add_action(ACTION_TOGGLE_PLAYER);
        Ref<InputEventKey> space;
        space.instantiate();
        space->set_physical_keycode(Key::KEY_SPACE);
        im->action_add_event(ACTION_TOGGLE_PLAYER, space);

        im->add_action(ACTION_SHOOT);
        Ref<InputEventMouseButton> leftMouse;
        leftMouse.instantiate();
        leftMouse->set_button_index(MouseButton::MOUSE_BUTTON_LEFT);
        im->action_add_event(ACTION_SHOOT, leftMouse);
    }

    worldInit(&world);
}

void GameNode::_process(double dt) {
    // NOTE: This avoids processing on the editor
    // if (Engine::get_singleton()->is_editor_hint())
    //     return;

    // TODO:
    // _physics_process()
    //     gameTick()
    //     syncVisuals()
    //
    // _process()
    //     gather input
    //     UI
    //     menus
    //     debug overlays
    //     purely visual effects
}

void GameNode::_physics_process(double dt) {
    // NOTE: This avoids processing physics on the editor
    // if (Engine::get_singleton()->is_editor_hint())
    //     return;

    InputState input = {};
    { // Handle input
        Input *inputSingleton = Input::get_singleton();

        input.forward  = inputSingleton->is_action_pressed(ACTION_FORWARD);
        input.backward = inputSingleton->is_action_pressed(ACTION_BACKWARD);
        input.left     = inputSingleton->is_action_pressed(ACTION_LEFT);
        input.right    = inputSingleton->is_action_pressed(ACTION_RIGHT);

        input.togglePlayer = inputSingleton->is_action_just_released(ACTION_TOGGLE_PLAYER);
        input.shoot        = inputSingleton->is_action_just_pressed(ACTION_SHOOT);
    }

    gameTick(&world, input, (float)dt);

    { // Update visuals
        for (int i = 0; i <= world.entityHiSlot; ++i) {
            Entity     *entity = &world.entities[i];
            EntityNode *node = &nodes[i];

            if (!entity->active) {
                if (node->visual) {
                    node->visual->queue_free();
                    node->visual = nullptr;
                }
                continue;
            }

            if (!node->visual) {
                MeshInstance3D *visual = createVisualNode(entity->type);
                if (visual) {
                    add_child(visual);
                    node->visual = visual;
                }
            }

            node->visual->set_position(Vector3(entity->pos.x,
                                                 entity->pos.y,
                                                 entity->pos.z));
        }
    }
}

/* GDExtension config */

void initialize_CPPTEMPLATE_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<GameNode>();
}

void uninitialize_CPPTEMPLATE_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Entry point
GDExtensionBool GDE_EXPORT CPPTEMPLATE_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    GDExtensionClassLibraryPtr         p_library,
    GDExtensionInitialization         *r_initialization) {
    GDExtensionBinding::InitObject init_obj(
        p_get_proc_address,
        p_library,
        r_initialization);

    init_obj.register_initializer(initialize_CPPTEMPLATE_module);
    init_obj.register_terminator(uninitialize_CPPTEMPLATE_module);

    init_obj.set_minimum_library_initialization_level(
        MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
