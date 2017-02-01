#ifndef TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H
// Include the Oculus SDK
#include "..\..\lib\LibOVR\Include\OVR_CAPI_GL.h"

#include "..\..\lib\LibOVRKernel\Src\GL\CAPI_GLE.h"
#include "..\..\lib\LibOVR\Include\Extras\OVR_Math.h"
#include "..\include\DepthBuffer.h"
#include <assert.h>

class TextureBuffer {
public:
	ovrSession				session;
	ovrTextureSwapChain		textureChain;
	GLuint					texId;
	GLuint					fboId;
	OVR::Sizei				texSize;
	
	TextureBuffer(ovrSession session, bool renderTarget, bool displayableOnHmd, OVR::Sizei size, int mipLevels, unsigned char * data, int sampleCount);
	~TextureBuffer(void);
	void SetAndClearRenderSurface(DepthBuffer* dbuffer);
	void UnsetRenderSurface(void);
	void Commit(void);
};
#endif // !TEXTURE_BUFFER_H