#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>

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
        : physics(), visual(modelPath, shader), lifetime(0), maxLifetime(0), color(1.0f) {
    }

    Particle(Particle&& other) noexcept = default;
    Particle& operator=(Particle&& other) noexcept = default;
    Particle(const Particle&) = delete;
    Particle& operator=(const Particle&) = delete;
};

int getMaxParticlesFromUser() {
    int maxParticles;
    std::cout << "Enter number of particles to spawn : ";
    while (!(std::cin >> maxParticles) || maxParticles < 0 || maxParticles > 10000) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "???";
    }
    return maxParticles;
}

// Camera variables
bool isPerspective = false;
bool isPaused = false;
float cameraDistance = 80.0f;
float cameraRotationX = 0.0f;
float cameraRotationY = 0.0f;

// Key callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const float rotationSpeed = 0.05f;
    const float maxVerticalAngle = glm::radians(89.0f); 

    // Handle inputs for camera
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_1:
            isPerspective = false; // Switch to orthographic
            std::cout << "Switched to orthographic view" << std::endl;
            break;
        case GLFW_KEY_2:
            isPerspective = true; // Switch to perspective
            std::cout << "Switched to perspective view" << std::endl;
            break;
        case GLFW_KEY_SPACE:
            isPaused = !isPaused;
            std::cout << (isPaused ? "Simulation PAUSED" : "Simulation RESUMED") << std::endl;
            break;
        }
    }

    // Handle continuous movement of camera
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W:
            cameraRotationY += rotationSpeed; // Look up
            cameraRotationY = glm::clamp(cameraRotationY, -maxVerticalAngle, maxVerticalAngle);
            break;
        case GLFW_KEY_S:
            cameraRotationY -= rotationSpeed; // Look down
            cameraRotationY = glm::clamp(cameraRotationY, -maxVerticalAngle, maxVerticalAngle);
            break;
        case GLFW_KEY_A:
            cameraRotationX -= rotationSpeed; // Look left
            break;
        case GLFW_KEY_D:
            cameraRotationX += rotationSpeed; // Look right
            break;
        }
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) return -1;

    // Window setup
    GLFWwindow* window = glfwCreateWindow(800, 800, "Group 5 - YN-GINE", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    // Set key callback
    glfwSetKeyCallback(window, key_callback);

    // Get user input
    const int maxParticles = getMaxParticlesFromUser();

    // Setup
    Shader shader("Shaders/Sample.vert", "Shaders/Sample.frag");
    PhysicsWorld pWorld;
    std::vector<Particle> particles;
    particles.reserve(maxParticles); // Reserve memory for particles to avoid reallocations
    MyVector spawnPoint(0, -30, 0);

    // Random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> hueDist(0.0f, 1.0f);  // For HSV color
    std::uniform_real_distribution<float> sizeDist(1.0f, 5.0f);
    std::uniform_real_distribution<float> lifeDist(1.0f, 10.0f);
    std::uniform_real_distribution<float> forceDist(800.0f, 1200.0f);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(0.0f, 10.0f);

    // Camera
    glm::mat4 projection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, -80.0f, 80.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Drag force generator
    DragForceGenerator dragForce(0.2f, 0.01f);

    // Timing
    using clock = std::chrono::high_resolution_clock;
    auto prev_time = clock::now();
    float spawnTimer = 0.0f;
    const float spawnInterval = 0.02f;
    bool ParticleStart = false;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Time calculation
        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - prev_time).count();
        prev_time = now;

        // Update camera matrices
        glm::mat4 projection;
        if (isPerspective) {
            // Perspective projection
            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 500.0f);
        }
        else {
            // Orthographic projection
            projection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, -80.0f, 500.0f);
        }

        // Calculate camera position based on rotation with height adjustment
        float cosY = cos(cameraRotationY);
        float camX = sin(cameraRotationX) * cosY * cameraDistance;
        float camZ = cos(cameraRotationX) * cosY * cameraDistance;
        float camY = sin(cameraRotationY) * cameraDistance;

        // Update view matrix
        glm::vec3 cameraPos = glm::vec3(camX, camY, camZ);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        view = glm::lookAt(cameraPos, target, up);

        // Check if eruption is complete and particles are gone
        if (ParticleStart && particles.empty()) {
            std::cout << "Restarting" << std::endl;
            ParticleStart = false;
        }

        // Only update physics if not paused
        if (!isPaused) {
            // Spawn new particles
            if (!ParticleStart) {
                spawnTimer += deltaTime;
                if (spawnTimer >= spawnInterval && particles.size() < maxParticles) {
                    spawnTimer = 0.0f;

                    particles.emplace_back("3D/sphere.obj", shader);
                    Particle& p = particles.back();

               

                    // All particles start at exact same point
                    p.physics.Position = MyVector(0, -80, 0);
                    p.physics.mass = 1.0f;
                    p.physics.Damping = 0.9f;
                    p.physics.Velocity = MyVector(0, 0, 0); // Reset velocity

                    // Generate random spherical direction with STRONG upward bias
                    float theta = angleDist(gen); // Full horizontal circle (0-2π)
                    float phi = angleDist(gen) * 0.25f; // Limited vertical angle (0-π/5)

                    // Convert to force direction - much stronger upward component
                    float forceMagnitude = 3000.0f + forceDist(gen); // 3000-4200 range
                    float sinPhi = sin(phi);
                    float cosPhi = cos(phi);
                    float sinTheta = sin(theta);
                    float cosTheta = cos(theta);

                    // Force vector with 5x stronger upward bias
                    MyVector forceDirection(
                        sinPhi * cosTheta * 0.4f,  // Reduced horizontal
                        cosPhi * 2.0f,             // Boosted vertical (2x)
                        sinPhi * sinTheta * 0.4f   // Reduced depth
                    );

                    p.physics.AddForce(forceDirection * forceMagnitude);

                    // Visual properties
                    float size = sizeDist(gen);
                    p.visual.SetScale(MyVector(size, size, size));
                    p.color = glm::vec3(hueDist(gen), hueDist(gen), hueDist(gen));
                    p.visual.SetColor(glm::vec4(p.color, 1.0f));

                    p.maxLifetime = lifeDist(gen);
                    p.lifetime = p.maxLifetime;

                    pWorld.AddParticle(&p.physics);
                }
                else if (particles.size() >= maxParticles) {
                    ParticleStart = true;
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
        }

        // Render particles (always render, even when paused)
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