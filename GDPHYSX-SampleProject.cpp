#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>

#include <chrono>
using namespace std::chrono_literals;

constexpr std::chrono::nanoseconds timestep(16ms);

#include "GameObject.h" //andles visual objects
#include "Shader.h" //shader program management

//physics engine components
#include "p6/MyVector.h"
#include "p6/PhysicsParticle.h"
#include "p6/PhysicsWorld.h"
#include "p6/ForceGenerator.h"
#include "p6/ForceRegistry.h"
#include "p6/GravityForceGenerator.h"
#include "p6/DragForceGenerator.h"
#include "p6/PhaseOne/ParticleSystem.h"

using namespace Physics;

//representce a particle with both physics and visual components
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

//getting of user input for number of particles
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

//global variables
bool isPerspective = false;
bool isPaused = false;
float cameraDistance = 80.0f;
float cameraRotationX = 0.0f;
float cameraRotationY = 0.0f;

//handling of keyboard input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const float rotationSpeed = 0.05f;
    const float maxVerticalAngle = glm::radians(89.0f); //this prevents camera flip

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_1:
            isPerspective = false; //switching to orthographic
            break;
        case GLFW_KEY_2:
            isPerspective = true; //switchingg to perspective
            break;
        case GLFW_KEY_SPACE:
            isPaused = !isPaused; //toggling of pause  andplay
            break;
        }
    }
    //camera movement
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W:
            cameraRotationY += rotationSpeed; //up
            cameraRotationY = glm::clamp(cameraRotationY, -maxVerticalAngle, maxVerticalAngle);
            break;
        case GLFW_KEY_S:
            cameraRotationY -= rotationSpeed; //down
            cameraRotationY = glm::clamp(cameraRotationY, -maxVerticalAngle, maxVerticalAngle);
            break;
        case GLFW_KEY_A:
            cameraRotationX -= rotationSpeed; //left
            break;
        case GLFW_KEY_D:
            cameraRotationX += rotationSpeed; //right
            break;
        }
    }
}

int main() {
    //initializeGLFW and creating of window
    if (!glfwInit()) return -1;
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

    glfwSetKeyCallback(window, key_callback);

    const int maxParticles = getMaxParticlesFromUser(); //gets particle count from user

    Shader shader("Shaders/Sample.vert", "Shaders/Sample.frag"); //loading of shaders
    PhysicsWorld pWorld;
    std::vector<Particle> particles; //container for the particles
    particles.reserve(maxParticles);
    MyVector spawnPoint(0, -30, 0); //initial spawn pos

    //random number generators for particle properties
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> hueDist(0.0f, 1.0f); //co lor
    std::uniform_real_distribution<float> sizeDist(1.0f, 5.0f); //size
    std::uniform_real_distribution<float> lifeDist(1.0f, 10.0f); //lifespan
    std::uniform_real_distribution<float> forceDist(800.0f, 1200.0f); //force
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f); //direction
    std::uniform_real_distribution<float> radiusDist(2.0f, 10.0f); //spread

    //camera ssetup
    glm::mat4 projection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, -80.0f, 80.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //background color

    DragForceGenerator dragForce(0.2f, 0.01f); //creatin of drag forc e

    //timing variables
    using clock = std::chrono::high_resolution_clock;
    auto prev_time = clock::now();
    float spawnTimer = 0.0f;
    const float spawnInterval = 0.02f; //time between particle spawns
    bool ParticleStart = false;

    //main game loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - prev_time).count();
        prev_time = now;

        //setting up  of projection matrix based on current view mode
        glm::mat4 projection;
        if (isPerspective) {
            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 500.0f);
        }
        else {
            projection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, -80.0f, 500.0f);
        }

        //calculate camera position based on rotations
        float cosY = cos(cameraRotationY);
        float camX = sin(cameraRotationX) * cosY * cameraDistance;
        float camZ = cos(cameraRotationX) * cosY * cameraDistance;
        float camY = sin(cameraRotationY) * cameraDistance;

        //updating of view matrix
        glm::vec3 cameraPos = glm::vec3(camX, camY, camZ);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        view = glm::lookAt(cameraPos, target, up);

        //checking if restarting of particle spawning is needed
        if (ParticleStart && particles.empty()) {
            ParticleStart = false;
        }

        if (!isPaused) {
            //spawn new particles
            if (!ParticleStart) {
                spawnTimer += deltaTime;
                if (spawnTimer >= spawnInterval && particles.size() < maxParticles) {
                    spawnTimer = 0.0f;

                    //create new particle
                    particles.emplace_back("3D/sphere.obj", shader);
                    Particle& p = particles.back();

                    //set initial physics properties
                    p.physics.Position = MyVector(0, -80, 0);
                    p.physics.mass = 1.0f;
                    p.physics.Damping = 0.9f;
                    p.physics.Velocity = MyVector(0, 0, 0);

                    //calculate random direction for force
                    float theta = angleDist(gen);
                    float phi = angleDist(gen) * 0.25f;

                    float forceMagnitude = 3000.0f + forceDist(gen);
                    float sinPhi = sin(phi);
                    float cosPhi = cos(phi);
                    float sinTheta = sin(theta);
                    float cosTheta = cos(theta);

                    //applying of force in calculated direction
                    MyVector forceDirection(
                        sinPhi * cosTheta * 0.4f,
                        cosPhi * 2.0f,
                        sinPhi * sinTheta * 0.4f
                    );

                    p.physics.AddForce(forceDirection * forceMagnitude);

                    //setting of visual properties
                    float size = sizeDist(gen);
                    p.visual.SetScale(MyVector(size, size, size));
                    p.color = glm::vec3(hueDist(gen), hueDist(gen), hueDist(gen));
                    p.visual.SetColor(glm::vec4(p.color, 1.0f));

                    //set lifespan
                    p.maxLifetime = lifeDist(gen);
                    p.lifetime = p.maxLifetime;

                    pWorld.AddParticle(&p.physics);
                }
                else if (particles.size() >= maxParticles) {
                    ParticleStart = true; //spawning is done
                }
            }

            pWorld.Update(deltaTime); //updating of physics

            //remove dead particles and update living ones
            particles.erase(std::remove_if(particles.begin(), particles.end(),
                [&](Particle& p) {
                    p.lifetime -= deltaTime;
                    if (p.lifetime <= 0) return true;

                    //update visual properties based on remaining lifetime
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

        //render all particles
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