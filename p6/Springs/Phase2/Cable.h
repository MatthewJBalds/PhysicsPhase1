#pragma once
#include "../ParticleLink.h"
#include "../../MyVector.h"
#include "../../ParticleContact.h"
#include "../../PhysicsParticle.h"

namespace Physics {
    class Cable : public ParticleLink
    {
    private:
        PhysicsParticle* particle;
        MyVector anchorPoint;
        float maxLength;
        float restitution;


    public:
        Cable(const MyVector& anchor, float maxLen, float rest = 0.0f)
            : anchorPoint(anchor), maxLength(maxLen), restitution(rest) {}

        // Override GetContact to use Rod-like constraint logic
        virtual ParticleContact* GetContact() override;

        void Update(float dt);
    };
}

