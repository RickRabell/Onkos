#include "ModelLoader.h"
#include <fstream>

bool 
ModelLoader::loadModel(const std::string& fileName, MeshComponent& outMesh) {
	std::ifstream file(fileName);

	if (!file.is_open()) {
		ERROR("ModelLoader.cpp", "loadModel", "The file couldn't be opened.");
		
		return false;
	}

	std::string line;
	std::vector<XMFLOAT3> temp_vertexes;
	std::vector<XMFLOAT2> temp_uvs;
	std::vector<XMFLOAT3> temp_normals;

	while (std::getline(file, line)) {
		std::stringstream streamLine(line);
		std::string prefix;
		streamLine >> prefix;

		if (prefix == "vt") {
			parseVec2(streamLine, temp_uvs);
		}
		else if (prefix == "vn") {
			parseVec3(streamLine, temp_normals);
		}
		else if (prefix == "v ") {
			parseVec3(streamLine, temp_vertexes);
		}
		else if (prefix == "f ") {

		}
	}
}

void 
ModelLoader::parseVec2(std::stringstream& streamLine, std::vector<XMFLOAT2>& outVector) {
	XMFLOAT2 uv;
	streamLine >> uv.x >> uv.y;

	outVector.push_back(uv);
}

void 
ModelLoader::parseVec3(std::stringstream& streamLine, std::vector<XMFLOAT3>& outVector) {
	XMFLOAT3 pos;
	streamLine >> pos.x >> pos.y >> pos.z;

	outVector.push_back(pos);
}
