#include "GameObject.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>

//GameObject::GameObject(const std::string& modelPath, Shader& shader)
//    : shader(shader) {
//    LoadModel(modelPath);
//    SetupBuffers();
//}

GameObject::~GameObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void GameObject::LoadModel(const std::string& path) {
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices = attributes.vertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            indices.push_back(index.vertex_index);
        }
    }
}

void GameObject::SetupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GameObject::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    model = glm::scale(model, scale);

    shader.Use();
    shader.SetMat4("mvp", projectionMatrix * viewMatrix * model);
    shader.SetVec3("color", color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

//void GameObject::SetPosition(const glm::vec3& position) {
//    this->position = position;
//}
//
//void GameObject::SetScale(const glm::vec3& scale) {
//    this->scale = scale;
//}

void GameObject::SetPosition(const Physics::MyVector& position) {
    this->position = glm::vec3(position.x, position.y, position.z);
}

void GameObject::SetScale(const Physics::MyVector& scale) {
    this->scale = glm::vec3(scale.x, scale.y, scale.z);
}

Physics::MyVector GameObject::GetPosition() const {
    return Physics::MyVector(position.x, position.y, position.z);
}

Physics::MyVector GameObject::GetScale() const {
    return Physics::MyVector(scale.x, scale.y, scale.z);
}

void GameObject::SetRotation(float angle, const glm::vec3& axis) {
    this->rotationAngle = angle;
    this->rotationAxis = axis;
}

GameObject::GameObject(const std::string& modelPath, Shader& shader, const glm::vec3& color)
    : shader(shader), color(color) {
    LoadModel(modelPath);
    SetupBuffers();
}

void GameObject::SetColor(const glm::vec3& newColor) {
    color = newColor;
}