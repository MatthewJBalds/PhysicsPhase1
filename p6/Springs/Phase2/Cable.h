#pragma once
#include "../ParticleLink.h"
#include "../../MyVector.h"

namespace Physics {
    class Cable : public ParticleLink
    {
    private:
        MyVector anchorPoint;
        float maxLength;
        float restitution;

    public:
        Cable(const MyVector& anchor, float maxLen, float rest = 0.0f)
            : anchorPoint(anchor), maxLength(maxLen), restitution(rest) {}

        // Override GetContact to use Rod-like constraint logic
        virtual ParticleContact* GetContact() override;
    };
}

