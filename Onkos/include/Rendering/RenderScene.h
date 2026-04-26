#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Skybox;

class 
RenderScene {
public:
	void
	clear();

private:
	std::vector<RenderObject> m_opaqueObjects;
	std::vector<RenderObject> m_transparentObjects;
	std::vector<LightData> m_directionalLights;
	Skybox* skybox = nullptr;
};
