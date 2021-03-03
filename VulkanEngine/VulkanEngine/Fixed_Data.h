#pragma once
#ifndef _FIXED_DATA_H
#define _FIXED_DATA_H

#include "GameObject.h"
#include "pch.h"

#define number_of_objects 15
#define EPSILON 0.00001



struct Point_xy
{
	float x;
	float y;

	Point_xy(float u = 0, float v = 0) : x(u), y(v)
	{}
};
struct _Point_xy
{
	float x1;
	float y1;

	_Point_xy(float u = 0.0f, float v = 0.0f) : x1(u), y1(v)
	{}
};

//gonna place this struct here for now, is project exclusive for now
struct VertexFormat
{
	glm::vec4 color;
	glm::vec3 position;

	//default
	VertexFormat() {
		color = glm::vec4(0.0f);
		position = glm::vec3(0.0f);
	}

	//Constructor
	VertexFormat(const glm::vec3& pos, const glm::vec4& iColor)
	{
		position = pos;
		color = iColor;
	}
};

typedef struct Tree_Point
{
	float xpos, ypos;

}Tree_Point;

#pragma region Definitions

float timeStep = 0.016f;
float overlap;
bool collision = false;
int lastState;
const int k = 2;
float min_x = -2.0f;
float min_y = -2.0f;
float max_x = 2.0f;
float max_y = 2.0f;
int numberOfObjects = number_of_objects;

float min_x_2 = -1.0f;
float min_y_2 = -1.0f;

glm::vec3 pointOfCollision;
glm::vec3 minimumTranslationVector;
glm::vec3 mousePos;
glm::vec3 tempVelocity;
glm::vec3 tempPosition;
glm::vec3 tempPosition2;
glm::vec3 tempVelocity2;

float point_body_x[number_of_objects];
float point_body_y[number_of_objects];

float point_tree_x[number_of_objects];
float point_tree_y[number_of_objects];

// vector of scene bodies
std::vector<VertexFormat> lines;

//gameObjects can probably be snagged later in gameManager VECTOR IS THERE
std::vector<GameObject*> bodies;

//vectors for data trees
std::vector<float> domain;
std::vector<float> range;
std::vector<Tree_Point> Points_Tree;
std::vector<_Point_xy> Point_Coord;

//objects for data trees
//Will have to see about these, OpenGL project is quite different
//VertexBuffer lineBuffer;
//Model* mesh;
Tree_Point point_temp;

#pragma endregion
#endif _FIXED_DATA_H