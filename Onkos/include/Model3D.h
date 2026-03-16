#pragma once
#include "Prerequisites.h"
#include "IResource.h"
#include "MeshComponent.h"
#include "fbxsdk.h"

/**
 * @enum ModelType
 * @brief Supported 3D model file formats.
 */
enum 
ModelType {
	OBJ, /**< Wavefront OBJ format. */
	FBX  /**< Autodesk FBX format. */
};

/**
 * @class Model3D
 * @brief Represents a 3D model resource.
 * @author Ricardo Rabell
 * @date 2025-11-20
 *
 * This class inherits from IResource and is responsible for loading, parsing,
 * and storing 3D geometry data. It specifically integrates the Autodesk FBX SDK
 * to handle complex model hierarchies, meshes, and material data.
 */
class 
Model3D : public IResource {
public:
	/**
	 * @brief Constructor.
	 * @param name The name of the model resource (also acts as the initial load path).
	 * @param modelType The format of the model (OBJ or FBX).
	 */
	Model3D(const std::string& name, ModelType modelType) : IResource(name), 
																													m_modelType(modelType), 
																													lSdkManager(nullptr), 
																													lScene(nullptr) {
		SetType(ResourceType::Model3D);
		load(name);
	}

	Model3D(const std::string& name,
					const SkyboxVertex vertices[],
					const unsigned int indices[]) : IResource(name) {
					MeshComponent mesh;
					mesh.m_skyVertex.assign(vertices, vertices + 8);
					mesh.m_index.assign(indices, indices + 36);
					SetType(ResourceType::Model3D);
					m_meshes.push_back(mesh);
	}

	/**
	 * @brief Default destructor.
	 */
	~Model3D() = default;

	/**
	 * @brief Loads the model from disk.
	 * Dispatches to the correct loader (e.g., LoadFBXModel) based on m_modelType.
	 * @param path The file path to the model.
	 * @return bool True if loaded successfully.
	 */
	bool
	load(const std::string& path) override;

	/**
	 * @brief Initializes the GPU resources (buffers) for the loaded meshes.
	 * @return bool True if initialization was successful.
	 */
	bool
	init() override;

	/**
	 * @brief Releases CPU and SDK memory associated with the model.
	 */
	void
	unload() override;

	/**
	 * @brief Calculates the memory usage of the model data.
	 * @return size_t Size in bytes.
	 */
	size_t
	getSizeInBytes() const override;

	/**
	 * @brief Retrieves the list of mesh components extracted from the model file.
	 * @return const std::vector<MeshComponent>& Reference to the mesh vector.
	 */
	const std::vector<MeshComponent>&
	GetMeshes() const { return m_meshes; }

	/* FBX MODEL LOADER METHODS */

	/**
	 * @brief Initializes the FBX SDK Manager.
	 * Required before loading any FBX files.
	 * @return bool True if the manager was created successfully.
	 */
	bool
	InitializeFBXManager();

	/**
	 * @brief Loads and parses an FBX file into MeshComponents.
	 * @param filePath Path to the FBX file.
	 * @return std::vector<MeshComponent> A vector of parsed meshes.
	 */
	std::vector<MeshComponent>
	LoadFBXModel(const std::string& filePath);

	/**
	 * @brief Recursively processes a node in the FBX scene graph.
	 * @param node The current FBX node to process.
	 */
	void
	ProcessFBXNode(FbxNode* node);

	/**
	 * @brief Extracts geometry data (vertices, indices) from an FBX node.
	 * @param node The FBX node containing mesh data.
	 */
	void
	ProcessFBXMesh(FbxNode* node);

	/**
	 * @brief Extracts material and texture information from an FBX material.
	 * @param material The FBX surface material to process.
	 */
	void
	ProcessFBXMaterials(FbxSurfaceMaterial* material);

	/**
	 * @brief Gets the file names of textures found within the model file.
	 * @return std::vector<std::string> List of texture filenames.
	 */
	std::vector<std::string>
	GetTextureFileNames() const { return textureFileNames; }

private:
	/** @brief Pointer to the Autodesk FBX SDK Manager. */
	FbxManager* lSdkManager;

	/** @brief Pointer to the FBX Scene object holding the model graph. */
	FbxScene* lScene;

	/** @brief Cached list of texture filenames associated with this model. */
	std::vector<std::string> textureFileNames;

public:
	/** @brief The file format type of this model instance. */
	ModelType m_modelType;

	/** @brief The collection of sub-meshes that make up this model. */
	std::vector<MeshComponent> m_meshes;
};