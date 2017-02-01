#include "..\include\VertexBuffer.h"

VertexBuffer::VertexBuffer(void* vertices, size_t size) {
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer(void) {
	if (buffer) {
		glDeleteBuffers(1, &buffer);
		buffer = 0;
	}
}