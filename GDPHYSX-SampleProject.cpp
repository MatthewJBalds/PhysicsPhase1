#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <vector>

//time
#include <chrono>
using namespace std::chrono_literals;
//time inbetween frames
constexpr std::chrono::nanoseconds timestep(16ms);

//own includes
#include "GameObject.h"
#include "Shader.h"

//physics engine includes
#include "p6/MyVector.h"
#include "p6/PhysicsParticle.h"
#include "p6/PhysicsWorld.h"
#include "p6/ForceGenerator.h"
#include "p6/ForceRegistry.h"
#include "p6/GravityForceGenerator.h"
#include "p6/DragForceGenerator.h"
#include "p6/PhaseOne/ParticleSystem.h"

using namespace Physics;

struct Particle {
    PhysicsParticle physics;
    GameObject visual;
    float lifetime;
    float maxLifetime;
    glm::vec3 color;

    Particle(const std::string& modelPath, Shader& shader)
        : physics(), visual(modelPath, shader), lifetime(0), maxLifetime(0), color(1.0f) {}

    Particle(Particle&& other) noexcept = default;
    Particle& operator=(Particle&& other) noexcept = default;
    Particle(const Particle&) = delete;
    Particle& operator=(const Particle&) = delete;
};

int getMaxParticlesFromUser() {
    int maxParticles;
    std::cout << "Enter maximum number of particles to spawn (100-5000): ";
    while (!(std::cin >> maxParticles) || maxParticles < 100 || maxParticles > 5000) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number between 100 and 5000: ";
    }
    return maxParticles;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) return -1;

    // Window setup
    GLFWwindow* window = glfwCreateWindow(800, 800, "Volcano Eruption", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    // Get user input
    const int maxParticles = getMaxParticlesFromUser();

    // Setup
    Shader shader("Shaders/Sample.vert", "Shaders/Sample.frag");
    PhysicsWorld pWorld;
    std::vector<Particle> particles;
    MyVector spawnPoint(0, -50, 0);

    // Random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> hueDist(0.0f, 1.0f);  // For HSV color
    std::uniform_real_distribution<float> sizeDist(1.0f, 5.0f);
    std::uniform_real_distribution<float> lifeDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> forceDist(800.0f, 1200.0f);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(0.0f, 10.0f);

    // Camera
    glm::mat4 projection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, -80.0f, 80.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Timing
    using clock = std::chrono::high_resolution_clock;
    auto prev_time = clock::now();
    float spawnTimer = 0.0f;
    const float spawnInterval = 0.02f;
    bool eruptionComplete = false;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Time calculation
        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - prev_time).count();
        prev_time = now;

        // Check if eruption is complete and particles are gone
        if (eruptionComplete && particles.empty()) {
            std::cout << "Eruption complete! Starting new eruption..." << std::endl;
            eruptionComplete = false;
        }

        // Spawn new particles
        if (!eruptionComplete) {
            spawnTimer += deltaTime;
            if (spawnTimer >= spawnInterval && particles.size() < maxParticles) {
                spawnTimer = 0.0f;

                particles.emplace_back("3D/sphere.obj", shader);
                Particle& p = particles.back();

                // Physics properties - volcano eruption pattern
                float angle = angleDist(gen);
                float radius = radiusDist(gen);
                p.physics.Position = MyVector(spawnPoint.x + radius * cos(angle),
                    spawnPoint.y,
                    spawnPoint.z + radius * sin(angle));
                p.physics.mass = sizeDist(gen);

                // Strong upward force with slight variation
                float forceVariation = forceDist(gen) * 0.1f;
                p.physics.AddForce(MyVector(forceVariation - 50.0f,
                    forceDist(gen) + 500.0f,
                    forceVariation - 50.0f));

                // Random vibrant color (HSV -> RGB)
                float hue = hueDist(gen);
                p.color = glm::vec3(
                    hueDist(gen),  //R
                    hueDist(gen),   //G
                    hueDist(gen)      //B 
                );
                p.visual.SetColor(glm::vec4(p.color, 1.0f));

                // Size and lifetime
                float size = p.physics.mass;
                p.visual.SetScale(MyVector(size, size, size));
                p.maxLifetime = lifeDist(gen);
                p.lifetime = p.maxLifetime;

                pWorld.AddParticle(&p.physics);
            }
            else if (particles.size() >= maxParticles) {
                eruptionComplete = true;
            }
        }

        // Update physics
        pWorld.Update(deltaTime);

        // Update particles
        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [&](Particle& p) {
                p.lifetime -= deltaTime;
                if (p.lifetime <= 0) return true;

                // Fade out and shrink
                float lifeRatio = p.lifetime / p.maxLifetime;
                p.visual.SetColor(glm::vec4(p.color, lifeRatio));
                p.visual.SetScale(MyVector(
                    p.physics.mass * lifeRatio,
                    p.physics.mass * lifeRatio,
                    p.physics.mass * lifeRatio
                ));
                return false;
            }), particles.end());

        // Render particles
        for (auto& p : particles) {
            p.visual.SetPosition(p.physics.Position);
            p.visual.Render(view, projection);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}