// Cable.cpp
#include "Cable.h"

namespace Physics {
    Cable::Cable(PhysicsParticle* p, const MyVector& anchor, float maxLen)
        : particle(p), anchorPoint(anchor), maxLength(maxLen) {}

    void Cable::UpdateForce(PhysicsParticle* particle, float duration) {
        // Empty - we'll handle constraints through contacts
    }

    ParticleContact* Cable::GenerateContact() {
        // Calculate current length
        MyVector direction = particle->Position - anchorPoint;
        float currentLength = direction.Magnitude();

        // If within limits, no contact needed
        if (currentLength <= maxLength) return nullptr;

        // Create contact to enforce constraint
        ParticleContact* contact = new ParticleContact();
        contact->particles[0] = particle;
        contact->contactNormal = direction.Direction();
        contact->Depth = currentLength - maxLength;
        contact->restitution = 0.0f; // Perfectly inelastic

        return contact;
    }
    
}