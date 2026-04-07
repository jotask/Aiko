#pragma once

#include <iomanip> // For std::setw
#include <cstring> // For std::memcpy
#include <cmath> // For std::memcpy

#include "logger/logger.h"

namespace aiko
{

    template <typename T>
    class vec2def
    {
    public:
        constexpr vec2def() : vec2def(0) { };
        constexpr vec2def(const T xy) : vec2def(xy, xy) { };
        constexpr vec2def(const T x, const T y) : x(x), y(y) { };
        int magnitude() const { return std::sqrt(x * x + y * y); };
        int product() const { return x * y; };
        T x;
        T y;
        bool operator==(vec2def& other) const { return { x == other.x && y == other.y }; }
        bool operator==(const vec2def& other) const { return { x == other.x && y == other.y }; }
        bool operator!=(vec2def& other) const { return !(*this == other); }
        bool operator!=(const vec2def& other) const { return !(*this == other); }
        bool operator<(const vec2def& other) const { if (x != other.x) { return x < other.x; } return y < other.y; }
        vec2def operator*(const vec2def& other) const { return { x * other.x, y * other.y }; }
        vec2def operator+(const vec2def& other) const { return { x + other.x, y + other.y }; }
        vec2def operator-(const vec2def& other) const { return { x - other.x, y - other.y }; }
    };

    using ivec2 = vec2def<int>;
    using vec2 = vec2def<float>;

    template <typename T>
    class vec3def
    {
    public:
        constexpr vec3def() : vec3def(0.0f) { };
        constexpr vec3def(vec2def<T> other) : vec3def(other, 0) { };
        constexpr vec3def(vec2def<T> other, T z) : vec3def(other.x, other.y, z) { };
        constexpr vec3def(T xyz) : vec3def(xyz, xyz, xyz) { };
        constexpr vec3def(T x, T y, T z) : x(x), y(y), z(z) { };

        T x;
        T y;
        T z;

        operator T* () {
            return &x;
        }

        // Addition
        vec3def operator+(const vec3def& other) const {
            return vec3def(x + other.x, y + other.y, z + other.z);
        }

        // Subtraction
        vec3def operator-(const vec3def& other) const {
            return vec3def(x - other.x, y - other.y, z - other.z);
        }

        // Multiplication
        vec3def operator*(float scalar) const {
            return vec3def(x * scalar, y * scalar, z * scalar);
        }

        vec3def operator*(const vec3def& other) const {
            return vec3def{ x * other.x, y * other.y, z * other.z };
        }

        // Division
        vec3def operator/(float scalar) const {
            return vec3def(x / scalar, y / scalar, z / scalar);
        }

        // Compound Addition
        vec3def& operator+=(const vec3def& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        // Compound Subtraction
        vec3def& operator-=(const vec3def& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        // Compound Multiplication (for scalar multiplication)
        vec3def& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
        vec3def& operator*=(vec3def other) {
            *this = *this * other;
            return *this;
        }

        // Compound Division (for scalar division)
        vec3def& operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

    };

    using ivec3 = vec3def<int>;
    using vec3 = vec3def<float>;

    template<typename T>
    class vec4def
    {
    public:
        constexpr vec4def() : vec4def(0.0f) { };
        constexpr vec4def(T xyzw) : vec4def(xyzw, xyzw, xyzw, xyzw) { };
        constexpr vec4def(vec3def<T> v3) : vec4def(v3.x, v3.y, v3.z, 0.0f) { };
        constexpr vec4def(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { };
        T x;
        T y;
        T z;
        T w;
    };

    using vec4 = vec4def<float>;

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

