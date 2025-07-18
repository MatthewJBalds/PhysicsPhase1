#include "Cable.h"

void Physics::Cable::UpdateForce(PhysicsParticle* particle, float time)
{
    MyVector pos = particle->Position;
    MyVector direction = pos - anchorPoint;
    float currentLength = direction.Magnitude();

    // Only apply force if the cable is at or beyond its maximum length
    if (currentLength >= maxLength) {
        // Calculate constraint force to keep particle at max length
        direction = direction.Direction();

        // Apply a strong spring force to prevent exceeding max length
        float extension = currentLength - maxLength;
        float constraintForce = -springConstant * extension;

        MyVector force = direction * constraintForce;
        particle->AddForce(force);
    }
}
