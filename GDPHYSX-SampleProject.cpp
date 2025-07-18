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

const float screenLeft = -400.0f;
const float screenRight = 400.0f;

// window dimensions
int windowWidth = 800;
int windowHeight = 800;

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
        cables[i] = new Physics::Cable(anchorPoint, cableLength, 5000.0f);

        // Add cable and gravity to force registry
        pWorld.forceRegistry.Add(&particles[i], cables[i]);
        pWorld.forceRegistry.Add(&particles[i], gravity);

        // Set sphere scale and initial position
        spheres[i]->SetScale(MyVector(particleRadius, particleRadius, particleRadius));
    }

    // Apply initial force to leftmost particle
    //particles[0].AddForce(MyVector(forceX, forceY, forceZ));

    //initialize clock and variables
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    // Camera setup
    float viewWidth = screenRight - screenLeft + 100.0f;
    float viewHalfHeight = viewWidth / 2.0f;
    float viewCenterY = 225.0f;

    // Camera setup
    glm::mat4 projection = glm::ortho(screenLeft - 50.0f, screenRight + 50.0f, viewCenterY - viewHalfHeight,  // bottom: -225
        viewCenterY + viewHalfHeight, -500.0f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    bool spacePressed = false;
    bool forceApplied = false;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        //Get current time
        curr_time = clock::now();
        //checking of duration since last iteration
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (curr_time - prev_time);
        //set prev time with current for next iteration
        prev_time = curr_time;


        //add the duration since last iteration to current time since last frame
        curr_ns += dur;
        while (curr_ns >= timestep) {
            //conver ns to ms
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestep);
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
            //call updates here:
            pWorld.Update((float)ms.count() / 1000);

            //reset time
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
        shader.SetVec3("objectColor", glm::vec3(0.3f, 0.3f, 0.3f)); // Dark gray for cables

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
