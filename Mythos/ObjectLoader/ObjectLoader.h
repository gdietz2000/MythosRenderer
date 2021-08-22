#pragma once

#include <vector>
#include "Vector2.h"
#include "Vector3.h"

struct ModelVertexInformation
{
	Math::Vector3 m_Vertex;
	Math::Vector2 m_Uvs;
};


struct ModelMesh
{
	std::vector<ModelVertexInformation> m_Vertices;
	std::vector<int> m_Indices;
	int GetVertexSize() { return m_Vertices.size(); }
	int GetIndexSize() { return m_Indices.size(); }
};

struct Model
{
	std::vector<ModelMesh*> m_Meshes;
};

struct ObjectLoader
{
public:
	static Model* CreateMeshFromOBJ(const char* filepath);
private:
	static void CompactMeshData(ModelMesh* mesh);

	static int GetObjectCount(FILE* file);
};