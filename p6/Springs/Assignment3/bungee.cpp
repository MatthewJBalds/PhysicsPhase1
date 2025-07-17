#include "Bungee.h"

namespace Physics
{
    void Bungee::UpdateForce(PhysicsParticle* particle, float time) {
        MyVector force = particle->Position - anchorPoint;

        float mag = force.Magnitude();

        if (mag <= restLength || mag == 0) return;

        force.Normalize();

        float bungeeForce = -bunlen * (mag - restLength);
        force = force * bungeeForce;

        MyVector reV = particle->Velocity;
        MyVector reduceVel = reV * -damping;

        particle->AddForce(force + reduceVel);
    }

}
