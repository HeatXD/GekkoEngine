#pragma once

//---------------------------------
// Physics/Collision Engine Section
//---------------------------------

#include "gekko_math.h"
#include "gekko_ds.h"

namespace Gekko::Physics {
    // ==== Todo / Goals ====
    // - Sphere, Capsule, Diamond Collision Detection
    // - Sphere, Capsule, Diamond Collision Resolution
    // - Collision Layers
    // - Collision Events
    // - Modifiable Physics World Origin
    // - Easily Saved and Loaded World State

    struct Object {
        enum Type : uint16_t {
            Sphere,
            Capsule,
            Diamond,
        } type;

        int16_t shape_id;
    };

    struct ObjectGroup {
        uint16_t group_layers;
        uint16_t detect_layers;
        uint16_t resolve_layers;
        uint16_t react_layers;

        DS::Vec<int16_t>* object_ids;
    };

    struct Body {
        bool is_static;

        Math::Vec3 position;
        Math::Vec3 velocity;
        Math::Vec3 acceleration;

        DS::Vec<int16_t>* group_ids;
    };

    struct Sphere {
        Math::Vec3 local_center;
        Math::Unit radius;
    };

    struct Capsule {};

    struct Diamond {};

    struct World {
    private:
        Math::Vec3 _origin;

        // physics bodies 
        DS::SparseSet<int16_t, Body> _bodies;

        // collision object groups
        DS::SparseSet<int16_t, ObjectGroup> _groups;

        // collision objects
        DS::SparseSet<int16_t, Object> _objects;

        // collision shapes
        DS::SparseSet<int16_t, Sphere> _spheres;
        DS::SparseSet<int16_t, Capsule> _capsules;
        DS::SparseSet<int16_t, Diamond> _diamonds;

    public:
        World();

        ~World();

        void SetOrigin(const Math::Vec3& origin);

        bool CreateBody(int16_t& new_body_id);

        bool DestroyBody(int16_t body_id);

        bool CreateGroup(int16_t body_id, int16_t& new_group_id);

        bool DestoryGroup(int16_t body_id, int16_t group_id);

        bool AddObject(int16_t group_id, Object::Type type, int16_t& new_object_id);

        bool RemoveObject(int16_t group_id, int16_t object_id);

        bool SetBodyState(int16_t body_id, bool state);

        bool SetGroupState(int16_t group_id, bool state);
    };
}
