#include <iostream>
#include <fstream>
#include "ObjectLoader.h"

int main(int argc, char** argv)
{
	InputMesh* importedMesh = ObjectLoader::CreateMeshFromOBJ("Assets/DesertEagle.obj");

	std::ofstream output;
	output.open("outputFile.txt", std::ios_base::trunc | std::ios_base::out);

	output.close();

	delete importedMesh;
	return 0;
}