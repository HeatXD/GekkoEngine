#include "gekko_physics.h"

Gekko::Physics::World::World() : _origin(Math::Unit::HALF, Math::Unit::HALF, Math::Unit::HALF)
{
}

void Gekko::Physics::World::SetOrigin(const Math::Vec3& origin)
{
    _origin = origin;
}

int16_t Gekko::Physics::World::CreateBody(bool static_body)
{
    int16_t body_id = -1;
    if (_zombie_bodies.empty()) {
        // Create a new body
        body_id = _bodies.size();
        _bodies.push_back(Body());
        _bodies.back().is_static = static_body;
    }
    else {
        // repurpose old body
        body_id = _zombie_bodies.back();
        _bodies[body_id].is_static = static_body;
        // remove from the zombie list.
        _zombie_bodies.pop_back();
    }
    return body_id;
}

bool Gekko::Physics::World::DestroyBody(int16_t body_id)
{
    if (_bodies.size() <= body_id) {
        // body doesnt exist.
        return false;
    }

    if (_zombie_bodies.contains(body_id) != -1) {
        // body is already a zombie
        return false;
    }

    // invalidate this body.
    auto& b = _bodies[body_id];

    b.is_static = false;
    b.position = b.velocity = b.acceleration = Math::Vec3::ZERO;

    // invalidate the groups 
    for (uint32_t i = 0; i < b.groups.size(); i++) {
        int16_t group_id = b.groups[i];
        auto& g = _groups[group_id];

        // reset all the masks
        g.group_layers = g.detect_layers = g.resolve_layers = g.react_layers = 0;

        // invalidate the objects 
        for (uint32_t j = 0; j < g.objects.size(); j++) {
            int16_t object_id = g.objects[j];
            auto& o = _objects[object_id];
            // Todo INVALIDATE SHAPES.
            // mark invalid objects
            _zombie_objects.push_back(j);
        }
        g.objects.clear();

        // mark invalid groups
        _zombie_groups.push_back(i);
    }
    b.groups.clear();

    // mark invalid body
    _zombie_bodies.push_back(body_id);

    return true;
}
