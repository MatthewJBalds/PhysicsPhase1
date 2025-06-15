#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

//time
#include <chrono>
using namespace std::chrono_literals;
//time inbetween frames
constexpr std::chrono::nanoseconds timestep(1000ms); //adjusted so i can click multiple times before the update hahaha

//own includes
#include "GameObject.h"
#include "Shader.h"

//physics engine includes
#include "p6/MyVector.h"
#include "p6/PhysicsParticle.h"
#include "p6/PhysicsWorld.h"
#include "p6/ForceGenerator.h"
#include "p6/ForceRegistry.h"
#include "p6/DragForceGenerator.h"

using namespace Physics;

const float trackTLength = 1000.0f;
const float boostLine = 0.6f * trackTLength;

class EnemyForce : public ForceGenerator {
private:
    float acceleration;
    bool boosted = false;
public:
    EnemyForce(float accel) : acceleration(accel) {}

    void UpdateForce(PhysicsParticle* particle, float duration) override {
        if (!boosted && particle->Position.x >= boostLine) {
            float boost = 1.1f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (8.0f - 1.1f)));
            acceleration *= boost;
            boosted = true;
        }
        particle->AddForce(Physics::MyVector(acceleration * particle->mass, 0, 0));
    }
};

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
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "PC01-MJ_Baldonado", NULL, NULL);
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

    // Shaders
    Shader shader("Shaders/Sample.vert", "Shaders/Sample.frag");

    // Set Objecs
    GameObject spheres[4] = {
        GameObject("3D/sphere.obj", shader, glm::vec3(1.0f, 0.0f, 0.0f)),  
        GameObject("3D/sphere.obj", shader, glm::vec3(0.0f, 0.0f, 1.0f)),    
        GameObject("3D/sphere.obj", shader, glm::vec3(0.0f, 1.0f, 0.0f)),   
        GameObject("3D/sphere.obj", shader, glm::vec3(1.0f, 0.0f, 1.0f))    
    };

    // Physics World
    Physics::PhysicsWorld pWorld = Physics::PhysicsWorld();

    // Set pos
    Physics::MyVector positions[4] = {
        Physics::MyVector(0, 150, 0),   
        Physics::MyVector(0, 50, 0),    
        Physics::MyVector(0, -50, 0),   
        Physics::MyVector(0, -150, 0)  
    };

    Physics::PhysicsParticle particles[4];
    for (int i = 0; i < 4; i++) {
        spheres[i].SetScale(MyVector(20.0f, 20.0f, 20.0f));
        particles[i].Position = positions[i];
        particles[i].mass = 1.0f;
        pWorld.AddParticle(&particles[i]);
    }

    //drag force
    Physics::DragForceGenerator playerDrag(0.5f, 0.1f);
    pWorld.forceRegistry.Add(&particles[3], &playerDrag);

    // enemy set force
    EnemyForce* enemyForces[3] = {
        new EnemyForce(20.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f), 
        new EnemyForce(20.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f),
        new EnemyForce(20.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f)
    };

    for (int i = 0; i < 3; i++) {
        pWorld.forceRegistry.Add(&particles[i], enemyForces[i]);
    }

    // player
    int spaceCount = 0;
    bool spacePressed = false;
    bool isPressed = false;
    const float pressForce = 10.0f;
    float totalForce = 0.0f;

    //game flags
    std::vector<std::pair<float, int>> finishTimes; 
    bool raceFinished = false;
    float totalTime = 0.0f;

    //initialize clock and variables
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    // Camera 
    glm::mat4 projection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, -1000.0f, 1000.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(500.0f, 0.0f, 10.0f), 
        glm::vec3(500.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Main loop
    while (!glfwWindowShouldClose(window) && !raceFinished) {
        glClear(GL_COLOR_BUFFER_BIT);

        // input handler
        spacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);

        if (spacePressed && !isPressed) {
            spaceCount++;
            std::cout << "Space Count: " << spaceCount << "\n";
        }
        isPressed = spacePressed;

        // print time
        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;

        curr_ns += dur;
        if (curr_ns >= timestep) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
            float deltaTime = (float)ms.count() / 1000;
            totalTime += deltaTime;

            //std::cout << "Frame time: " << (float)ms.count() << "ms\n";

            curr_ns -= curr_ns;

            // player movement
            if (spaceCount > 0) {
                totalForce = pressForce * spaceCount;
                particles[3].AddForce(Physics::MyVector(totalForce * particles[3].mass, 0, 0));
                std::cout << "force: " << totalForce << " ms "
                    << spaceCount << " presses\n";
                spaceCount = 0;
                totalForce = 0.0f;
            }

            pWorld.Update(deltaTime);

      
            for (int i = 0; i < 4; i++) {
                particles[i].Position.y = positions[i].y;
            }

            // end flags
            for (int i = 0; i < 4; i++) {
                if (particles[i].Position.x >= trackTLength &&
                    std::find_if(finishTimes.begin(), finishTimes.end(),
                        [i](const auto& pair) { return pair.second == i; }) == finishTimes.end()) {
                    finishTimes.emplace_back(totalTime, i);
                    std::cout << "Particle " << i << " finished at " << totalTime << " seconds\n";
                }
            }

            // end flag
            if (finishTimes.size() == 4) {
                raceFinished = true;
                std::sort(finishTimes.begin(), finishTimes.end());

                std::cout << "\nFinal Standings:\n";
                for (size_t i = 0; i < finishTimes.size(); i++) {
                    const char* names[] = { "Red", "Blue", "Green", "Purple" };
                    std::cout << i + 1 << ". " << names[finishTimes[i].second]
                        << " - Time: " << finishTimes[i].first << "s\n";
                }
            }
        }

        //render
        for (int i = 0; i < 4; i++) {
            spheres[i].SetPosition(particles[i].Position);
            spheres[i].Render(view, projection);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    for (int i = 0; i < 3; i++) {
        delete enemyForces[i];
    }

    glfwTerminate();
    return 0;
}