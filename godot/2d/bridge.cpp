#include "../../src/2d/game.cpp"
#include "jvs/array.h"

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#define GAMENODE_METHOD_ON_COLLISION "on_collision"

#define ACTION_UP "move-up"
#define ACTION_DOWN "move-down"
#define ACTION_LEFT "move-left"
#define ACTION_RIGHT "move-right"
#define ACTION_TOGGLE_PLAYER "toggle-player"
#define ACTION_SHOOT "shoot"

#define COLOR_RED (Color){1, 0, 0}
#define COLOR_GREEN (Color){0, 1, 0}
#define COLOR_BLUE (Color){0, 0, 1}

#define NODE2D(x) ((Node2D *)x)
#define AREA2D(x) ((Area2D *)x)
#define BODY2D(x) ((CharacterBody2D *)x)

// Collision layers
#define COLLAYER_PLAYER (1 << 0)
#define COLLAYER_ENEMIES (1 << 1)
#define COLLAYER_PLAYER_PROJECTILE (1 << 2)

#define NODE_NM_POLYGON "Polygon2D"

using namespace godot;

enum EntityNodeType {
    ENTITY_NODE_BODY,
    ENTITY_NODE_AREA,
};

struct EntityNode {
    Node2D        *root;
    EntityNodeType type;
};

class GameNode : public Node2D {
    GDCLASS(GameNode, Node2D);

private:
    GameWorld world;

    EntityNode nodes[MAX_ENTITIES] = {};

protected:
    static void _bind_methods();

public:
    GameNode();

    void _ready() override;
    void _process(double dt) override;
    void _physics_process(double dt) override;

    void on_collision(Node2D *other, Node2D *self);
};

void GameNode::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD(GAMENODE_METHOD_ON_COLLISION, "other", "self"),
        &GameNode::on_collision);
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
    poly->set_name(NODE_NM_POLYGON);

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
    poly->set_name(NODE_NM_POLYGON);

    PackedVector2Array points;
    const int          segments = 16;
    const float        radius   = 4.0f;

    for (size_t i = 0; i < segments; ++i) {
        float a = (Math_TAU * i) / segments;
        points.push_back((Vector2){(float)cos(a) * radius,
                                   (float)sin(a) * radius});
    }

    poly->set_polygon(points);
    poly->set_color(color);

    body->add_child(poly);

    return body;
}

Area2D *createCircleArea(Vector2 pos, Color color) {
    Area2D *area = memnew(Area2D);

    area->set_position(pos);

    // Hitbox
    CollisionShape2D *shape = memnew(CollisionShape2D);

    CircleShape2D *circle = memnew(CircleShape2D);
    circle->set_radius(4.0f);
    shape->set_shape(circle);

    area->add_child(shape);

    // Visual
    Polygon2D *poly = memnew(Polygon2D);
    poly->set_name(NODE_NM_POLYGON);

    PackedVector2Array points;
    const int          segments = 16;
    const float        radius   = 4.0f;

    for (size_t i = 0; i < segments; ++i) {
        float a = (Math_TAU * i) / segments;
        points.push_back((Vector2){(float)cos(a) * radius,
                                   (float)sin(a) * radius});
    }

    poly->set_polygon(points);
    poly->set_color(color);

    area->add_child(poly);

    return area;
}

// -1 for not found
int findEntityNodeIdByRoot(EntityNode *nodes, Node2D *node, size_t entityHiSlot) {
    // entityHiSlot show be <= MAX_ENTITIES that right now is 4096
    assert(entityHiSlot < INT_MAX);
    for (size_t i = 0; i <= entityHiSlot; ++i) {
        if (nodes[i].root == node) {
            return i;
        }
    }

    return -1;
}

void GameNode::on_collision(Node2D *other, Node2D *self) {
    int oId = findEntityNodeIdByRoot(nodes, other, world.entityHiSlot);
    int sId = findEntityNodeIdByRoot(nodes, self, world.entityHiSlot);

    if (oId < 0 || sId < 0) {
        // TODO: LOG that entity wasn't found
        return;
    }

    CollisionEvent ce = {.self  = &world.entities[sId],
                         .other = &world.entities[oId]};

    jvs_arrPushBack(world.events.collisions, ce);
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

    if (world.events.collisions) {
        jvs_arrEmpty(world.events.collisions);
    }

    { // Update nodes
        for (size_t i = 0; i <= world.entityHiSlot; ++i) {
            Entity     *entity = &world.entities[i];
            EntityNode *node   = &nodes[i];

            if (!entity->active) {
                if (node->root) {
                    node->root->queue_free();
                    node->root = nullptr;
                }
                continue;
            }

            /* Create visual -- START */
            if (!node->root) {
                Vector2 pos;

                switch (entity->type) {
                    case ENTITY_PROJECTILE: {
                        pos              = nodes[world.player->id].root->get_global_position();
                        Vector2 mousePos = get_global_mouse_position();
                        // TODO: This is the bridge handling gameplay stuff...
                        entity->move = {mousePos.x - pos.x, mousePos.y - pos.y};

                        node->root = createCircleArea(pos, COLOR_BLUE);
                        AREA2D(node->root)->set_collision_layer(COLLAYER_PLAYER_PROJECTILE);
                        AREA2D(node->root)->set_collision_mask(COLLAYER_ENEMIES);

                        node->root->connect("body_entered",
                                            Callable(this, GAMENODE_METHOD_ON_COLLISION).bind(node->root));

                        node->type = ENTITY_NODE_AREA;
                        break;
                    }

                    case ENTITY_ENEMY: {
                        pos = (Vector2){0.0f, 0.0f};

                        Ref<PackedScene> scene = ResourceLoader::get_singleton()->load("res://scenes/entities/chars/enemy.tscn");
                        node->root             = NODE2D(scene->instantiate());
                        BODY2D(node->root)->set_collision_layer(COLLAYER_ENEMIES);
                        BODY2D(node->root)->set_collision_mask(COLLAYER_PLAYER);

                        node->type = ENTITY_NODE_BODY;
                        break;
                    }

                    case ENTITY_PLAYER: {
                        pos = (Vector2){0.0f, 0.0f};

                        Ref<PackedScene> scene = ResourceLoader::get_singleton()->load("res://scenes/entities/chars/player.tscn");
                        node->root             = NODE2D(scene->instantiate());
                        BODY2D(node->root)->set_collision_layer(COLLAYER_PLAYER);
                        BODY2D(node->root)->set_collision_mask(COLLAYER_ENEMIES);

                        node->type = ENTITY_NODE_BODY;
                        break;
                    }

                    case ENTITY_NONE:
                    default:
                        break;
                }

                if (!node->root) {
                    continue;
                }

                add_child(node->root);
            }
            /* Create visual -- END */

            /* Update visual -- START */
            if (entity->type == ENTITY_ENEMY) {
                if (entity->hp <= 0.0f) {
                    Polygon2D *poly = node->root->get_node<Polygon2D>(NODE_NM_POLYGON);
                    if (poly) {
                        poly->set_color(COLOR_BLUE);
                    }
                }
            }
            /* Update visual -- END */

            Vector2 dir = {entity->move.x, entity->move.y};
            if (dir.length() > 0.0f) {
                dir = dir.normalized();
            }

            switch (node->type) {
                case ENTITY_NODE_BODY: {
                    BODY2D(node->root)->set_velocity(dir * entity->moveSpeed);
                    BODY2D(node->root)->move_and_slide();
                    break;
                }

                case ENTITY_NODE_AREA: {
                    Vector2 currentPos = AREA2D(node->root)->get_position();
                    AREA2D(node->root)->set_position(currentPos + dir * entity->moveSpeed * dt);
                    break;
                }
            }
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
