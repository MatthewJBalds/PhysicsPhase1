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

    void ParticleSystem::Update(float deltaTime) {
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [this, deltaTime](Particle& p) {
                    p.lifetime -= deltaTime;
                    if (p.lifetime <= 0) {
                        world->RemoveParticle(&p.physics);
                        return true;
                    }
                    return false;
                }),
            particles.end()
        );
    }

    void ParticleSystem::Render(const glm::mat4& view, const glm::mat4& projection) {
        for (auto& particle : particles) {
            particle.visual.SetPosition(particle.physics.Position);
            particle.visual.Render(view, projection);
        }
    }

    void ParticleSystem::SpawnParticle() {
        particles.emplace_back("3D/sphere.obj", *shader);
        Particle& p = particles.back();

        // Physics setup - all particles start at same point
        p.physics.Position = MyVector(0, 0, 0);
        p.physics.mass = 1.0f;
        p.physics.Damping = 0.9f;
        p.physics.Velocity = MyVector(0, 0, 0);
        p.physics.ResetForce();

        // Generate random spherical direction
        float theta = angleDist(gen); // Horizontal angle
        float phi = angleDist(gen) * 0.25f; // Vertical angle 
        float forceMagnitude = forceDist(gen);

        // Calculate force direction
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);
        MyVector forceDirection(
            sinPhi * cos(theta) * 0.3f,  // Reduced horizontal
            cosPhi * 1.5f,               // Boosted vertical
            sinPhi * sin(theta) * 0.3f   // Reduced depth
        );

        // Apply force
        p.physics.AddForce(forceDirection * forceMagnitude);

        // Visual properties
        float size = sizeDist(gen);
        p.visual.SetScale(MyVector(size, size, size));
        p.visual.SetColor(glm::vec4(GetRandomColor(), 1.0f));

        // Set lifetime
        p.maxLifetime = lifeDist(gen);
        p.lifetime = p.maxLifetime;

        world->AddParticle(&p.physics);
    }

    MyVector ParticleSystem::GetRandomForce() {
        return MyVector(
            (colorDist(gen) - 0.5f) * 100.0f,  // Small horizontal variation
            2500.0f,                           // Strong upward force
            (colorDist(gen) - 0.5f) * 100.0f    // Small depth variation
        );
    }

    glm::vec3 ParticleSystem::GetRandomColor() {
        return glm::vec3(
            colorDist(gen),
            colorDist(gen),
            colorDist(gen)
        );
    }
}