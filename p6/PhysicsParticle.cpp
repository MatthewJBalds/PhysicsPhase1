#include "PhysicsParticle.h"
#include <random>
#include <iostream>

using namespace Physics;

//Physics::PhysicsParticle::PhysicsParticle() {}

void PhysicsParticle::UpdatePosition(float time) {

	// p2 = p1 + V * t //must be like this (may vary based on vector class)

    // p2 = p1 + Vt + [(At^2)/2]
    this->Position = this->Position + this->Velocity * time + this->Acceleration * (0.5f * time * time);

    //angular velocity
    //MyVector angularV = AngularVelocity * time;
    //float angleMag = angularV.Magnitude();
    //MyVector MagDir = angularV.Direction(); //numralize
    //if (angleMag != 0) {
    //    glm::quat rotBy = glm::rotate(
    //        glm::mat4(1.f),
    //        angleMag,
    //        (glm::vec3)MagDir
    //    );
    //    this->Rotation = glm::toMat4(glm::toQuat(this->Rotation) * rotBy);
    //}

}

void PhysicsParticle::UpdateVelocity(float time) {
    //acceleration = force/mass
    if (mass != 0.0f) {
        this->Acceleration += AccumulatedForce * (1 / mass);
    }
    // v_final = v_initial + a * t
    this->Velocity = this->Velocity + (this->Acceleration * time);
    //vf * damping^time
    this->Velocity = this->Velocity * powf(this->Damping, time);

    ////get moment of inetria
    //float mI = MomentOfInertia();
    ////aV = (Torque * time) /MoI
    //AngularVelocity +=accumulatedTorque * time * ((float)1 / mI);
    ////add in damening similar to position velocity
    //AngularVelocity = AngularVelocity * powf(AngularDampening, time);
}

void PhysicsParticle::Update(float time) {

    //pos first
    this->UpdatePosition(time);
    //velocity next
    this->UpdateVelocity(time);
    //reset
    this->ResetForce();
     
}

void PhysicsParticle::Destroy() {
    this->isDestroyed = true;
}

void PhysicsParticle::AddForce(MyVector force) {
    this->AccumulatedForce += force;
}

void PhysicsParticle::ResetForce() {
    this->AccumulatedForce = MyVector(0, 0, 0);
    this->Acceleration = MyVector(0, 0, 0);
    //this->accumulatedTorque = MyVector(0, 0, 0);
}

//float PhysicsParticle::MomentOfInertia() {
//    return((float)2 / 5) * mass * radius * radius;
//}
//
//void PhysicsParticle::AddForceAtPoint(MyVector force, MyVector p) {
//    this->AddForce(force);
//    //torque = PXF
//    this->accumulatedTorque = p.Cross(force);
//}

//PhysicsParticle::PhysicsParticle(const MyVector& position, float mass, float radius)
//    : Position(position), mass(mass), radius(radius)
//{
//    Velocity = MyVector(0, 0, 0);
//    Acceleration = MyVector(0, 0, 0);
//    Damping = 0.99f;
//    AngularVelocity = MyVector(0, 0, 0);
//    AngularDampening = 0.99f;
//    accumulatedTorque = MyVector(0, 0, 0);
//    isDestroyed = false;
//    // Initialize other members as needed
//}
