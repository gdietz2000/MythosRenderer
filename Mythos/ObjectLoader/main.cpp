#include <iostream>
#include <fstream>
#include "ObjectLoader.h"

int main(int argc, char** argv)
{
	Model* newMesh = ObjectLoader::CreateMeshFromOBJ("Assets/DesertEagle.obj");

	std::ofstream exportFile = std::ofstream("DesertEagle.txt", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	if (exportFile.is_open())
	{
		uint32_t modelCount = newMesh->m_Meshes.size();
		exportFile.write((const char*)&modelCount, sizeof(uint32_t));

		for (int i = 0; i < newMesh->m_Meshes.size(); ++i)
		{
			uint32_t indexCount = newMesh->m_Meshes[i]->GetIndexSize();
			uint32_t vertexCount = newMesh->m_Meshes[i]->GetVertexSize();
			exportFile.write((const char*)&indexCount, sizeof(uint32_t));
			exportFile.write((const char*)newMesh->m_Meshes[i]->m_Indices.data(), sizeof(uint32_t) * indexCount);
			exportFile.write((const char*)&vertexCount, sizeof(uint32_t));
			exportFile.write((const char*)newMesh->m_Meshes[i]->m_Vertices.data(), sizeof(VertexInfo) * vertexCount);
		}
	}

	exportFile.close();

	std::fstream file{ "DesertEagle.txt", std::ios_base::in | std::ios_base::binary };

	if (!file.is_open())
		return 0;

	Model* importedModel = new Model();

	uint32_t modelCount;
	file.read((char*)&modelCount, sizeof(uint32_t));

	importedModel->m_Meshes.resize(modelCount);

	for (int i = 0; i < modelCount; ++i)
	{
		std::vector<VertexInfo> m_Vertices;
		std::vector<int> m_Indices;

		uint32_t player_index_count;
		file.read((char*)&player_index_count, sizeof(uint32_t));

		m_Indices.resize(player_index_count);

		file.read((char*)m_Indices.data(), sizeof(uint32_t) * player_index_count);

		uint32_t player_vertex_count;
		file.read((char*)&player_vertex_count, sizeof(uint32_t));

		m_Vertices.resize(player_vertex_count);

		file.read((char*)m_Vertices.data(), sizeof(VertexInfo) * player_vertex_count);

		ModelMesh* mesh = new ModelMesh();

		mesh->m_Indices = m_Indices;
		mesh->m_Vertices = m_Vertices;

		importedModel->m_Meshes[i] = mesh;
	}

	file.close();

	delete newMesh;

	return 0;
}