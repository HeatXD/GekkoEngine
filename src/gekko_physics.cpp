#include "gekko_physics.h"

Gekko::Physics::World::World() : _origin(Math::Unit::HALF, Math::Unit::HALF, Math::Unit::HALF)
{
}

Gekko::Physics::World::~World()
{
}

void Gekko::Physics::World::SetOrigin(const Math::Vec3& origin)
{
    _origin = origin;
}

int16_t Gekko::Physics::World::CreateBody(bool static_body)
{
    int16_t body_id = _bodies.insert(Body());
    _bodies.end()->is_static = static_body;
    return body_id;
}

bool Gekko::Physics::World::DestroyBody(int16_t body_id)
{
    if (_bodies.contains(body_id)) {
        // body doesnt exist.
        return false;
    }

    return true;
}
