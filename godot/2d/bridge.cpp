#include "../../src/2d/game.cpp"

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>

#define ACTION_UP "move-up"
#define ACTION_DOWN "move-down"
#define ACTION_LEFT "move-left"
#define ACTION_RIGHT "move-right"
#define ACTION_TOGGLE_PLAYER "toggle-player"
#define ACTION_SHOOT "shoot"

#define COLOR_RED (Color){1, 0, 0}
#define COLOR_GREEN (Color){0, 1, 0}
#define COLOR_BLUE (Color){0, 0, 1}

using namespace godot;

struct EntityNode {
    CharacterBody2D *body;
};

class GameNode : public Node2D {
    GDCLASS(GameNode, Node2D);

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

CharacterBody2D *createSquareBody(Vector2 pos, Color color) {
    CharacterBody2D *body = memnew(CharacterBody2D);

    body->set_position(pos);

    // NOTE: Grounded mode is useful for platformers due to
    // gravity-based movement, but glitches out with topdown movement
    // body->set_motion_mode(CharacterBody2D::MOTION_MODE_GROUNDED);
    body->set_motion_mode(CharacterBody2D::MOTION_MODE_FLOATING);

    // Hitbox
    CollisionShape2D *shape = memnew(CollisionShape2D);

    RectangleShape2D *rect = memnew(RectangleShape2D);
    rect->set_size(Vector2(16, 16));
    shape->set_shape(rect);

    body->add_child(shape);

    // Visual
    Polygon2D *poly = memnew(Polygon2D);

    PackedVector2Array points;
    points.push_back(Vector2(-8, -8));
    points.push_back(Vector2(8, -8));
    points.push_back(Vector2(8, 8));
    points.push_back(Vector2(-8, 8));

    poly->set_polygon(points);
    poly->set_color(color);

    body->add_child(poly);

    return body;
}

CharacterBody2D *createCircleBody(Vector2 pos, Color color) {
    CharacterBody2D *body = memnew(CharacterBody2D);

    body->set_position(pos);

    // NOTE: Grounded mode is useful for platformers due to
    // gravity-based movement, but glitches out with topdown movement
    // body->set_motion_mode(CharacterBody2D::MOTION_MODE_GROUNDED);
    body->set_motion_mode(CharacterBody2D::MOTION_MODE_FLOATING);

    // Hitbox
    CollisionShape2D *shape = memnew(CollisionShape2D);

    CircleShape2D *circle = memnew(CircleShape2D);
    circle->set_radius(4.0f);
    shape->set_shape(circle);

    body->add_child(shape);

    // Visual
    Polygon2D *poly = memnew(Polygon2D);

    PackedVector2Array points;
    const int          segments = 16;
    const float        radius   = 4.0f;

    for (int i = 0; i < segments; ++i) {
        float a = (Math_TAU * i) / segments;
        points.push_back((Vector2){(float)cos(a) * radius,
                                   (float)sin(a) * radius});
    }

    poly->set_polygon(points);
    poly->set_color(color);

    body->add_child(poly);

    return body;
}

// Init game
void GameNode::_ready() {
    memset(nodes, 0, sizeof(nodes));

    { // Define input actions
        InputMap *im = InputMap::get_singleton();

        im->add_action(ACTION_UP);
        Ref<InputEventKey> w;
        w.instantiate();
        w->set_physical_keycode(Key::KEY_W);
        im->action_add_event(ACTION_UP, w);

        im->add_action(ACTION_DOWN);
        Ref<InputEventKey> s;
        s.instantiate();
        s->set_physical_keycode(Key::KEY_S);
        im->action_add_event(ACTION_DOWN, s);

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

        input.up    = inputSingleton->is_action_pressed(ACTION_UP);
        input.down  = inputSingleton->is_action_pressed(ACTION_DOWN);
        input.left  = inputSingleton->is_action_pressed(ACTION_LEFT);
        input.right = inputSingleton->is_action_pressed(ACTION_RIGHT);

        input.togglePlayer = inputSingleton->is_action_just_released(ACTION_TOGGLE_PLAYER);
        input.shoot        = inputSingleton->is_action_just_pressed(ACTION_SHOOT);
    }

    gameTick(&world, input, (float)dt);

    { // Update nodes
        for (int i = 0; i <= world.entityHiSlot; ++i) {
            Entity     *entity = &world.entities[i];
            EntityNode *node   = &nodes[i];

            if (!entity->active) {
                if (node->body) {
                    node->body->queue_free();
                    node->body = nullptr;
                }
                continue;
            }

            if (!node->body) {
                CharacterBody2D *body = nullptr;
                Vector2          pos;

                switch (entity->type) {
                    case ENTITY_PROJECTILE: {
                        pos              = nodes[world.player->id].body->get_global_position();
                        Vector2 mousePos = get_global_mouse_position();
                        entity->move     = {mousePos.x - pos.x, mousePos.y - pos.y};
                        body             = createCircleBody(pos, COLOR_BLUE);
                        break;
                    }

                    case ENTITY_ENEMY: {
                        pos  = (Vector2){0.0f, 0.0f};
                        body = createSquareBody(pos, COLOR_RED);
                        break;
                    }

                    case ENTITY_PLAYER: {
                        pos  = (Vector2){0.0f, 0.0f};
                        body = createSquareBody(pos, COLOR_GREEN);
                        break;
                    }
                }

                add_child(body);
                node->body = body;
            }

            Vector2 dir = {entity->move.x, entity->move.y};
            if (dir.length() > 0.0f) {
                dir = dir.normalized();
            }

            node->body->set_velocity(dir * entity->moveSpeed);
            node->body->move_and_slide();
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
