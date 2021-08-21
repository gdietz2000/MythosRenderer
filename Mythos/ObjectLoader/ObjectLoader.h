#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <vector>

struct InputMesh
{
	std::vector<std::vector<Math::Vector3>> m_Meshes;
	std::vector<std::vector<int>> m_MeshIndices;
};

struct OutputMesh
{
	std::vector<Math::Vector3> m_Vertices;
	std::vector<int> m_Indices;
};

class ObjectLoader
{
public:
	static InputMesh* CreateMeshFromOBJ(const char* filepath);
	static InputMesh* CreateMeshFromFBX(const char* filepath);
};