#pragma once
#include "../../GameObject.h"
#include "../PhysicsParticle.h"
#include "../PhysicsWorld.h"
#include <random>

namespace Physics {
    class ParticleSystem {
    public:
        struct Particle {
            PhysicsParticle physics;
            GameObject visual;
            float lifetime;
            float maxLifetime;

            Particle(const std::string& modelPath, Shader& shader)
                : visual(modelPath, shader), lifetime(0), maxLifetime(0) {
            }
        };

        ParticleSystem(Shader* shader, PhysicsWorld* world, const MyVector& spawnPoint);
        void Update(float deltaTime);
        void Render(const glm::mat4& view, const glm::mat4& projection);
        void SpawnParticle();

    private:
        Shader* shader;
        PhysicsWorld* world;
        MyVector spawnPoint;
        std::vector<Particle> particles;


        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> colorDist;
        std::uniform_real_distribution<float> sizeDist;
        std::uniform_real_distribution<float> lifeDist;
        std::uniform_real_distribution<float> forceDist;
        std::uniform_real_distribution<float> angleDist;

        MyVector GetRandomForce();
        glm::vec3 GetRandomColor();
    };
}