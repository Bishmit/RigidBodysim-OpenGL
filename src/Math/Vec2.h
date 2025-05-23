#pragma once 

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

class Vec2
{
private:
    glm::vec2 vec; 
public:
    Vec2();
    Vec2(float x, float y); 
    Vec2(const glm::vec2& v);
    ~Vec2() = default; 

    float& x() { return vec.x; }
    float& y() { return vec.y; }
    const float& x() const { return vec.x; }
    const float& y() const { return vec.y; }

    // Basic operations
    void Add(const Vec2& v); 
    void Sub(const Vec2& v); 
    void Scale(float n); 
    Vec2 Rotate(float angle) const; 

    // Vector properties
    float Magnitude() const; 
    float MagnitudeSquared() const; 

    Vec2& Normalize();
    Vec2 UnitVector() const;
    Vec2 Normal() const; 
    
    float Dot(const Vec2& v) const; 
    float Cross(const Vec2& v) const; 

     // Getters
    // float x() const { return vec.x; }
    // float y() const { return vec.y; }
    glm::vec2& Glm() { return vec; }
    const glm::vec2& Glm() const { return vec; }

    // Operators
    Vec2& operator=(const Vec2& v);
    bool operator==(const Vec2& v) const;
    bool operator!=(const Vec2& v) const;

    Vec2 operator+(const Vec2& v) const;
    Vec2 operator-(const Vec2& v) const;
    Vec2 operator*(float n) const;
    Vec2 operator/(float n) const;
    Vec2 operator-() const;

    Vec2& operator+=(const Vec2& v);
    Vec2& operator-=(const Vec2& v);
    Vec2& operator*=(float n);
    Vec2& operator/=(float n);
};

std::ostream& operator<<(std::ostream& os, const Vec2& v);

