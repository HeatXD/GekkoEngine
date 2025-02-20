#pragma once

#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <iostream>

namespace Gekko::Math {

    struct Unit {
    private:
        int32_t _raw;

    public:
        static const int32_t ONE = 0x8000;
        static const int32_t HALF = 0x4000;

        Unit() : _raw(0) {}
        Unit(int32_t val) : _raw(val) {}
        Unit(const Unit& val) : _raw(val._raw) {}

        static Unit From(int32_t val) {
            return Unit(val * ONE);
        }

        Unit operator+(const Unit& other) const {
            return Unit(_raw + other._raw);
        }

        Unit& operator+=(const Unit& other) {
            _raw += other._raw;
            return *this;
        }

        Unit operator-(const Unit& other) const {
            return Unit(_raw - other._raw);
        }

        Unit& operator-=(const Unit& other) {
            _raw -= other._raw;
            return *this;
        }

        Unit operator/(const Unit& other) const {
            if (other._raw == 0) {
                throw std::runtime_error("Division by zero");
            }
            int64_t num = static_cast<int64_t>(_raw) * ONE;
            int64_t adjust = std::abs(other._raw) / 2;
            return Unit(static_cast<int32_t>((num + (other._raw > 0 ? adjust : -adjust)) / other._raw));
        }

        Unit operator*(const Unit& other) const {
            int64_t result = static_cast<int64_t>(_raw) * other._raw;
            return Unit(static_cast<int32_t>((result + (ONE / 2)) / ONE));
        }

        bool operator==(const Unit& other) const {
            return _raw == other._raw;
        }

        bool operator!=(const Unit& other) const {
            return _raw != other._raw;
        }

        Unit& operator*=(const Unit& other) {
            *this = *this * other;
            return *this;
        }

        Unit& operator/=(const Unit& other) {
            *this = *this / other;
            return *this;
        }

        // VISUALIZATION ONLY
        inline float AsFloat() const {
            return static_cast<float>(_raw) / ONE;
        }
    };

    // VISUALIZATION ONLY
    struct Vec3F {
        float x, y, z;
        Vec3F(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
    };

    struct Vec3 {
        static const Vec3 ZERO;

        Unit x, y, z;

        Vec3() : x(), y(), z() {}
        Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
        Vec3(const Unit& xx, const Unit& yy, const Unit& zz) : x(xx), y(yy), z(zz) {}

        Unit Dot(const Vec3& other) const {
            return (x * other.x) + (y * other.y) + (z * other.z);
        }

        Vec3 operator+(const Vec3& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        Vec3& operator+=(const Vec3& other) {
            *this = *this + other;
            return *this;
        }

        Vec3 operator+(const Unit& other) const {
            return Vec3(x + other, y + other, z + other);
        }

        Vec3& operator+=(const Unit& other) {
            *this = *this + other;
            return *this;
        }

        Vec3 operator-(const Vec3& other) const {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

        Vec3& operator-=(const Vec3& other) {
            *this = *this - other;
            return *this;
        }

        Vec3 operator-(const Unit& other) const {
            return Vec3(x - other, y - other, z - other);
        }

        Vec3& operator-=(const Unit& other) {
            *this = *this - other;
            return *this;
        }

        Vec3 operator/(const Vec3& other) const {
            return Vec3(x / other.x, y / other.y, z / other.z);
        }

        Vec3 operator/(const Unit& other) const {
            return Vec3(x / other, y / other, z / other);
        }

        Vec3& operator/=(const Vec3& other) {
            *this = *this / other;
            return *this;
        }

        Vec3& operator/=(const Unit& other) {
            *this = *this / other;
            return *this;
        }

        Vec3 operator*(const Vec3& other) const {
            return Vec3(x * other.x, y * other.y, z * other.z);
        }

        Vec3 operator*(const Unit& other) const {
            return Vec3(x * other, y * other, z * other);
        }

        Vec3& operator*=(const Vec3& other) {
            *this = *this * other;
            return *this;
        }

        Vec3& operator*=(const Unit& other) {
            *this = *this * other;
            return *this;
        }

        bool operator==(const Vec3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vec3& other) const {
            return !(*this == other);
        }

        // VISUALIZATION ONLY
        Vec3F AsFloat() const {
            return Vec3F(x.AsFloat(), y.AsFloat(), z.AsFloat());
        }
    };
}
