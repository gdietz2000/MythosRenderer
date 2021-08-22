#include <iostream>
#include <fstream>
#include "ObjectLoader.h"

int main(int argc, char** argv)
{
	ModelMesh* newMesh = ObjectLoader::CreateMeshFromOBJ("Assets/DesertEagle.obj");

	std::ofstream exportFile = std::ofstream("outputFile.txt", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	if (exportFile.is_open())
	{
		uint32_t indexCount = newMesh->GetIndexSize();
		uint32_t vertexCount = newMesh->GetVertexSize();
		exportFile.write((const char*)&indexCount, sizeof(uint32_t));
		exportFile.write((const char*)newMesh->m_Indices.data(), sizeof(uint32_t) * indexCount);
		exportFile.write((const char*)&vertexCount, sizeof(uint32_t));
		exportFile.write((const char*)newMesh->m_Vertices.data(), sizeof(ModelVertexInformation) * vertexCount);
	}

	exportFile.close();

	std::vector<ModelVertexInformation> m_Vertices;
	std::vector<int> m_Indices;

	std::fstream file{ "outputFile.txt", std::ios_base::in | std::ios_base::binary };

	if (!file.is_open())
		return 0;

	uint32_t player_index_count;
	file.read((char*)&player_index_count, sizeof(uint32_t));

	m_Indices.resize(player_index_count);

	file.read((char*)m_Indices.data(), sizeof(uint32_t) * player_index_count);

	uint32_t player_vertex_count;
	file.read((char*)&player_vertex_count, sizeof(uint32_t));

	m_Vertices.resize(player_vertex_count);

	file.read((char*)m_Vertices.data(), sizeof(ModelVertexInformation) * player_vertex_count);

	file.close();

	return 0;
}