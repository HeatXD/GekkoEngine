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

        DS::Vec<int16_t> objects;
    };

    struct Body {
        bool is_static;

        Math::Vec3 position;
        Math::Vec3 velocity;
        Math::Vec3 acceleration;

        DS::Vec<int16_t> groups;
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
        DS::Vec<Body> _bodies;

        // collision object groups
        DS::Vec<ObjectGroup> _groups;

        // collision objects
        DS::Vec<Object> _objects;

        // collision shapes
        DS::Vec<Sphere> _spheres;
        DS::Vec<Capsule> _capsules;
        DS::Vec<Diamond> _diamonds;

        // book keeping
        // we also want to try and reuse as much of the preallocated buffers as we can
        DS::Vec<int16_t> _zombie_bodies;
        DS::Vec<int16_t> _zombie_groups;
        DS::Vec<int16_t> _zombie_objects;

        DS::Vec<int16_t> _zombie_spheres;
        DS::Vec<int16_t> _zombie_capsules;
        DS::Vec<int16_t> _zombie_diamonds;

    public:
        World();

        void SetOrigin(const Math::Vec3& origin);

        int16_t CreateBody(bool static_body);

        bool DestroyBody(int16_t body_id);
    };
}
