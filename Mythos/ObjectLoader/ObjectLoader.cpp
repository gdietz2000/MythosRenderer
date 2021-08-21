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

	while (true)
	{
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "o") == 0) {
			newMesh->m_Meshes.push_back(std::vector<Math::Vector3>());
			newMesh->m_MeshIndices.push_back(std::vector<int>());
			currentMesh++;
		}

		if (strcmp(lineHeader, "v") == 0) {
			Math::Vector3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			newMesh->m_Meshes[currentMesh].push_back(vertex);
		}

		if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndices[3], uvIndices[3], normalIndices[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndices[0], &uvIndices[0], &normalIndices[0], &vertexIndices[1], &uvIndices[1], &normalIndices[1], &vertexIndices[2], &uvIndices[2], &normalIndices[2]);
			if (matches != 9) 
			{
				printf("Cannot parse this object with this simple parser");
				delete newMesh;
				return nullptr;
			}
			newMesh->m_MeshIndices[currentMesh].push_back(vertexIndices[0] - 1);
			newMesh->m_MeshIndices[currentMesh].push_back(vertexIndices[1] - 1);
			newMesh->m_MeshIndices[currentMesh].push_back(vertexIndices[2] - 1);
		}
	}

	return newMesh;
}