#include "ObjectLoader.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

InputMesh* ObjectLoader::CreateMeshFromOBJ(const char* filepath)
{
	FILE* file = fopen(filepath, "r");
	if (file == NULL) {
		printf("Couldn't find file :/");
		return nullptr;
	}
	else
		printf("Successfully opened the file!");

	InputMesh* newMesh = new InputMesh();

	int currentMesh = -1;

	int oCount = 0;

	while (oCount != 2)
	{
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "o") == 0) {
			oCount++;
		}

		else if (strcmp(lineHeader, "v") == 0) {
			Math::Vector3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			newMesh->m_Vertices.push_back(vertex);
		}

		else if (strcmp(lineHeader, "vt") == 0) {
			Math::Vector2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			newMesh->m_Uvs.push_back(uv);
		}

		else if (strcmp(lineHeader, "vn") == 0) {
			Math::Vector3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			newMesh->m_Normals.push_back(normal);
		}

		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndices[3], uvIndices[3], normalIndices[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndices[0], &uvIndices[0], &normalIndices[0], &vertexIndices[1], &uvIndices[1], &normalIndices[1], &vertexIndices[2], &uvIndices[2], &normalIndices[2]);
			if (matches != 9) 
			{
				printf("Cannot parse this object with this simple parser");
				delete newMesh;
				return nullptr;
			}
			newMesh->m_VertexIndices.push_back(vertexIndices[0] - 1);
			newMesh->m_VertexIndices.push_back(vertexIndices[1] - 1);
			newMesh->m_VertexIndices.push_back(vertexIndices[2] - 1);
			
			newMesh->m_UvIndices.push_back(uvIndices[0] - 1);
			newMesh->m_UvIndices.push_back(uvIndices[1] - 1);
			newMesh->m_UvIndices.push_back(uvIndices[2] - 1);
			
			newMesh->m_NormalIndices.push_back(normalIndices[0] - 1);
			newMesh->m_NormalIndices.push_back(normalIndices[1] - 1);
			newMesh->m_NormalIndices.push_back(normalIndices[2] - 1);
		}
	}

	return newMesh;
}