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
}

    