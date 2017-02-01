#ifndef SCENE_H
#define SCENE_H
#include "Model.h"

class Scene {
public:
	int				numModels;
	Model*			models[10];

	Scene(void);
	Scene(bool includeIntensiveGPUobject);
	void Release(void);
	~Scene(void);
	void Add(Model *n);
	void Render(OVR::Matrix4f view, OVR::Matrix4f proj);
	GLuint CreateShader(GLenum type, const GLchar* src);
	void Init(int includeIntensiveGPUObject);
};
#endif // !SCENE_H