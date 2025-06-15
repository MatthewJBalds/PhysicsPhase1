#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Physics {
    class MyVector
    {
    public:
        float x, y, z;

        // Constructors
        MyVector();
        MyVector(const float _x, const float _y, const float _z);

        // Conversion
        explicit operator glm::vec3() const;

        // Vector properties
        float Magnitude() const;
        MyVector Direction() const;
        void Normalize();

        // Arithmetic operations
        MyVector operator+(const MyVector& rhs) const;  // Addition
        void operator+=(const MyVector& rhs);

        MyVector operator-(const MyVector& rhs) const; // Subtraction
        void operator-=(const MyVector& rhs);

        MyVector operator*(float scalar) const; // Scalar multiplication
        void operator*=(float scalar);

        MyVector& operator=(const MyVector& rhs);

        // Vector products
        MyVector ComponentProduct(const MyVector& rhs) const; // Component-wise product

        float ScalarMultiplication(const MyVector& rhs) const; // Dot product (alias)

        float Dot(const MyVector& rhs) const; // Dot product

        MyVector VectorProduct(const MyVector& rhs) const; // Cross product (alias)
        MyVector Cross(const MyVector& rhs) const; // Cross product
    };
}