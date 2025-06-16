#include "ParticleSystem.h"

namespace Physics {
    ParticleSystem::ParticleSystem(Shader* shader, PhysicsWorld* world, const MyVector& spawnPoint)
        : shader(shader), world(world), spawnPoint(spawnPoint), gen(rd()),
        colorDist(0.0f, 1.0f),
        sizeDist(2.0f, 10.0f),
        lifeDist(1.0f, 10.0f),
        forceDist(1.0f, 3000.0f),
        angleDist(0.0f, 2.0f * 3.14159265f) {
    }

    //updates all particles and removes dead ones
    void ParticleSystem::Update(float deltaTime) {
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [this, deltaTime](Particle& p) {
                    p.lifetime -= deltaTime;
                    if (p.lifetime <= 0) {
                        world->RemoveParticle(&p.physics); //remove from  physics world
                        return true; //mark for deletion
                    }
                    return false;
                }),
            particles.end()
        );
    }

    //renderingg of all active particles
    void ParticleSystem::Render(const glm::mat4& view, const glm::mat4& projection) {
        for (auto& particle : particles) {
            particle.visual.SetPosition(particle.physics.Position);
            particle.visual.Render(view, projection);
        }
    }

    //creates and initializes a new particle
    void ParticleSystem::SpawnParticle() {
        particles.emplace_back("3D/sphere.obj", *shader);
        Particle& p = particles.back();

        //initialize properties for physics
        p.physics.Position = MyVector(0, 0, 0);
        p.physics.mass = 1.0f;
        p.physics.Damping = 0.9f;
        p.physics.Velocity = MyVector(0, 0, 0);
        p.physics.ResetForce();

        //calculate random direction for force
        float theta = angleDist(gen);
        float phi = angleDist(gen) * 0.25f;
        float forceMagnitude = forceDist(gen);

        //onvert spherical coordinates to force vector
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);
        MyVector forceDirection(
            sinPhi * cos(theta) * 0.3f,
            cosPhi * 1.5f,
            sinPhi * sin(theta) * 0.3f
        );

        p.physics.AddForce(forceDirection * forceMagnitude); //applying of force

        float size = sizeDist(gen);
        p.visual.SetScale(MyVector(size, size, size));
        p.visual.SetColor(glm::vec4(GetRandomColor(), 1.0f));

        // Set lifetime
        p.maxLifetime = lifeDist(gen);
        p.lifetime = p.maxLifetime;

        world->AddParticle(&p.physics);
    }

    //generates a random force vector (mostly upward)
    MyVector ParticleSystem::GetRandomForce() {
        return MyVector(
            (colorDist(gen) - 0.5f) * 100.0f,
            2500.0f,
            (colorDist(gen) - 0.5f) * 100.0f
        );
    }

    //generates a random RGB color
    glm::vec3 ParticleSystem::GetRandomColor() {
        return glm::vec3(
            colorDist(gen), //red
            colorDist(gen), //green
            colorDist(gen)  //blue
        );
    }
}