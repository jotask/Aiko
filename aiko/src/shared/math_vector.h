#pragma once

#include <iostream> //  For Log::error
#include <iomanip> // For std::setw
#include <cstring> // For std::memcpy

#include "core/log.h"

namespace aiko
{

    class ivec2
    {
    public:
        ivec2() : ivec2(0) { };
        ivec2(int xyz) : ivec2(xyz, xyz) { };
        ivec2(int x, int y) : x(x), y(y) { };
        int x;
        int y;
        bool operator==(ivec2& other) const { return { x == other.x && y == other.y }; }
        bool operator==(const ivec2& other) const { return { x == other.x && y == other.y }; }
        ivec2 operator*(const float& scalar) const { return { static_cast<int>(x * scalar), static_cast<int>(y * scalar) }; }
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
        // Constructor
        mat4()
        {
            // Initialize all elements to 0.0f
            memset(elements, 0.0f, sizeof(elements));
        }

        // Identity matrix constructor
        mat4(float identity) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    elements[i][j] = (i == j) ? identity : 0.0f;
                }
            }
        }

        mat4(const float(&data)[16]) {
            int index = 0;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    elements[i][j] = data[index++];
                }
            }
        }

        // Accessors
        float& operator()(int row, int col)
        {
            return elements[row][col];
        }

        const float& operator()(int row, int col) const
        {
            return elements[row][col];
        }

        mat4 operator*(const mat4& other) const {
            mat4 result;

            // Perform matrix multiplication
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.elements[i][j] = 0;
                    for (int k = 0; k < 4; ++k) {
                        result.elements[i][j] += elements[i][k] * other.elements[k][j];
                    }
                }
            }

            return result;
        }

        // Print matrix
        void print() const
        {
            static std::stringstream buffer;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    buffer << std::setw(8) << std::setprecision(3) << elements[i][j] << " ";
                }
                buffer << std::endl;
            }
            Log::info(buffer.str());
        }

        // Copy assignment operator
        mat4& operator=(const mat4& other)
        {
            if (this != &other) {
                std::memcpy(elements, other.elements, sizeof(elements));
            }
            return *this;
        }

        const float* data() const {
            return &elements[0][0];
        }

    // private:
        float elements[4][4];
    };

}

