#include "gekko_physics.h"

Gekko::Physics::World::World() :
    _origin(Math::Unit::HALF, Math::Unit::HALF, Math::Unit::HALF),
    _up(Math::Unit(), Math::Unit::ONE, Math::Unit())
{
}

Gekko::Physics::World::~World()
{
    auto cleanup_list = DS::Vec<int16_t>();

    // acquire id's
    auto current = _bodies.begin();
    while (current != _bodies.end_set()) {
        cleanup_list.push_back(current->id);
        current++;
    }

    // cleanup bodies
    for (uint32_t i = 0; i < cleanup_list.size(); i++) {
        DestroyBody(cleanup_list[i]);
    }
}

void Gekko::Physics::World::SetOrientation(const Math::Vec3& up)
{
    _up = up;
}

void Gekko::Physics::World::SetOrigin(const Math::Vec3& origin)
{
    _origin = origin;
}

bool Gekko::Physics::World::CreateBody(int16_t& new_body_id)
{
    new_body_id = _bodies.insert(Body());

    if (new_body_id == _bodies.INVALID_ID) {
        return false;
    }

    _bodies.end()->id = new_body_id;
    _bodies.end()->group_ids = new DS::Vec<int16_t>();

    return true;
}

bool Gekko::Physics::World::DestroyBody(int16_t body_id)
{
    if (_bodies.contains(body_id)) {
        // body doesnt exist.
        return false;
    }

    auto& body = _bodies.get(body_id);
    auto group_list = body.group_ids;
    const uint32_t num_groups = group_list->size();

    for (uint32_t i = 0; i < num_groups; i++) {
        DestoryGroup(body_id, group_list->back());
        group_list->pop_back();
    }

    // cleanup group id list
    delete body.group_ids;
    body.group_ids = nullptr;

    return true;
}

bool Gekko::Physics::World::CreateGroup(int16_t body_id, int16_t& new_group_id)
{
    if (!_bodies.contains(body_id)) {
        // no body found. cant create group
        return false;
    }

    new_group_id = _groups.insert(ObjectGroup());

    if (new_group_id == _groups.INVALID_ID) {
        return false;
    }

    _groups.end()->object_ids = new DS::Vec<int16_t>();

    return true;
}

bool Gekko::Physics::World::DestoryGroup(int16_t body_id, int16_t group_id)
{
    if (!_bodies.contains(body_id) || !_groups.contains(group_id)) {
        // no body or group found. cant remove group
        return false;
    }

    auto& group = _groups.get(group_id);
    auto obj_list = group.object_ids;
    const uint32_t num_objects = obj_list->size();

    for (uint32_t i = 0; i < num_objects; i++) {
        RemoveObject(group_id, obj_list->back());
        obj_list->pop_back();
    }

    // de-attach group from body
    _bodies.get(body_id).group_ids->remove_first(group_id);

    // cleanup object id list
    delete group.object_ids;
    group.object_ids = nullptr;

    return true;
}

bool Gekko::Physics::World::AddObject(int16_t group_id, Object::Type type, int16_t& new_object_id)
{
    if (!_groups.contains(group_id)) {
        // no group found / invalid group found. cant create object
        return false;
    }

    new_object_id = _objects.insert(Object());

    if (new_object_id == _objects.INVALID_ID) {
        return false;
    }

    int16_t shape_id = _objects.INVALID_ID;

    switch (type)
    {
    case Gekko::Physics::Object::Sphere:
        shape_id = _spheres.insert(Sphere());
        break;
    case Gekko::Physics::Object::Capsule:
        shape_id = _capsules.insert(Capsule());
        break;
    default:
        break;
    }

    if (shape_id == _objects.INVALID_ID) {
        return false;
    }

    // attach object to the group
    _groups.get(group_id).object_ids->push_back(new_object_id);

    // attach shape to the object
    _objects.end()->shape_id = shape_id;
    _objects.end()->type = type;

    return true;
}

bool Gekko::Physics::World::RemoveObject(int16_t group_id, int16_t object_id)
{
    if (!_objects.contains(object_id) || !_groups.contains(group_id)) {
        // no object found / invalid object found / invalid group found. cant remove object
        return false;
    }

    // de-attach from the group
    _groups.get(group_id).object_ids->remove_first(object_id);

    // cleanup the shape
    auto& obj = _objects.get(object_id);
    switch (obj.type)
    {
    case Gekko::Physics::Object::Sphere:
        _spheres.remove(obj.shape_id);
        break;
    case Gekko::Physics::Object::Capsule:
        _capsules.remove(obj.shape_id);
        break;
    default:
        break;
    }

    // cleanup the object
    _objects.remove(object_id);

    return true;
}

bool Gekko::Physics::World::SetBodyState(int16_t body_id, bool state)
{
    if (!_bodies.contains(body_id)) {
        return false;
    }

    if (state) {
        _bodies.enable(body_id);
    } else {
        _bodies.disable(body_id);
    }

    return true;
}

bool Gekko::Physics::World::SetGroupState(int16_t group_id, bool state)
{
    if (!_groups.contains(group_id)) {
        return false;
    }

    if (state) {
        _groups.enable(group_id);
    } else {
        _groups.disable(group_id);
    }

    return true;
}

void Gekko::Physics::World::Update()
{
    // find collision pairs
    DetectPairs();

    // resolve the collision pairs
    ResolvePairs();

    // apply body movement
    IntegrateBodies();

    // send out signals
    ReactPairs();
}

void Gekko::Physics::World::DetectPairs()
{
    // cleanup pairs
    _pairs.clear();

    for (auto& body_a : _bodies) {
        for (auto& body_b : _bodies) {
            // same body collision? we skip that for now. maybe later
            if (body_a.id == body_b.id) {
                continue;
            }

            const uint32_t body_pair = HashPair(body_a.id, body_b.id);

            // check collision groups for collisions
            // we allow for unique collision combinations
            // check the mask if they should interact tho
            const uint32_t a_group_len = body_a.group_ids->size();
            const uint32_t b_group_len = body_b.group_ids->size();

            // technically currently the first inserted groups in the list get priority over the others.
            // maybe add priotirty based sorting later down the line? TODO
            for (uint32_t i = 0; i < a_group_len; i++) {
                for (uint32_t j = 0; j < b_group_len; j++) {
                    const int16_t g_a_id = body_a.group_ids->get(i);
                    const int16_t g_b_id = body_b.group_ids->get(j);

                    // skip if any of the groups are disabled.
                    if (!_groups.is_enabled(g_a_id) || !_groups.is_enabled(g_b_id)) {
                        continue;
                    }

                    const auto& group_a = _groups.get(g_a_id);
                    const auto& group_b = _groups.get(g_b_id);

                    const bool matching =
                        group_a.group_layers & group_b.detect_layers ||
                        group_b.group_layers & group_a.detect_layers;

                    // skip if these groups dont interact
                    if (!matching) {
                        continue;
                    }

                    const uint32_t group_pair = HashPair(g_a_id, g_b_id);

                    // we know these groups might interact.
                    // so first check if these groups are already interacting
                    bool skip_pair = false;
                    for (uint32_t i = 0; i < _pairs.size(); i++) {
                        if (_pairs[i].groups_hash == group_pair) {
                            skip_pair = true;
                            break;
                        }
                    }

                    if (skip_pair) {
                        continue;
                    }

                    // now we know the pair hasnt been found before
                    // lets see if they interact with eachother
                    CPair pair{};
                    pair.info.collided = false;
                    DoGroupsCollide(pair, body_a, body_b, group_a, group_b);

                    // if the group didnt collide move on
                    if (!pair.info.collided) {
                        continue;
                    }

                    pair.bodies_hash = body_pair;
                    pair.groups_hash = group_pair;

                    // add collision pair and move on to the next steps
                    _pairs.push_back(pair);
                }
            }
        }
    }
}

void Gekko::Physics::World::ResolvePairs()
{
    // todo
}

void Gekko::Physics::World::ReactPairs()
{
    // todo
}

void Gekko::Physics::World::IntegrateBodies()
{
    // todo
}

uint32_t Gekko::Physics::World::HashPair(int16_t a, int16_t b)
{
    // map int16_t (range: -32768 to 32767) to uint16_t (0 to 65535)
    uint16_t u_a = static_cast<uint16_t>(a + 32768);
    uint16_t u_b = static_cast<uint16_t>(b + 32768);

    // keep the order consistent
    if (u_a > u_b) {
        std::swap(u_a, u_b);
    }

    // pack the two uint16_t values into a uint32_t
    return (static_cast<uint32_t>(u_a) << 16) | u_b;
}

void Gekko::Physics::World::UnhashPair(uint32_t hash, int16_t& a, int16_t& b)
{
    a = (hash >> 16) - 32768;
    b = (hash & 0xFFFF) - 32768;
}

bool Gekko::Physics::World::HashContainsId(uint32_t hash, int16_t id)
{
    int16_t a, b;
    UnhashPair(hash, a, b);
    return a == id || b == id;
}

void Gekko::Physics::World::DoGroupsCollide(
    CPair& info,
    const Body& body_a, const Body& body_b,
    const ObjectGroup& group_a, const ObjectGroup& group_b)
{
    // technically currently the first inserted objects in the list get priority over the others.
    // maybe add priotirty based sorting later down the line? TODO
    const uint32_t a_obj_len = group_a.object_ids->size();
    const uint32_t b_obj_len = group_b.object_ids->size();

    for (uint32_t i = 0; i < a_obj_len; i++) {
        for (uint32_t j = 0; j < b_obj_len; j++) {
            // figure out the collison function and sort by the type
            const Object* a_obj = &_objects.get(group_a.object_ids->get(i));
            const Object* b_obj = &_objects.get(group_b.object_ids->get(j));

            const Body* b_a = &body_a;
            const Body* b_b = &body_b;

            // keep common order
            if (a_obj->type > b_obj->type) {
                std::swap(a_obj, b_obj);
                std::swap(b_a, b_b);
            }

            const uint16_t combined_type = a_obj->type | b_obj->type;

            switch (combined_type) {
            case Object::Sphere | Object::Sphere:
                CheckSphereSphere(info.info, a_obj, b_obj, b_a, b_b);
                break;
            case Object::Sphere | Object::Capsule:
                CheckSphereCapsule(info.info, a_obj, b_obj, b_a, b_b);
                break;
            case Object::Capsule | Object::Capsule:
                CheckCapsuleCapsule(info.info, a_obj, b_obj, b_a, b_b);
                break;
            default:
                break;
            };

            // collision found? stop checking the groups against eachother and early return.
            if (info.info.collided) {
                return;
            }
        }
    }
}

void Gekko::Physics::World::CheckSphereSphere(
    CInfo& info,
    const Object* obj_a, const Object* obj_b,
    const Body* body_a, const Body* body_b)
{
    auto& sphere_a = _spheres.get(obj_a->shape_id);
    auto& sphere_b = _spheres.get(obj_b->shape_id);

    // TODO when i get to body rotation handle it properly by applying the transforms.
    Math::Vec3 wpos_a = _origin + body_a->position;
    Math::Vec3 wpos_b = _origin + body_b->position;

    Math::Vec3 diff = (wpos_a + sphere_a.position) - (wpos_b + sphere_b.position);

    Math::Unit distSq = diff.Dot(diff);
    Math::Unit radSum = sphere_a.radius + sphere_b.radius;
    Math::Unit radSumSq = radSum * radSum;

    info.collided = distSq >= radSumSq;

    // quit early if theres no collision
    if (!info.collided) {
        return;
    }
     
    CalculateDepthNorm(info, distSq, radSum, diff);
}

void Gekko::Physics::World::CheckSphereCapsule(
    CInfo& info,
    const Object* obj_a, const Object* obj_b,
    const Body* body_a, const Body* body_b)
{
    auto& sphere = _spheres.get(obj_a->shape_id);
    auto& capsule = _capsules.get(obj_b->shape_id);
}

void Gekko::Physics::World::CheckCapsuleCapsule(
    CInfo& info,
    const Object* obj_a, const Object* obj_b,
    const Body* body_a, const Body* body_b)
{
    auto& cap_a = _capsules.get(obj_a->shape_id);
    auto& cap_b = _capsules.get(obj_b->shape_id);
}

void Gekko::Physics::World::CalculateDepthNorm(
    CInfo& info,
    const Math::Unit& distSq,
    const Math::Unit& radSum,
    const Math::Vec3& diff)
{
    static const auto def_norm = Math::Vec3(Math::Unit::ONE, Math::Unit(), Math::Unit());
    static const auto zero = Math::Vec3(Math::Unit(), Math::Unit(), Math::Unit());

    // compute real distance
    const Math::Unit distance = Math::Unit::SqrtNewton(distSq);

    // compute normal
    info.normal = distance > 0 ? diff / distance : def_norm;

    // handle zero case
    if (info.normal == zero) {
        info.normal = def_norm;
    }

    // compute pen depth
    info.depth = radSum - distance;
}
