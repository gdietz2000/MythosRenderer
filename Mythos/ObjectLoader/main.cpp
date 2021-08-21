#include <iostream>
#include <fstream>
#include "ObjectLoader.h"

int main(int argc, char** argv)
{
	InputMesh* importedMesh = ObjectLoader::CreateMeshFromOBJ("Assets/DesertEagle.obj");

	std::ofstream output;
	output.open("outputFile.txt", std::ios_base::trunc | std::ios_base::out);


	output << "nm\n";

	for (int i = 0; i < importedMesh->m_Meshes.size(); ++i)
	{
		for (int j = 0; j < importedMesh->m_Meshes[i].size(); ++j)
		{
			output << "vp " << importedMesh->m_Meshes[i][j].x << " " << importedMesh->m_Meshes[i][j].y << " " << importedMesh->m_Meshes[i][j].z << '\n';

		}

		for (int j = 0; j < importedMesh->m_MeshIndices[i].size(); ++j)
		{
			output << "i " << importedMesh->m_MeshIndices[i][j] << '\n';
		}

		output << "nm\n";
	}



	output.close();

	delete importedMesh;
	return 0;
}