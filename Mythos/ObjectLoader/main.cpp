#include <iostream>
#include <fstream>
#include "ObjectLoader.h"

int main(int argc, char** argv)
{
	//TempMesh* importedMesh = ObjectLoader::CreateMeshFromFBX("Assets/DesertEagle.fbx");
	FBX_InitLoad("Assets/DesertEagle.fbx", "outputFile.txt");
	/*std::ofstream output;
	output.open("outputFile.txt", std::ios_base::trunc | std::ios_base::out);

	for (int i = 0; i < importedMesh->m_VerticeIndices.size(); ++i)
	{
		output << "v " << importedMesh->m_Vertices[importedMesh->m_VerticeIndices[i]].x << ' ' << importedMesh->m_Vertices[importedMesh->m_VerticeIndices[i]].y << ' ' << importedMesh->m_Vertices[importedMesh->m_VerticeIndices[i]].z << '\n';
		output << "vt " << importedMesh->m_Uvs[importedMesh->m_UvIndices[i]].x << ' ' << importedMesh->m_Uvs[importedMesh->m_UvIndices[i]].y << '\n';
	}

	output.close();*/

	return 0;
}