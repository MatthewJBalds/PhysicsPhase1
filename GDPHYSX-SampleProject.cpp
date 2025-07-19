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

const float screenLeft = -400.0f;
const float screenRight = 400.0f;

// window dimensions
int windowWidth = 800;
int windowHeight = 800;

bool isPerspective = false;
float cameraDistance = 500.0f;
float cameraRotationX = 0.0f;
float cameraRotationY = 0.3f;
const float MAX_VERTICAL_ANGLE = glm::pi<float>() * 0.49f;

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
    std::cout << "X: ";
    std::cin >> forceX;
    std::cout << "Y: ";
    std::cin >> forceY;
    std::cout << "Z: ";
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
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "GROUP 5 - YNGINE", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialization
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
        spheres[i]->SetColor(glm::vec3(1.f, 0.f, 0.f));
    }

    Physics::RenderLine lineRenderer;

    //Physics World
    Physics::PhysicsWorld pWorld = Physics::PhysicsWorld();

    // Create gravity force generator
    Physics::GravityForceGenerator* gravity = new Physics::GravityForceGenerator(MyVector(0, -gravityStrength, 0));

    // Create 5 particles for Newton's cradle
    std::vector<Physics::PhysicsParticle> particles(5);
    std::vector<Physics::Cable*> cables(5);

    // Calculate starting positions - arrange in a line with specified gap
    float startX = -(particleGap * 2); // Center the cradle
    float anchorY = 400.0f; // Height of anchor points
    float startY = anchorY - cableLength; // Particles hang down from anchors

    for (int i = 0; i < 5; i++) {
        // Set up particle
        particles[i].Position = MyVector(startX + (i * particleGap), startY, 0);
        particles[i].mass = 50;
        particles[i].radius = particleRadius;
        particles[i].restitution = 0.9f;
        particles[i].Velocity = MyVector(0, 0, 0);

        // Add particle to world
        pWorld.AddParticle(&particles[i]);

        // Create cable constraint
        MyVector anchorPoint(startX + (i * particleGap), anchorY, 0);
        cables[i] = new Physics::Cable(anchorPoint, cableLength, 0.0f);
        cables[i]->particles[0] = &particles[i]; // Set the particle reference
        cables[i]->particles[1] = nullptr;

        // Add cable
        pWorld.AddLink(cables[i]);

        // add grav
        pWorld.forceRegistry.Add(&particles[i], gravity);

        // Set sphere scale and initial position
        spheres[i]->SetScale(MyVector(particleRadius, particleRadius, particleRadius));
    }

    //initialize clock and variables
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    // Camera setup
    float viewWidth = screenRight - screenLeft + 100.0f;
    float viewHalfHeight = viewWidth / 2.0f;
    float viewCenterY = 225.0f;



    bool spacePressed = false;
    bool forceApplied = false;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear both color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Time calculation - FIXED
        auto new_time = clock::now();
        auto frame_time = new_time - curr_time;
        curr_time = new_time;
        curr_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(frame_time);

        // Calculate camera position - MOVED BEFORE PHYSICS UPDATE
        float cosY = cos(cameraRotationY);
        float camX = sin(cameraRotationX) * cosY * cameraDistance;
        float camZ = cos(cameraRotationX) * cosY * cameraDistance;
        float camY = sin(cameraRotationY) * cameraDistance + 100.0f;

        // Set up view matrix
        glm::vec3 cameraPos(camX, camY, camZ);
        glm::mat4 view = glm::lookAt(cameraPos,
            glm::vec3(0.0f, 100.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

        // Set up projection matrix based on view mode
        glm::mat4 projection;
        if (isPerspective) {
            projection = glm::perspective(glm::radians(45.0f),
                static_cast<float>(windowWidth) / windowHeight,
                0.1f, 1000.0f);
        }
        else {
            float orthoSize = 400.0f * 1.2f;
            projection = glm::ortho(-orthoSize, orthoSize,
                -orthoSize, orthoSize,
                -1000.0f, 1000.0f);
        }

        // Handle input - MOVED OUTSIDE THE PHYSICS TIMESTEP LOOP
        const float rotationSpeed = 0.01f;
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            isPerspective = false;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            isPerspective = true;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if (cameraRotationY < MAX_VERTICAL_ANGLE) {
                cameraRotationY += rotationSpeed;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if (cameraRotationY > -MAX_VERTICAL_ANGLE) {
                cameraRotationY -= rotationSpeed;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraRotationX -= rotationSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraRotationX += rotationSpeed;
        }

        // Space bar handling - MOVED OUTSIDE THE PHYSICS TIMESTEP LOOP
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
            if (!forceApplied) {
                particles[0].AddForce(MyVector(forceX, forceY, forceZ));
                forceApplied = true;
            }
            spacePressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
        }

        // Physics update - FIXED timing
        while (curr_ns >= timestep) {
            pWorld.Update(static_cast<float>(timestep.count()) / 1000000000.0f);
            curr_ns -= timestep;
        }

        // Render all spheres with new camera
        for (int i = 0; i < 5; i++) {
            spheres[i]->SetPosition(particles[i].Position);
            spheres[i]->Render(view, projection);
        }

        // Draw cables with new camera
        shader.Use();
        shader.SetMat4("mvp", projection * view);
        shader.SetVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

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