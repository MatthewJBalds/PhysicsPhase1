#include "MyVector.h"

namespace Physics {
    // Constructors
    MyVector::MyVector() : x(0), y(0), z(0) {}
    MyVector::MyVector(const float _x, const float _y, const float _z)
        : x(_x), y(_y), z(_z) {}

    // Conversion to glm::vec3
    MyVector::operator glm::vec3() const {
        return glm::vec3(x, y, z);
    }

    // Vector magnitude (length)
    float MyVector::Magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Unit vector in same direction
    MyVector MyVector::Direction() const {
        float mag = Magnitude();
        if (mag <= 0) return MyVector(); // Handle zero vector
        return MyVector(x / mag, y / mag, z / mag);
    }

    // Normalize this vector
    void MyVector::Normalize() {
        float mag = Magnitude();
        if (mag > 0) {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }

    // Vector addition
    MyVector MyVector::operator+(const MyVector& rhs) const {
        return MyVector(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    void MyVector::operator+=(const MyVector& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }

    // Vector subtraction
    MyVector MyVector::operator-(const MyVector& rhs) const {
        return MyVector(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    void MyVector::operator-=(const MyVector& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
    }

    // Scalar multiplication
    MyVector MyVector::operator*(float scalar) const {
        return MyVector(x * scalar, y * scalar, z * scalar);
    }

    void MyVector::operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    // Component-wise product
    MyVector MyVector::ComponentProduct(const MyVector& rhs) const {
        return MyVector(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    // Dot product (scalar product)
    float MyVector::ScalarMultiplication(const MyVector& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    float MyVector::Dot(const MyVector& rhs) const {
        return ScalarMultiplication(rhs); // Alias
    }

    // Cross product (vector product)
    MyVector MyVector::VectorProduct(const MyVector& rhs) const {
        return MyVector(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }

    MyVector MyVector::Cross(const MyVector& rhs) const {
        return VectorProduct(rhs); // Alias
    }

    MyVector& MyVector::operator=(const MyVector& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    
}
