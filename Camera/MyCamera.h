#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

class MyCamera
{
    protected:
        glm::vec3 cameraPos;
        glm::vec3 orientation;
        glm::vec3 up;
        glm::mat4 view_matrix;


        glm::mat4 identity_matrix = glm::mat4(1.f);
    public:
        MyCamera();

        void CameraMovement(glm::vec3 movement);
        void setCameraPosition(glm::vec3 position);
        void setCenter(glm::vec3 orientation);

        virtual void Update(GLFWwindow* window, float time);
    public:
        void SetViewMatrix();
        glm::mat4 GetViewMatrix() const { return view_matrix; };
    public:
        virtual glm::mat4 getViewProjection() const = 0;
};

