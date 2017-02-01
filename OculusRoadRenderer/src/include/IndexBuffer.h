#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H
#include "..\..\lib\LibOVRKernel\Src\GL\CAPI_GLE.h"

class IndexBuffer {
public:
	GLuint		buffer;
	
	IndexBuffer(void* indices, size_t);
	~IndexBuffer(void);
};
#endif // !INDEX_BUFFER_H
