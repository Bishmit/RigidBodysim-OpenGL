#include "Vec2.h"
#include <cmath>

Vec2::Vec2() : vec(0.0f, 0.0f) {}
Vec2::Vec2(float x, float y) : vec(x, y) {}
Vec2::Vec2(const glm::vec2& v) : vec(v) {}

void Vec2::Add(const Vec2& v){
    vec += v.vec; 
}

void Vec2::Sub(const Vec2& v){
    vec -= v.vec; 
}

void Vec2::Scale(float n){
    vec *= n; 
}

Vec2 Vec2::Rotate(float angle) const {
    return Vec2(glm::rotate(vec, angle));
}

float Vec2::Magnitude() const {
    return glm::length(vec);
}

float Vec2::MagnitudeSquared() const {
    return glm::length2(vec);
}

Vec2& Vec2::Normalize() {
    vec = glm::normalize(vec);
    return *this;
}

Vec2 Vec2::UnitVector() const {
    return Vec2(glm::normalize(vec));
}

Vec2 Vec2::Normal() const {
    return Vec2(glm::normalize(glm::vec2(-vec.y, vec.x))); 
}

float Vec2::Dot(const Vec2& v) const {
    return glm::dot(vec, v.vec);
}

float Vec2::Cross(const Vec2& v) const {
    return vec.x * v.vec.y - vec.y * v.vec.x;
}

Vec2& Vec2::operator=(const Vec2& v) {
    vec = v.vec;
    return *this;
}

bool Vec2::operator==(const Vec2& v) const {
    return vec == v.vec;
}

bool Vec2::operator!=(const Vec2& v) const {
    return vec != v.vec;
}

Vec2 Vec2::operator+(const Vec2& v) const {
    return Vec2(vec + v.vec);
}

Vec2 Vec2::operator-(const Vec2& v) const {
    return Vec2(vec - v.vec);
}

Vec2 Vec2::operator*(float n) const {
    return Vec2(vec * n);
}

Vec2 Vec2::operator/(float n) const {
    return Vec2(vec / n);
}

Vec2 Vec2::operator-() const {
    return Vec2(-vec);
}

Vec2& Vec2::operator+=(const Vec2& v) {
    vec += v.vec;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& v) {
    vec -= v.vec;
    return *this;
}

Vec2& Vec2::operator*=(float n) {
    vec *= n;
    return *this;
}

Vec2& Vec2::operator/=(float n) {
    vec /= n;
    return *this;
}
