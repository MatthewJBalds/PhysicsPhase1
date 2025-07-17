#pragma once
#include "../ParticleLink.h"
#include "../../PhysicsParticle.h"
#include "../../ParticleContact.h"
#include "../../MyVector.h"

namespace Physics {
    class Cable : public ParticleLink {
    public:
        // Constructors
        Cable(PhysicsParticle* particle1, PhysicsParticle* particle2,
            float maxLen, float rest = 0.9f);
        Cable(PhysicsParticle* particle, const MyVector& anchor,
            float maxLen, float rest = 0.9f);

        // Getters
        float GetMaxLength() const { return maxLength; }
        float GetRestitution() const { return restitution; }
        bool IsAnchored() const { return particles[1] == nullptr; }

    protected:
        float CurrentLength();

    private:
        float maxLength;
        float restitution;
        MyVector anchorPoint;
    };
}