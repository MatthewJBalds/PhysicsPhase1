#include "ParticleSystem.h"

namespace Physics {
    ParticleSystem::ParticleSystem(Shader* shader, PhysicsWorld* world,
        const MyVector& spawnPoint, int maxParticles)
        : shader(shader), world(world), spawnPoint(spawnPoint),
        maxParticles(maxParticles), gen(rd()),
        colorDist(0.0f, 1.0f),
        sizeDist(2.0f, 10.0f),
        lifeDist(1.0f, 2.0f),
        forceDist(500.0f, 1500.0f) {}

    void ParticleSystem::Update(float deltaTime) {
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [this, deltaTime](Particle& p) {
                    p.lifetime -= deltaTime;
                    if (p.lifetime <= 0) {
                        world->RemoveParticle(&p.physics);
                        return true;
                    }
                    // Fade out
                    float alpha = p.lifetime / p.maxLifetime;
                    p.visual.SetColor(glm::vec4(1, 1, 1, alpha));
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
        if (particles.size() >= maxParticles) return;

        // Construct in-place
        particles.emplace_back("3D/sphere.obj", *shader);
        Particle& newParticle = particles.back();

        newParticle.physics.Position = spawnPoint;
        newParticle.physics.mass = sizeDist(gen);
        newParticle.physics.AddForce(GetRandomForce());

        // Visual properties
        float size = newParticle.physics.mass;
        newParticle.visual.SetScale(MyVector(size, size, size));
        newParticle.visual.SetColor(glm::vec4(GetRandomColor(), 1.0f));

        // Lifetime
        newParticle.maxLifetime = lifeDist(gen);
        newParticle.lifetime = newParticle.maxLifetime;

        world->AddParticle(&newParticle.physics);
    }

    MyVector ParticleSystem::GetRandomForce() {
        return MyVector(
            forceDist(gen) * 0.2f - 100.0f,
            forceDist(gen),
            0
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