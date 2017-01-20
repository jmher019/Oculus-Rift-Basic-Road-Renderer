#include "..\..\lib\glm\glm.hpp"

using namespace glm;

struct Vertex {
	// Object Space Coordinate
	vec3 v;
	// Vertex Normal
	vec3 n;
	// Vertex Color
	vec3 c;
	// Vertex Texture Coordinate
	vec2 vt;
};