#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H
#include "..\..\lib\LibOVRKernel\Src\GL\CAPI_GLE.h"

class VertexBuffer {
public:
	GLuint		buffer;

	VertexBuffer(void* vertices, size_t size);
	~VertexBuffer(void);
};
#endif // !VERTEX_BUFFER_H