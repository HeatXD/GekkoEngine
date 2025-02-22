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
    Body body {};
    body.is_static = static_body;
    body.group_list_id = _id_reg.insert(new DS::Vec<int16_t>());
    return _bodies.insert(body);
}

bool Gekko::Physics::World::DestroyBody(int16_t body_id)
{
    if (_bodies.contains(body_id)) {
        // body doesnt exist.
        return false;
    }

    auto& body = _bodies.get(body_id);
    auto group_list = _id_reg.get(body.group_list_id);

    const uint32_t num_groups = group_list->size();

    for (uint32_t i = 0; i < num_groups; i++) {
        int16_t group_id = group_list->back();

        auto& group = _groups.get(group_id);
        auto object_list = _id_reg.get(group.object_list_id);

        const uint32_t num_objects = object_list->size();

        for (uint32_t i = 0; i < num_objects; i++) {
            int16_t object_id = object_list->back();

            // todo clear shape based on shape type.

            _objects.remove(object_id);
            object_list->pop_back();
        }

        _id_reg.remove(group.object_list_id);
        _groups.remove(group_id);

        group_list->pop_back();
    }

    _id_reg.remove(body.group_list_id);
    _bodies.remove(body_id);

    return true;
}
