#include "Cable.h"

namespace Physics {
    Cable::Cable(const MyVector& anchor, float maxLen)
        : anchorPoint(anchor), maxLength(maxLen) {}

    void Cable::UpdateForce(PhysicsParticle* particle, float duration) {
        // Calculate current vector from anchor to particle
        MyVector direction = particle->Position - anchorPoint;
        float currentLength = direction.Magnitude();

        // If cable is stretched beyond max length
        if (currentLength > maxLength) {
            // Normalize direction vector
            direction = direction * (1.0f / currentLength);

            // Calculate how much we're over-extended
            float overExtension = currentLength - maxLength;

            // Calculate position correction
            MyVector correction = direction * (-overExtension);

            // Immediately correct position (teleport to max length)
            particle->Position = anchorPoint + direction * maxLength;

            // Apply velocity correction (remove radial velocity component)
            float radialVelocity = particle->Velocity.Dot(direction);
            particle->Velocity = particle->Velocity - direction * radialVelocity;
        }
    }
}