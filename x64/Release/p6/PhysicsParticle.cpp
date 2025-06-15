#include "PhysicsParticle.h"
#include <random>
#include <iostream>

using namespace Physics;

//Physics::PhysicsParticle::PhysicsParticle() {}

void PhysicsParticle::UpdatePosition(float time) {

	// p2 = p1 + V * t //must be like this (may vary based on vector class)

    // p2 = p1 + Vt + [(At^2)/2]
    this->Position = this->Position + this->Velocity * time + this->Acceleration * (0.5f * time * time);
}

void PhysicsParticle::UpdateVelocity(float time) {
    //acceleration = force/mass
    this->Acceleration += AccumulatedForce * (1 / mass);
    // v_final = v_initial + a * t
    this->Velocity = this->Velocity + (this->Acceleration * time);
    //vf * damping^time
    this->Velocity = this->Velocity * powf(this->Damping, time);
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
}