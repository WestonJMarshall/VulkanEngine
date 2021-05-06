#pragma once
#include "pch.h"
#include <string>


class VectorMath2D 
{
private:


public:
	float length(glm::vec2 v) 
	{
		return glm::sqrt(v.x * v.x + v.y * v.y);
	}

	glm::vec2 add(glm::vec2 v1, glm::vec2 v2) 
	{
		return glm::vec2(v1.x + v2.x, v1.y + v2.y);
	}

	glm::vec2 subtract(glm::vec2 v1, glm::vec2 v2)
	{
		return glm::vec2(v1.x - v2.x, v1.y - v2.y);
	}

	glm::vec2 scale(glm::vec2 v, float n)
	{
		return glm::vec2(v.x * n, v.y * n);
	}

	float dot(glm::vec2 v1, glm::vec2 v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	float cross(glm::vec2 v1, glm::vec2 v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

	glm::vec2 normalize(glm::vec2 n) {
		float len = length(n);
		if (len > 0)
		{
			len = 1 / len;
		}
		return glm::vec2(n.x * len, n.y * len);
	}

	float distance(glm::vec2 v1, glm::vec2 v2)
	{
		float x = v1.x - v2.x;
		float y = v1.y - v2.y;
		return glm::sqrt(x * x + y * y);
	}

	glm::vec2 rotate(glm::vec2 v, glm::vec2 center, float angle) 
	{
		float r[2];
		float x = v.x - center.x;
		float y = v.y - center.y;
		r[0] = x * glm::cos(angle) - y * glm::sin(angle);
		r[1] = x * glm::sin(angle) + y * glm::cos(angle);
		r[0] += center.x;
		r[1] += center.y;

		return glm::vec2(r[0], r[1]);
	}
};