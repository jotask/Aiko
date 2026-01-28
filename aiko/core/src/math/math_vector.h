#pragma once

#include <iomanip> // For std::setw
#include <cstring> // For std::memcpy
#include <cmath> // For std::memcpy

#include "logger/logger.h"

namespace aiko
{

    class ivec2
    {
    public:
        constexpr ivec2() : ivec2(0) { };
        constexpr ivec2(const int xy) : ivec2(xy, xy) { };
        constexpr ivec2(const int x, const int y) : x(x), y(y) { };
        int magnitude() const { return std::sqrt(x * x + y * y); };
        int product() const { return x * y; };
        int x;
        int y;
        bool operator==(ivec2& other) const { return { x == other.x && y == other.y }; }
        bool operator==(const ivec2& other) const { return { x == other.x && y == other.y }; }
        bool operator!=(ivec2& other) const { return !(*this == other); }
        bool operator!=(const ivec2& other) const { return !(*this == other); }
        bool operator<(const ivec2& other) const { if (x != other.x) { return x < other.x; } return y < other.y; }
        ivec2 operator*(const ivec2& other) const { return { x * other.x, y * other.y }; }
        ivec2 operator+(const ivec2& other) const { return { x + other.x, y + other.y }; }

    };

    class vec2
    {
    public:
        vec2() : vec2(0.0f) { };
        vec2(ivec2 other) : vec2( static_cast<float>(other.x), static_cast<float>(other.y)) { };
        vec2(float xyz) : vec2(xyz, xyz) { };
        vec2(float x, float y) : x(x), y(y) { };
        float x;
        float y;

        vec2 operator*(const float& scalar) const { return { x * scalar, y * scalar }; }
        vec2 operator+(vec2& other) const { return { x + other.x, y + other.y }; }
        vec2 operator+(const vec2& other) const { return { x + other.x, y + other.y }; }
        vec2 operator-(vec2& other) const { return { x - other.x, y - other.y }; }
        vec2 operator+=(vec2& other) const { return { x + other.x, y + other.y }; }
        bool operator==(vec2& other) const { return { x == other.x && y == other.y }; }

    };

    class vec3
    {
    public:
        vec3() : vec3(0.0f) { };
        vec3(ivec2 other) : vec3(other.x, other.y, 0) { };
        vec3(vec2 other) : vec3(other, 0) { };
        vec3(vec2 other, float z) : vec3(other.x, other.y, z) { };
        vec3(float xyz) : vec3(xyz, xyz, xyz) { };
        vec3(float x, float y, float z) : x(x), y(y), z(z) { };
        float x;
        float y;
        float z;

        operator float* () {
            return &x;
        }

        // Addition
        vec3 operator+(const vec3& other) const {
            return vec3(x + other.x, y + other.y, z + other.z);
        }

        // Subtraction
        vec3 operator-(const vec3& other) const {
            return vec3(x - other.x, y - other.y, z - other.z);
        }

        // Multiplication
        vec3 operator*(float scalar) const {
            return vec3(x * scalar, y * scalar, z * scalar);
        }

        vec3 operator*(const vec3& other) const {
            return vec3{ x * other.x, y * other.y, z * other.z };
        }

        // Division
        vec3 operator/(float scalar) const {
            return vec3(x / scalar, y / scalar, z / scalar);
        }

        // Compound Addition
        vec3& operator+=(const vec3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        // Compound Subtraction
        vec3& operator-=(const vec3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        // Compound Multiplication (for scalar multiplication)
        vec3& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
        vec3& operator*=(vec3 other) {
            *this = *this * other;
            return *this;
        }

        // Compound Division (for scalar division)
        vec3& operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

    };

    class vec4
    {
    public:
        vec4() : vec4(0.0f) { };
        vec4(float xyzw) : vec4(xyzw, xyzw, xyzw, xyzw) { };
        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { };
        float x;
        float y;
        float z;
        float w;
    };

    class mat4
    {
    public:

        mat4()
        {
            // Initialize all elements to 0
            std::memset(elements, 0, sizeof(elements));
        }

        // Identity matrix constructor
        mat4(float identity)
        {
            std::memset(elements, 0, sizeof(elements));
            elements[0]  = identity;
            elements[5]  = identity;
            elements[10] = identity;
            elements[15] = identity;
            /*
            for (int i = 0; i < std::size; ++i)
            {
                elements[i][i] = identity;
            }
            */
        }

        mat4(const float(&data)[16])
        {
            std::memcpy(elements, data, sizeof(elements));
        }

        // Accessors
        float& operator()(int row, int col)
        {
            return elements[col * 4 + row];
        }

        const float& operator()(int row, int col) const
        {
            return elements[col * 4 + row];
        }

        mat4 operator*(const mat4& other) const
        {
            mat4 result;
            for (int col = 0; col < 4; ++col)
                {
                for (int row = 0; row < 4; ++row)
                    {
                    result(row, col) = 0.0f;
                    for (int k = 0; k < 4; ++k)
                    {
                        result(row, col) += (*this)(row, k) * other(k, col);
                    }
                }
            }
            return result;
        }

        // Print matrix
        void print() const
        {
            std::stringstream buffer;
            for (int row = 0; row < 4; ++row)
            {
                for (int col = 0; col < 4; ++col)
                {
                    buffer << std::setw(8) << std::setprecision(3) << (*this)(row, col) << " ";
                }
                buffer << std::endl;
            }
            logger::Log::info(buffer.str());
        }

        // Copy assignment operator
        mat4& operator=(const mat4& other)
        {
            if (this != &other)
                {
                std::memcpy(elements, other.elements, sizeof(elements));
            }
            return *this;
        }

        float* data() { return elements; }
        const float* data() const { return elements; }

    private:
        float elements[4 * 4];

    };

}

