#pragma once
#include "Prerequisites.h"

// Forward Declarations
class MeshComponent;

class
ModelLoader {
public:
	ModelLoader() = default;
	~ModelLoader() = default;

	bool 
	loadModel(const std::string& fileName, MeshComponent& outMesh);

	void
	parseVec2(std::stringstream& streamLine, std::vector<XMFLOAT2>& outVector);

	void
	parseVec3(std::stringstream& streamLine, std::vector<XMFLOAT3>& outVector);
};
