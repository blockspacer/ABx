#pragma once

#include <stdint.h>

namespace Math {
class BoundingBox;
class Vector3;
}

namespace Game {

class Actor;
class GameObject;

namespace Components {

/// Only an Actor can have a CollisionComp, because only moving objects need it.
class CollisionComp
{
private:
    Actor& owner_;
    void ResolveOne(const Math::BoundingBox& myBB, GameObject& other);
    void Slide(const Math::BoundingBox& myBB, GameObject& other);
    void GotoSafePosition();
    static Math::Vector3 GetBodyCenter(const Math::Vector3& pos);
public:
    CollisionComp() = delete;
    explicit CollisionComp(Actor& owner) :
        owner_(owner)
    { }
    // non-copyable
    CollisionComp(const CollisionComp&) = delete;
    CollisionComp& operator=(const CollisionComp&) = delete;
    ~CollisionComp() = default;

    void Update(uint32_t timeElapsed);
    void ResolveCollisions();
};

}
}
