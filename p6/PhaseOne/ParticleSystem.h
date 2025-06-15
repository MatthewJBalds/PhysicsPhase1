#pragma once
#include "../../GameObject.h"
#include "../PhysicsParticle.h"
#include "../PhysicsWorld.h"
#include <vector>
#include <random>
#include <algorithm>

namespace Physics {
    class ParticleSystem {
    public:
        struct Particle {
            PhysicsParticle physics;
            GameObject visual;
            float lifetime;
            float maxLifetime;

            // Constructor
            Particle(const std::string& modelPath, Shader& shader)
                : visual(modelPath, shader), lifetime(0), maxLifetime(0) {}

            // Move constructor and assignment
            Particle(Particle&&) = default;
            Particle& operator=(Particle&&) = default;  // <-- ADD THIS

            // Delete copy
            Particle(const Particle&) = delete;
            Particle& operator=(const Particle&) = delete;
        };

        ParticleSystem(Shader* shader, PhysicsWorld* world,
            const MyVector& spawnPoint, int maxParticles = 1000);

        void Update(float deltaTime);
        void Render(const glm::mat4& view, const glm::mat4& projection);
        void SpawnParticle();

    private:
        Shader* shader;
        PhysicsWorld* world;
        MyVector spawnPoint;
        std::vector<Particle> particles;
        int maxParticles;

        // Random number generation
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> colorDist;
        std::uniform_real_distribution<float> sizeDist;
        std::uniform_real_distribution<float> lifeDist;
        std::uniform_real_distribution<float> forceDist;

        MyVector GetRandomForce();
        glm::vec3 GetRandomColor();
    };
}