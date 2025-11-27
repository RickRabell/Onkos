#include "Model3D.h"

bool
Model3D::initializeFBXManager() {
	// Se maneja como un árbol binario un FBX
	// Initialize the FBX SDK manager
	lSdkManager = FbxManager::Create();
	if(!lSdkManager) {
		ERROR("Model3D", "FbxManager::Create()", "Unable to create FBX Manager");
		return false;
	} 
	else {
		MESSAGE("Model3D", "ModelLoader", "Autodesk FBX SDK version " << lSdkManager->GetVersion());
	}

	// Create an IDSettings object
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an FBX Scene
	lScene = FbxScene::Create(lSdkManager, "My Scene");
	if(!lScene) {
		ERROR("Model3D", "FbxScene::Create()", "Unable to create FBX Scene");
		return false;
	}
	else {
		MESSAGE("Model3D", "ModelLoader", "FBX Scene created successfully");
	}
	
	return true;
}

std::vector<MeshComponent>
Model3D::loadFBXModel(const std::string& filePath) {
	// 01. Initialize the SDK from FBX Manager
	if (initializeFBXManager()) {
		// 02. Create an importer using the SDK Manager
		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
		if (!lImporter) {
			ERROR("Model3D", "FbxImporter::Create()", "Unable to create FBX Importer");
			return std::vector<MeshComponent>();
		}
		else {
			MESSAGE("Model3D", "ModelLoader", "FBX Importer created successfully");
		}

		// 03. Use the first argument as the filename for the inspector
		if (!lImporter->Initialize(filePath.c_str(), -1, lSdkManager->GetIOSettings())) {
			ERROR("Model3D", "FbxImporter::Initialize()", 
				    "Unable to initialize FBX Importer with file: Error: " << lImporter->GetStatus().GetErrorString());
			lImporter->Destroy();
			return std::vector<MeshComponent>();
		}
		else {
			MESSAGE("Model3D", "ModelLoader", "FBX Importer initialized successfully.");
		}

		// 04. Import the scene from the file into the scene
		if (!lImporter->Import(lScene)) {
			ERROR("Model3D", "FbxImporter::Import()", 
					  "Unable to import FBX Scene: ERROR: " << lImporter->GetStatus().GetErrorString());
			lImporter->Destroy();
			return std::vector<MeshComponent>();
		}
		else {
			MESSAGE("Model3D", "ModelLoader", "FBX Scene imported successfully.");
			m_name = lImporter->GetFileName();
		}

		FbxAxisSystem::DirectX.ConvertScene(lScene);
		FbxSystemUnit::m.ConvertScene(lScene);
		FbxGeometryConverter gc(lSdkManager);
		gc.Triangulate(lScene, /*replace*/ true);

		// 05. Destroy the Importer
		lImporter->Destroy();
		MESSAGE("Model3D", "ModelLoader", "FBX Importer destroyed successfully.");

		// 06. Process the model from the scene
		FbxNode* lRootNode = lScene->GetRootNode();

		if(lRootNode) {
			MESSAGE("Model3D", "ModelLoader", "Processing model from the scene root node.");
			for (int i = 0; i < lRootNode->GetChildCount(); i++) {
				processFBXNode(lRootNode->GetChild(i));
			}
			return m_meshes;
		}
		else {
			ERROR("Model3D", "FbxScene::GetRootNode()", 
						"Unable to get root node from FBX Scene.");
			return std::vector<MeshComponent>();
		}
	}

	return m_meshes;
}