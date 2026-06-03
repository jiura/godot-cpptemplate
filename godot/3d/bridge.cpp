#include "../../src/3d/game.cpp"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>

#define ACTION_FORWARD "move-forward"
#define ACTION_BACKWARD "move-backward"
#define ACTION_LEFT "move-left"
#define ACTION_RIGHT "move-right"
#define ACTION_TOGGLE_PLAYER "toggle-player"
#define ACTION_SHOOT "shoot"

using namespace godot;

struct VisualBinding {
    Node3D *node;
};

class GameNode : public Node3D {
    GDCLASS(GameNode, Node3D);

private:
    GameWorld world;

    VisualBinding visuals[MAX_ENTITIES];
    int           visualsHiSlot;

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

MeshInstance3D *createPlayerMesh() {
    MeshInstance3D *cube = memnew(MeshInstance3D);

    Ref<BoxMesh> mesh;
    mesh.instantiate();
    cube->set_mesh(mesh);

    return cube;
}

MeshInstance3D *createProjectileMesh() {
    MeshInstance3D *sphere = memnew(MeshInstance3D);

    Ref<SphereMesh> mesh;
    mesh.instantiate();
    sphere->set_mesh(mesh);

    return sphere;
}

// Init game
void GameNode::_ready() {
    visualsHiSlot = 0;

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

    { // Create player
        // Create mesh node
        MeshInstance3D *cube = createPlayerMesh();
        add_child(cube);

        // Bind mesh node to entity
        VisualBinding binding = {};
        binding.node          = cube;

        visuals[visualsHiSlot] = binding;
    }
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
            Entity        *entity = &world.entities[i];
            VisualBinding *visual = &visuals[i];

            if (!entity->active) {
                if (visual->node) {
                    visual->node->queue_free();
                    visual->node = nullptr;
                }
                continue;
            }

            if (!visual->node) {
                // TODO: Check visualHiSlot when creating new visuals
                MeshInstance3D *mesh = nullptr;
                switch (entity->type) {
                    case ENTITY_PLAYER:
                        mesh = createPlayerMesh();
                        break;
                    case ENTITY_PROJECTILE:
                        mesh = createProjectileMesh();
                        break;
                    default:
                        // TODO
                        break;
                }

                if (mesh) {
                    add_child(mesh);
                    visual->node = mesh;
                }
            }

            visual->node->set_position(Vector3(entity->pos.x,
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
