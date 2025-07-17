#include "Chain.h"

namespace Physics
{
    void Chain::UpdateForce(PhysicsParticle* particle, float time) {
        MyVector direction = particle->Position - anchorPoint;
        float distance = direction.Magnitude();

        if (distance <= restLength) return;

        direction.Normalize();
        particle->Position = anchorPoint + direction * restLength;

        float remVel = particle->Velocity.ScalarMultiplication(direction);
        if (remVel > 0) {
            particle->Velocity = particle->Velocity - direction * remVel;
        }
    }

}
