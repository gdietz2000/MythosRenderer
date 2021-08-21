#include <iostream>
#include <fstream>
#include "ObjectLoader.h"

int main(int argc, char** argv)
{
	InputMesh* importedMesh = ObjectLoader::CreateMeshFromOBJ("Assets/DesertEagle.obj");

	std::ofstream output;
	output.open("outputFile.txt", std::ios_base::trunc | std::ios_base::out);

	for (int i = 0; i < importedMesh->m_Vertices.size(); ++i)
	{
		output << "vp " << importedMesh->m_Vertices[i].x << " " << importedMesh->m_Vertices[i].y << " " << importedMesh->m_Vertices[i].z << '\n';
	}

	for (int i = 0; i < importedMesh->m_Indices.size(); ++i)
	{
		output << "i " << importedMesh->m_Indices[i] << '\n';
	}

	output.close();

	delete importedMesh;
	return 0;
}