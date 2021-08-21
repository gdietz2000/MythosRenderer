#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <vector>

struct Vertex
{
	Math::Vector3 position;
	Math::Vector2 uv;
	Math::Vector3 normal;
};

struct InputMesh
{
	std::vector<Math::Vector3> m_Vertices;
	std::vector<Math::Vector2> m_Uvs;
	std::vector<Math::Vector3> m_Normals;
	std::vector<int> m_VertexIndices, m_UvIndices, m_NormalIndices;
};

struct OutputMesh
{
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
};

class ObjectLoader
{
public:
	static InputMesh* CreateMeshFromOBJ(const char* filepath);
	static InputMesh* CreateMeshFromFBX(const char* filepath);
	static void SplitMesh(Vertex* mesh, size_t meshSize, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
	{
		int index = 0;
		for (int i = 0; i < (int)meshSize; i++)
		{
			std::pair<bool, int> foundVertex = std::make_pair(false, 0);
			for (int j = 0; j < (int)vertices.size(); j++)
			{
				if (mesh[i].position == vertices[j].position && mesh[i].uv == vertices[j].uv && mesh[i].normal == vertices[j].normal) {
					foundVertex = std::make_pair(true, j);
					break;
				}
			}

			if (foundVertex.first)
			{
				indices.push_back(foundVertex.second);
			}
			else
			{
				vertices.push_back(mesh[i]);
				indices.push_back(index++);
			}
		}
	}
};