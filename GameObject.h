#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "p6/MyVector.h"

class GameObject {
public:
    //GameObject(const std::string& modelPath, Shader& shader);
    ~GameObject();

    void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const;

    //void SetPosition(const glm::vec3& position);
    //void SetScale(const glm::vec3& scale);
    GameObject(const std::string& modelPath, Shader& shader, const glm::vec3& color = glm::vec3(1.0f));
    void SetRotation(float angle, const glm::vec3& axis);
    void SetPosition(const Physics::MyVector& position);
    void SetScale(const Physics::MyVector& scale);
    Physics::MyVector GetPosition() const;
    Physics::MyVector GetScale() const;
    void SetColor(const glm::vec3& newColor);


    //pashe one
    // Move constructor
    GameObject(GameObject&& other) noexcept
        : shader(other.shader),
        position(std::move(other.position)),
        scale(std::move(other.scale)),
        color(std::move(other.color)),
        VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
        vertices(std::move(other.vertices)),
        indices(std::move(other.indices)) {
        other.VAO = other.VBO = other.EBO = 0;  // Invalidate source
    }

    // Move assignment operator
    GameObject& operator=(GameObject&& other) noexcept {
        if (this != &other) {
            // Release current resources
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);

            // Transfer ownership
            shader = other.shader;
            position = std::move(other.position);
            scale = std::move(other.scale);
            color = std::move(other.color);
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);

            // Invalidate source
            other.VAO = other.VBO = other.EBO = 0;
        }
        return *this;
    }

    // Delete copy operations
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

private:
    void LoadModel(const std::string& modelPath);
    void SetupBuffers();

    GLuint VAO, VBO, EBO;
    Shader& shader;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    float rotationAngle = 0.0f;

    glm::vec3 color;
};