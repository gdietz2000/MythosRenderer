#include "ObjectLoader.h"

#include <fbxsdk.h>

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

	while (true)
	{
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		else if (strcmp(lineHeader, "v") == 0) {
			Math::Vector3 temp;
			fscanf(file, "%f %f %f", &temp.x, &temp.y, &temp.z);
			newMesh->m_Vertices.push_back(temp);
		}

		else if (strcmp(lineHeader, "vt") == 0) {
			Math::Vector2 temp;
			fscanf(file, "%f %f", &temp.x, &temp.y);
			newMesh->m_Uvs.push_back(temp);
		}

		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
			}
			newMesh->m_VerticeIndices.push_back(vertexIndex[0] - 1);
			newMesh->m_VerticeIndices.push_back(vertexIndex[1] - 1);
			newMesh->m_VerticeIndices.push_back(vertexIndex[2] - 1);
			
			newMesh->m_UvIndices.push_back(uvIndex[0] - 1);
			newMesh->m_UvIndices.push_back(uvIndex[1] - 1);
			newMesh->m_UvIndices.push_back(uvIndex[2] - 1);
		}
	}

	return newMesh;
}

InputMesh* ObjectLoader::CreateMeshFromFBX(const char* filepath)
{
	InputMesh* newMesh = new InputMesh();
	return newMesh;
}