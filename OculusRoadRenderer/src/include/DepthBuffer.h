#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H
// Include the Oculus SDK
#include "..\..\lib\LibOVR\Include\OVR_CAPI_GL.h"

#include "..\..\lib\LibOVRKernel\Src\GL\CAPI_GLE.h"
#include "..\..\lib\LibOVR\Include\Extras\OVR_Math.h"
#include <assert.h>

class DepthBuffer {
public:
	GLuint texId;

	DepthBuffer(OVR::Sizei size, int sampleCount);
	~DepthBuffer(void);
};
#endif // !DEPTH_BUFFER_H