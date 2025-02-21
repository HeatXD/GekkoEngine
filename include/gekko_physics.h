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

        int16_t shape_index;
    };

    struct ObjectGroup {
        uint16_t group_layers;
        uint16_t detect_layers;
        uint16_t resolve_layers;
        uint16_t react_layers;

        // int16_t group_index;
    };

    struct Body {
        bool is_static;

        Math::Vec3 position;
        Math::Vec3 velocity;
        Math::Vec3 acceleration;

        // DS::Vec<int16_t> groups;
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
        DS::Vec<Sphere> _spheres;
        DS::Vec<Capsule> _capsules;
        DS::Vec<Diamond> _diamonds;

    public:
        World();

        void SetOrigin(const Math::Vec3& origin);

        int16_t CreateBody(bool static_body);

        bool DestroyBody(int16_t body_id);
    };
}
