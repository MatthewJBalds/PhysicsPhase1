#include "RenderLine.h"
#include "../../Shader.h"

void Physics::RenderLine::Draw(glm::vec3 a, glm::vec3 b)
{
    // Set up line vertices
    float vertices[] = {
        a.x, a.y, a.z,
        b.x, b.y, b.z
    };

    // Create and bind VAO/VBO for line
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set line properties
    glLineWidth(2.0f);  // Make lines thicker and more visible

    // Draw the line
    glDrawArrays(GL_LINES, 0, 2);

    // Clean up
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}