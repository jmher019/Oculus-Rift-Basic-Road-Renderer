#include "..\include\ShaderFill.h"

ShaderFill::ShaderFill(GLuint vertexShader, GLuint pixelShader, TextureBuffer* texture) {
	this->texture = texture;

	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDetachShader(program, pixelShader);

	GLint r;
	glGetProgramiv(program, GL_LINK_STATUS, &r);
	if (!r)
	{
		GLchar msg[1024];
		glGetProgramInfoLog(program, sizeof(msg), 0, msg);
		OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
	}
}

ShaderFill::~ShaderFill(void) {
	if (program) {
		glDeleteProgram(program);
		program = 0;
	}

	if (texture) {
		delete texture;
		texture = nullptr;
	}
}