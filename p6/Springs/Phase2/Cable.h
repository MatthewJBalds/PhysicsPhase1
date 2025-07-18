#pragma once
#include "../../ForceGenerator.h"

namespace Physics {
    class Cable : public ForceGenerator
    {
    private:
        MyVector anchorPoint;
        float maxLength;
        float springConstant; // For applying corrective force when at max length

    public:
        Cable(MyVector pos, float _maxLength, float _springConstant = 1000.0f)
            : anchorPoint(pos), maxLength(_maxLength), springConstant(_springConstant) {}

        void UpdateForce(PhysicsParticle* particle, float time) override;
    };
}

