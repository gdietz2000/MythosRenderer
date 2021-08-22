#include <iostream>
#include <fstream>
#include "ObjectLoader.h"

int main(int argc, char** argv)
{
	InputMesh* importedMesh = ObjectLoader::CreateMeshFromOBJ("Assets/Deagle.obj");

	std::ofstream output;
	output.open("outputFile.txt", std::ios_base::trunc | std::ios_base::out);

	for (int i = 0; i < importedMesh->m_VerticeIndices.size(); ++i)
	{
		output << "v " << importedMesh->m_Vertices[importedMesh->m_VerticeIndices[i]].x << ' ' << importedMesh->m_Vertices[importedMesh->m_VerticeIndices[i]].y << ' ' << importedMesh->m_Vertices[importedMesh->m_VerticeIndices[i]].z << '\n';
		output << "vt " << importedMesh->m_Uvs[importedMesh->m_UvIndices[i]].x << ' ' << importedMesh->m_Uvs[importedMesh->m_UvIndices[i]].y << '\n';
	}

	output.close();

	delete importedMesh;
	return 0;
}