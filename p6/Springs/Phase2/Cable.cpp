#include "Cable.h"

namespace Physics {
    ParticleContact* Cable::GetContact() {
        // Calculate current length from anchor to particle
        MyVector direction = particles[0]->Position - anchorPoint;
        float currentLength = direction.Magnitude();

        // Only create contact if stretched beyond max length
        if (currentLength <= maxLength) {
            return nullptr;
        }

        // Create contact to maintain maximum length (like Rod logic)
        ParticleContact* contact = new ParticleContact();
        contact->particles[0] = particles[0];
        contact->particles[1] = nullptr; // Anchor point (infinite mass)

        // Normalize direction
        if (currentLength > 0) {
            direction = direction * (1.0f / currentLength);
        }

        // Set contact normal to pull particle back to anchor
        contact->contactNormal = direction * -1;
        contact->Depth = currentLength - maxLength;
        contact->restitution = restitution;

        return contact;
    }

    void Cable::Update(float dt) {
        // Calculate current vector from anchor to particle
        MyVector toParticle = particle->Position - anchorPoint;
        float currentLength = toParticle.Magnitude();

        // Check if cable is over-extended
        if (currentLength > maxLength) {
            // Calculate correction vector
            MyVector correction = toParticle.Direction() * (currentLength - maxLength);

            // Apply position correction
            particle->Position -= correction;

            // Optional: Velocity damping to reduce oscillation
            MyVector relativeVel = particle->Velocity;
            float velAlongCable = relativeVel.Dot(toParticle.Direction());
            particle->Velocity -= toParticle.Direction() * velAlongCable * 0.1f;
        }
    }
}

    