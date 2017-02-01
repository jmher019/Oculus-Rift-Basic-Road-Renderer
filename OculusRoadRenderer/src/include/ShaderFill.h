#ifndef SHADER_FILL_H
#define SHADER_FILL_H
#include "TextureBuffer.h"

#ifndef OVR_DEBUG_LOG
	#define OVR_DEBUG_LOG(x)
#endif

class ShaderFill {
public:
	GLuint			program;
	TextureBuffer	*texture;

	ShaderFill(GLuint vertexShader, GLuint pixelShader, TextureBuffer* texture);
	~ShaderFill(void);
};
#endif // !SHADER_FILL_H
