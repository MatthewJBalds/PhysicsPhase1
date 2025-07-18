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
#include "p6/ForceGenerator.h"
#include "p6/ParticleContact.h"
#include "p6/ContactResolver.h"
#include "p6/Springs/AnchoredSpring.h"
#include "p6/Springs/ParticleSpring.h"
#include "p6/Springs/ParticleLink.h"
#include "p6/Springs/Rod.h"
#include "p6/Springs/Assignment3/Bungee.h"
#include "p6/Springs/Assignment3/Chain.h"
#include "p6/Springs/RenderLine.h"
#include "p6/Springs/Phase2/Cable.h"

using namespace Physics;

// Window dimensions and screen bounds
const int windowWidth = 800;
const int windowHeight = 800;
const float screenLeft = -400.0f;
const float screenRight = 400.0f;

int main() {
    // Get user input for params
    float cableLength, particleGap, particleRadius, gravityStrength;
    float forceX, forceY, forceZ;

    std::cout << "Cable Length: ";
    std::cin >> cableLength;

    std::cout << "Particle Gap: ";
    std::cin >> particleGap;

    std::cout << "Particle Radius: ";
    std::cin >> particleRadius;

    std::cout << "Gravity Strength: ";
    std::cin >> gravityStrength;

    std::cout << "Apply Force:\n";
    std::cout << "x: ";
    std::cin >> forceX;
    std::cout << "y: ";
    std::cin >> forceY;
    std::cout << "z: ";
    std::cin >> forceZ;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Particle Chain", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Create shader
    Shader shader("Shaders/sample.vert", "Shaders/sample.frag");

    // Create game objects for 5 spheres
    std::vector<std::unique_ptr<GameObject>> spheres;
    for (int i = 0; i < 5; i++) {
        spheres.push_back(std::make_unique<GameObject>("3D/sphere.obj", shader));
        spheres[i]->SetColor(glm::vec3(1.f, 0.f, 0.f)); // Red spheres
    }

    // Line renderer for cables
    Physics::RenderLine lineRenderer;

    // Physics World
    Physics::PhysicsWorld pWorld;
    //pWorld.GetContactResolver().SetIterations(10);

    // Create gravity force generator
    Physics::GravityForceGenerator* gravity = new Physics::GravityForceGenerator(MyVector(0, -gravityStrength, 0));

    // Create 5 particles and cables
    std::vector<Physics::PhysicsParticle> particles(5);
    std::vector<Physics::Cable*> cables(5);

    // Calculate starting positions
    float startX = -(particleGap * 2); // Center the chain
    float anchorY = 400.0f; // Height of anchor points
    float startY = anchorY - cableLength; // Initial particle positions


    for (int i = 0; i < 5; i++) {
        // Set up particle
        particles[i].Position = MyVector(startX + (i * particleGap), startY, 0);
        particles[i].mass = 50;
        particles[i].radius = particleRadius;
        particles[i].restitution = 0.9f;
        particles[i].Velocity = MyVector(0, 0, 0);

        // Add particle to world
        pWorld.AddParticle(&particles[i]);

        MyVector anchorPoint(startX + (i * particleGap), anchorY, 0);
        cables[i] = new Physics::Cable(&particles[i], anchorPoint, cableLength);
        // Add forces to registry
        //pWorld.forceRegistry.Add(&particles[i], cables[i]);
        pWorld.forceRegistry.Add(&particles[i], gravity);

        // Set sphere scale
        spheres[i]->SetScale(MyVector(particleRadius, particleRadius, particleRadius));
    }

    // Apply initial force to leftmost particle
    particles[0].AddForce(MyVector(forceX, forceY, forceZ));

    // Time tracking
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    // Camera setup
    float viewWidth = screenRight - screenLeft + 100.0f;
    float viewHalfHeight = viewWidth / 2.0f;
    float viewCenterY = 225.0f;

    glm::mat4 projection = glm::ortho(
        screenLeft - 50.0f, screenRight + 50.0f,
        viewCenterY - viewHalfHeight, viewCenterY + viewHalfHeight,
        -500.0f, 100.0f
    );
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark background

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Time management
        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;
        curr_ns += dur;

        // Fixed timestep physics update
        // In your main loop, modify the physics update section:
        // In your physics update loop:
        while (curr_ns >= timestep) {
            // Generate contacts from cables
            std::vector<Physics::ParticleContact*> contacts;
            for (auto cable : cables) {
                if (Physics::ParticleContact* contact = cable->GenerateContact()) {
                    contacts.push_back(contact);
                }
            }

            // Add ground contact checks
            for (auto& particle : particles) {
                if (particle.Position.y < 0) {  // Assuming ground is at y=0
                    Physics::ParticleContact* groundContact = new Physics::ParticleContact();
                    groundContact->particles[0] = &particle;
                    groundContact->contactNormal = MyVector(0, 1, 0); // Pushes upward
                    groundContact->Depth = -particle.Position.y;
                    groundContact->restitution = 0.7f; // Some bounce
                    contacts.push_back(groundContact);
                }
            }

            // Resolve all contacts
            if (!contacts.empty()) {
                pWorld.GetContactResolver().ResolveContacts(
                    contacts,
                    (float)std::chrono::duration_cast<std::chrono::milliseconds>(timestep).count() / 1000
                );
            }

            // Clean up contacts
            for (auto contact : contacts) {
                delete contact;
            }

            // Regular physics update
            pWorld.Update((float)std::chrono::duration_cast<std::chrono::milliseconds>(timestep).count() / 1000);
            curr_ns -= timestep;
        }

        // Render all spheres
        for (int i = 0; i < 5; i++) {
            spheres[i]->SetPosition(particles[i].Position);
            spheres[i]->Render(view, projection);
        }

        // Draw cables
        shader.Use();
        shader.SetMat4("mvp", projection * view * glm::mat4(1.0f));
        shader.SetVec3("objectColor", glm::vec3(0.3f, 0.3f, 0.3f)); // Dark gray cables

        for (int i = 0; i < 5; i++) {
            glm::vec3 anchorPos(startX + (i * particleGap), anchorY, 0);
            glm::vec3 particlePos(particles[i].Position.x, particles[i].Position.y, particles[i].Position.z);
            lineRenderer.Draw(anchorPos, particlePos);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    for (auto cable : cables) {
        delete cable;
    }
    delete gravity;

    glfwTerminate();
    return 0;
}

