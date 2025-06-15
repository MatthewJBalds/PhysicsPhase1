#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "p6/MyVector.h"

class GameObject {
public:
    GameObject(const std::string& modelPath, Shader& shader);
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