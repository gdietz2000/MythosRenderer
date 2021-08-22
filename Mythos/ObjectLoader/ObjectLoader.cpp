#include "ObjectLoader.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#include <fstream>

Model* ObjectLoader::CreateMeshFromOBJ(const char* filepath)
{
	FILE* modelFile = fopen(filepath, "r");
	if (modelFile == nullptr)
		return nullptr;

	int objCount = GetObjectCount(modelFile);

	fclose(modelFile);
	modelFile = fopen(filepath, "r");
	if (modelFile == nullptr)
		return nullptr;
	
	Model* model = new Model();
	model->m_Meshes.resize(objCount);

	int vertexIndexSub = 0;
	int uvIndexSub = 0;
	int objectCounter = 0;

	for (int i = 0; i < objCount; ++i)
	{
		ModelMesh* modelMesh = new ModelMesh();

		std::vector<Math::Vector3> outVertices;
		std::vector<Math::Vector2> outUvs;

		std::vector<Math::Vector3> tempVertices;
		std::vector<Math::Vector2> tempUvs;
		std::vector<Math::Vector3> tempNormals;
		std::vector<int> vertexIndices, uvIndices, normalIndices;



		while (true)
		{
			char lineHeader[128];

			int res = fscanf(modelFile, "%s", lineHeader);
			if (res == EOF)
				break;

			//Parse Line Header
			if (strcmp(lineHeader, "o") == 0)
			{
				objectCounter++;
				if (objectCounter != 1)
				{
					break;
				}
			}


			else if (strcmp(lineHeader, "v") == 0)
			{
				Math::Vector3 vertex;
				fscanf(modelFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				tempVertices.push_back(vertex);
			}

			else if (strcmp(lineHeader, "vt") == 0) {
				Math::Vector2 uvs;
				fscanf(modelFile, "%f %f", &uvs.x, &uvs.y);
				tempUvs.push_back(uvs);
			}

			else if (strcmp(lineHeader, "vn") == 0) {
				Math::Vector3 normals;
				fscanf(modelFile, "%f %f %f", &normals.x, &normals.y, &normals.z);
				tempNormals.push_back(normals);
			}

			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(modelFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return nullptr;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		for (int j = 0; j < vertexIndices.size(); ++j) {
			unsigned int vertexIndex = vertexIndices[i];
			Math::Vector3 vertex = tempVertices[vertexIndex - 1 - vertexIndexSub];
			outVertices.push_back(vertex);
		}

		for (int j = 0; j < uvIndices.size(); ++j) {
			unsigned int uvIndex = uvIndices[j];
			Math::Vector2 uv = tempUvs[uvIndex - 1 - uvIndexSub];
			outUvs.push_back(uv);
		}

		for (int j = 0; j < vertexIndices.size(); ++j) {
			ModelVertexInformation data;
			data.m_Vertex = outVertices[j];
			data.m_Uvs = outUvs[j];
			modelMesh->m_Vertices.push_back(data);
		}

		vertexIndexSub += tempVertices.size();
		uvIndexSub += tempUvs.size();

		CompactMeshData(modelMesh);
		model->m_Meshes[i] = modelMesh;
	}

	return model;
}

void ObjectLoader::CompactMeshData(ModelMesh* mesh)
{
	std::vector<ModelVertexInformation> vertices;
	std::vector<int> indices;


	int index = 0;
	for (int i = 0; i < mesh->GetVertexSize(); ++i)
	{
		std::pair<bool, int> foundVertex = std::make_pair(false, 0);
		for (int j = 0; j < vertices.size(); ++j)
		{
			if (mesh->m_Vertices[i].m_Vertex == vertices[j].m_Vertex && mesh->m_Vertices[i].m_Uvs == vertices[j].m_Uvs) {
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
			vertices.push_back(mesh->m_Vertices[i]);
			indices.push_back(index++);
		}
	}

	mesh->m_Vertices = vertices;
	mesh->m_Indices = indices;
}

int ObjectLoader::GetObjectCount(FILE* file)
{
	int objectCount = 0;

	while(true)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "o") == 0)
			objectCount++;
	}

	return objectCount;
}