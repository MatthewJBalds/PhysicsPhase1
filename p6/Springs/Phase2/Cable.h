// Cable.h
#pragma once
#include "../../ForceGenerator.h"
#include "../../PhysicsParticle.h"
#include "../../ParticleContact.h"

namespace Physics {
    class Cable : public ForceGenerator {
    public:
        Cable(PhysicsParticle* particle, const MyVector& anchor, float maxLen);

        // Force generator implementation
        virtual void UpdateForce(PhysicsParticle* particle, float duration) override;

        // Contact generator implementation
        ParticleContact* GenerateContact();

    private:
        PhysicsParticle* particle;  // Store reference to the particle
        MyVector anchorPoint;
        float maxLength;
    };
}