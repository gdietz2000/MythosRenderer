#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <vector>

struct InputMesh
{
	std::vector<Math::Vector3> m_Vertices;
	std::vector<Math::Vector2> m_Uvs;
	std::vector<int> m_VerticeIndices;
	std::vector<int> m_UvIndices;
};

class ObjectLoader
{
public:
	static InputMesh* CreateMeshFromOBJ(const char* filepath);
	static InputMesh* CreateMeshFromFBX(const char* filepath);
};