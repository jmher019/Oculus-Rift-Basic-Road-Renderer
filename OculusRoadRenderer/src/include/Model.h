#ifndef MODEL_H
#define MODEL_H
#include "..\..\lib\LibOVRKernel\Src\GL\CAPI_GLE.h"
#include "..\..\lib\LibOVR\Include\Extras\OVR_Math.h"

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ShaderFill.h"

struct Vertex {
	OVR::Vector3f	Pos;
	DWORD		C;
	float		U, V;
};

class Model {
public:
	OVR::Vector3f		pos;
	OVR::Quatf			rot;
	OVR::Matrix4f		mat;
	int					numVertices, numIndices;
	Vertex				vertices[2000]; // Note fixed maximum
	GLushort			indices[2000];
	ShaderFill			*fill;
	VertexBuffer		*vertexBuffer;
	IndexBuffer			*indexBuffer;

	Model(OVR::Vector3f pos, ShaderFill *fill);
	~Model(void);

	OVR::Matrix4f& GetMatrix(void);
	void AddVertex(const Vertex& v);
	void AddIndex(GLushort a);
	void AllocateBuffers(void);
	void FreeBuffers(void);
	void AddSolidColorBox(float x1, float y1, float z1, float x2, float y2, float z2, DWORD c);
	void Render(OVR::Matrix4f view, OVR::Matrix4f proj);
};
#endif // !MODEL_H
