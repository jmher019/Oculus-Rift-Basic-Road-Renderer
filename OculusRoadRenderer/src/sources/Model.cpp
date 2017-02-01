#include "..\include\Model.h"

Model::Model(OVR::Vector3f pos, ShaderFill *fill) :
	numVertices(0),
	numIndices(0),
	pos(pos),
	rot(),
	mat(),
	fill(fill),
	vertexBuffer(nullptr),
	indexBuffer(nullptr) {
}

Model::~Model(void) {
	FreeBuffers();
}

OVR::Matrix4f& Model::GetMatrix(void) {
	mat = OVR::Matrix4f(rot);
	mat = OVR::Matrix4f::Translation(pos) * mat;
	return mat;
}

void Model::AddVertex(const Vertex& v) {
	vertices[numVertices++] = v;
}

void Model::AddIndex(GLushort a) {
	indices[numIndices++] = a;
}

void Model::AllocateBuffers(void) {
	vertexBuffer = new VertexBuffer(&vertices[0], numVertices * sizeof(vertices[0]));
	indexBuffer = new IndexBuffer(&indices[0], numIndices * sizeof(indices[0]));
}

void Model::FreeBuffers(void) {
	delete vertexBuffer; vertexBuffer = nullptr;
	delete indexBuffer; indexBuffer = nullptr;
}

void Model::AddSolidColorBox(float x1, float y1, float z1, float x2, float y2, float z2, DWORD c) {
	OVR::Vector3f Vert[][2] =
	{
		OVR::Vector3f(x1, y2, z1), OVR::Vector3f(z1, x1), OVR::Vector3f(x2, y2, z1), OVR::Vector3f(z1, x2),
		OVR::Vector3f(x2, y2, z2), OVR::Vector3f(z2, x2), OVR::Vector3f(x1, y2, z2), OVR::Vector3f(z2, x1),
		OVR::Vector3f(x1, y1, z1), OVR::Vector3f(z1, x1), OVR::Vector3f(x2, y1, z1), OVR::Vector3f(z1, x2),
		OVR::Vector3f(x2, y1, z2), OVR::Vector3f(z2, x2), OVR::Vector3f(x1, y1, z2), OVR::Vector3f(z2, x1),
		OVR::Vector3f(x1, y1, z2), OVR::Vector3f(z2, y1), OVR::Vector3f(x1, y1, z1), OVR::Vector3f(z1, y1),
		OVR::Vector3f(x1, y2, z1), OVR::Vector3f(z1, y2), OVR::Vector3f(x1, y2, z2), OVR::Vector3f(z2, y2),
		OVR::Vector3f(x2, y1, z2), OVR::Vector3f(z2, y1), OVR::Vector3f(x2, y1, z1), OVR::Vector3f(z1, y1),
		OVR::Vector3f(x2, y2, z1), OVR::Vector3f(z1, y2), OVR::Vector3f(x2, y2, z2), OVR::Vector3f(z2, y2),
		OVR::Vector3f(x1, y1, z1), OVR::Vector3f(x1, y1), OVR::Vector3f(x2, y1, z1), OVR::Vector3f(x2, y1),
		OVR::Vector3f(x2, y2, z1), OVR::Vector3f(x2, y2), OVR::Vector3f(x1, y2, z1), OVR::Vector3f(x1, y2),
		OVR::Vector3f(x1, y1, z2), OVR::Vector3f(x1, y1), OVR::Vector3f(x2, y1, z2), OVR::Vector3f(x2, y1),
		OVR::Vector3f(x2, y2, z2), OVR::Vector3f(x2, y2), OVR::Vector3f(x1, y2, z2), OVR::Vector3f(x1, y2)
	};

	GLushort CubeIndices[] =
	{
		0, 1, 3, 3, 1, 2,
		5, 4, 6, 6, 4, 7,
		8, 9, 11, 11, 9, 10,
		13, 12, 14, 14, 12, 15,
		16, 17, 19, 19, 17, 18,
		21, 20, 22, 22, 20, 23
	};

	for (int i = 0; i < sizeof(CubeIndices) / sizeof(CubeIndices[0]); ++i)
		AddIndex(CubeIndices[i] + GLushort(numVertices));

	// Generate a quad for each box face
	for (int v = 0; v < 6 * 4; v++) {
		// Make vertices, with some token lighting
		Vertex vvv; vvv.Pos = Vert[v][0]; vvv.U = Vert[v][1].x; vvv.V = Vert[v][1].y;
		float dist1 = (vvv.Pos - OVR::Vector3f(-2, 4, -2)).Length();
		float dist2 = (vvv.Pos - OVR::Vector3f(3, 4, -3)).Length();
		float dist3 = (vvv.Pos - OVR::Vector3f(-4, 3, 25)).Length();
		int   bri = rand() % 160;
		float B = ((c >> 16) & 0xff) * (bri + 192.0f * (0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
		float G = ((c >> 8) & 0xff) * (bri + 192.0f * (0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
		float R = ((c >> 0) & 0xff) * (bri + 192.0f * (0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
		vvv.C = (c & 0xff000000) +
			((R > 255 ? 255 : DWORD(R)) << 16) +
			((G > 255 ? 255 : DWORD(G)) << 8) +
			(B > 255 ? 255 : DWORD(B));
		AddVertex(vvv);
	}
}

void Model::Render(OVR::Matrix4f view, OVR::Matrix4f proj) {
	OVR::Matrix4f combined = proj * view * GetMatrix();

	glUseProgram(fill->program);
	glUniform1i(glGetUniformLocation(fill->program, "Texture0"), 0);
	glUniformMatrix4fv(glGetUniformLocation(fill->program, "matWVP"), 1, GL_TRUE, (FLOAT*)&combined);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fill->texture->texId);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->buffer);

	GLuint posLoc = glGetAttribLocation(fill->program, "Position");
	GLuint colorLoc = glGetAttribLocation(fill->program, "Color");
	GLuint uvLoc = glGetAttribLocation(fill->program, "TexCoord");

	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(colorLoc);
	glEnableVertexAttribArray(uvLoc);

	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, Pos));
	glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, C));
	glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, U));

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL);

	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(colorLoc);
	glDisableVertexAttribArray(uvLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}