#pragma once
#include "Prerequisites.h"
#include "IResource.h"
#include "MeshComponent.h"
#include "fbxsdk.h"

enum ModelType {
	OBJ,
	FBX
};

class Model3D : public IResource {
public:
	Model3D(const std::string& name, ModelType modelType);
	~Model3D() = default;

	bool 
	load(const std::string& filePath) override;
	
	bool
	init() override;

	void
	unload() override;

	size_t 
	getSizeInBytes() const override;

	const std::vector<MeshComponent>&
	getMeshes() const { return m_meshes; }

	// FBX Model Loader
	bool
	initializeFBXManager();

	std::vector<MeshComponent>
	loadFBXModel(const std::string& filePath);

	void
	processFBXNode(FbxNode* node);

	void
	processFBXMesh(FbxNode* node);

	void
	processFBXMaterials(FbxSurfaceMaterial* material);

	std::vector<std::string>
	getTextureFileNames() const { return textureFileNames; }
	
private:
	FbxManager* lSdkManager;
	FbxScene* lScene;
	std::vector<std::string> textureFileNames;
public:
	ModelType m_modelType;
	std::vector<MeshComponent> m_meshes;
};