#include "..\include\IndexBuffer.h"

IndexBuffer::IndexBuffer(void* indices, size_t size) {
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer(void) {
	if (buffer) {
		glDeleteBuffers(1, &buffer);
		buffer = 0;
	}
}