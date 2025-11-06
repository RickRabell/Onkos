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
	std::vector<XMFLOAT3> tempVertexes;
	std::vector<XMFLOAT2> tempUvs;
	std::vector<XMFLOAT3> tempNormals;
	std::map<std::string, int> uniqueVertexes;

	while (std::getline(file, line)) {
		std::stringstream streamLine(line);
		std::string prefix;
		streamLine >> prefix;

		if (prefix == "vt") {
			parseVec2(streamLine, tempUvs);
		}
		else if (prefix == "vn") {
			parseVec3(streamLine, tempNormals);
		}
		else if (prefix == "v") {
			parseVec3(streamLine, tempVertexes);
		}
		else if (prefix == "f") {

			std::string chunk;
			std::vector<int> faceIndexes;

			// For each vertex in the face (the entire line after 'f ')
			while (streamLine >> chunk) {
				
				int finalIndex = 0;

				if (uniqueVertexes.count(chunk)) {
					// Vertex already exists, use the existing index
					finalIndex = uniqueVertexes[chunk];
				}
				else {

					int posIdx, txtIdx, nrmIdx;

					// Extract position, texture, and normal indices from the line of the vertex data
					std::stringstream ss_vertexData(chunk);
					std::string idxData;

					std::getline(ss_vertexData, idxData, '/');
					posIdx = std::stoi(idxData) - 1;

					std::getline(ss_vertexData, idxData, '/');
					txtIdx = std::stoi(idxData) - 1;

					std::getline(ss_vertexData, idxData, '/');
					nrmIdx = std::stoi(idxData) - 1;

					SimpleVertex newVertex;
					newVertex.Pos = tempVertexes[posIdx];
					newVertex.Tex = tempUvs[txtIdx];
					//newVertex.Norm = tempNormals[nrm_idx];

					outMesh.m_vertex.push_back(newVertex);

					int newIndex = static_cast<int>(outMesh.m_vertex.size()) - 1;
					uniqueVertexes[chunk] = newIndex;

					finalIndex = newIndex;
				}
				// Add the index to the temporal face vector
				faceIndexes.push_back(finalIndex);
			}
			// Triangulate the face (assuming it's a quad or triangle)
			if (faceIndexes.size() == 3) {
				for (int idx : faceIndexes) {
					outMesh.m_index.push_back(idx);
				}
			}
			else if (faceIndexes.size() == 4) {
				for (int i = 0; i < 3; i++) {
					outMesh.m_index.push_back(faceIndexes[i]);
				}
				outMesh.m_index.push_back(faceIndexes[0]);
				outMesh.m_index.push_back(faceIndexes[2]);
				outMesh.m_index.push_back(faceIndexes[3]);
			}
		}
	}

	outMesh.m_numVertex = static_cast<int>(outMesh.m_vertex.size());
	outMesh.m_numIndex = static_cast<int>(outMesh.m_index.size());

	file.close();

	return true;
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
