#pragma once

#include "glm/glm.hpp"
#include "math.h"


constexpr glm::vec3 up = {0, 0, 1.0f};

class Plane
{
public:
    Plane() = default;
    Plane(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) :
        Plane(cross(v2 - v1, v3 - v1), v1)
    {}
    Plane(const glm::vec3& normal, const glm::vec3& point) :
        Plane(normal, glm::dot(normal, point))
    {}
    Plane(const glm::vec3& normal, float distance) :
        normal(normal), distance(distance)
    {}

    enum Classification {
        Front,
        Back,
        Planar
    };
    Classification Classify(const glm::vec3& point) const
    {
		auto nominator = glm::dot(normal, point) - distance;
		if (nominator > SMALL_EPS) {
			return Classification::Front;
		} else if (nominator < -SMALL_EPS) {
			return Classification::Back;
		} else {
			return Classification::Planar;
		}
    }
    float SignedDistance(const glm::vec3& point) const
    {
		auto nominator = glm::dot(normal, point) - distance;
        if (fabs(nominator) < SMALL_EPS) {
            return 0;
        }
        return nominator;
    }
    const glm::vec3& Normal() const
    {
        return normal;
    }
    float     Distance() const
    {
        return distance;
    }
    void Transform(const glm::mat4& mat)
    {
        glm::vec3 point = normal * distance;
        point = glm::vec3(mat * glm::vec4(point, 1.0));
        distance = dot(normal, point);
    }

private:
    glm::vec3   normal = {0, 0, 0};
    float       distance = 0.f;
};
