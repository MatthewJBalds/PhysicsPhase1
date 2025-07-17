#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

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
#include "p6/ParticleContact.h"
#include "p6/ContactResolver.h"
#include "p6/Springs/AnchoredSpring.h"
#include "p6/Springs/ParticleSpring.h"
#include "p6/Springs/Rod.h"
#include "p6/Springs/ParticleLink.h"
#include "p6/Springs/Assignment3/bungee.h"
#include "p6/Springs/Assignment3/chain.h"
#include "p6/Springs/Phase2/Cable.h"

using namespace Physics;

int main() {
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "Assignment03-MJ_Baldonado", NULL, NULL);
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
    Shader shader("Shaders/Sample.vert", "Shaders/Sample.frag");

    // Get simulation parameters from user
    float cableLength, particleGap, particleRadius, gravityStrength;
    MyVector applyForce;

    std::cout << "Enter simulation parameters:\n";
    std::cout << "Cable Length: "; std::cin >> cableLength;
    std::cout << "Particle Gap (horizontal spacing): "; std::cin >> particleGap;
    std::cout << "Particle Radius: "; std::cin >> particleRadius;
    std::cout << "Gravity Strength: "; std::cin >> gravityStrength;
    std::cout << "Apply Force (x y z): ";
    std::cin >> applyForce.x >> applyForce.y >> applyForce.z;

    //Physics World
    Physics::PhysicsWorld pWorld = Physics::PhysicsWorld();



    const int PENDULUM_COUNT = 5;
    std::vector<GameObject> spheres;
    std::vector<Physics::PhysicsParticle*> particles;
    std::vector<Physics::Cable*> cables;

    // Calculate starting positions to center the 3rd pendulum
    float totalWidth = (PENDULUM_COUNT - 1) * particleGap;
    float startX = -totalWidth / 2.0f;  // Center the group
    float anchorY = 20.0f;  // Common anchor height

    // Create pendulum systems
    for (int i = 0; i < PENDULUM_COUNT; i++) {
        float anchorX = startX + i * particleGap;

        // Create anchor point
        MyVector anchor(anchorX, anchorY, 0);

        // Create pendulum bob - positioned cableLength below anchor
        Physics::PhysicsParticle* p = new Physics::PhysicsParticle();
        p->Position = MyVector(anchorX, anchorY - cableLength, 0);
        p->radius = particleRadius; // or p->Radius if that's the correct member
        p->mass = 1.0f;
        pWorld.AddParticle(p);
        particles.push_back(p);

        // Create render object
        spheres.emplace_back("3D/sphere.obj", shader,
            glm::vec3(0.5f, 0.8f, 1.0f));
        spheres.back().SetScale(MyVector(particleRadius * 2.0f, particleRadius * 2.0f, particleRadius * 2.0f));


        // Create cable from anchor to particle
        cables.push_back(new Physics::Cable(p, anchor, cableLength));
    }

    // Add gravity
    Physics::GravityForceGenerator gravity(MyVector(0, gravityStrength, 0));
    for (auto p : particles) {
        pWorld.forceRegistry.Add(p, &gravity);
    }
    

    //initialize clock and variables
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    // Camera setup
    glm::mat4 projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -100.0f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Set window background
    //glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
    bool forceApplied = false;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Time management
        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;
        curr_ns += dur;

        if (curr_ns >= timestep) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);

            // Generate and process cable constraints
            for (auto& cable : cables) {
                if (ParticleContact* contact = cable->GetContact()) {
                    // Add contact using the correct method signature from your reference
                    pWorld.AddContact(contact->particles[0],
                        contact->particles[1],
                        contact->restitution,
                        contact->contactNormal,
                        contact->Depth);
                    delete contact; // Clean up the contact object
                }
            }

            // Apply force when space is pressed
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !forceApplied) {
                for (auto particle : particles) {
                    particle->AddForce(applyForce);
                }
                forceApplied = true;
            }

            pWorld.Update((float)ms.count() / 1000);
            curr_ns -= curr_ns;
        }

        // Rendering
        for (size_t i = 0; i < particles.size(); i++) {
            spheres[i].SetPosition(particles[i]->Position);
            spheres[i].Render(view, projection);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    for (auto p : particles) delete p;
    for (auto c : cables) delete c;
    glfwTerminate();
    return 0;
}