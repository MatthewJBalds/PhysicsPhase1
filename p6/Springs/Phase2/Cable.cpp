#include "Cable.h"

namespace Physics {
    // Constructors
    Cable::Cable(PhysicsParticle* p1, PhysicsParticle* p2, float maxLen, float rest)
        : maxLength(maxLen), restitution(rest) {
        particles[0] = p1;
        particles[1] = p2;
    }

    Cable::Cable(PhysicsParticle* p, const MyVector& anchor, float maxLen, float rest)
        : maxLength(maxLen), restitution(rest), anchorPoint(anchor) {
        particles[0] = p;
        particles[1] = nullptr;
    }

    float Cable::CurrentLength() {
        if (IsAnchored()) {
            return (anchorPoint - particles[0]->Position).Magnitude();
        }
        return (particles[1]->Position - particles[0]->Position).Magnitude();
    }

    
}