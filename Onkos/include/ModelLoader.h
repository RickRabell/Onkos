#pragma once
#include "Prerequisites.h"
#include "MeshComponent.h"

/**
 * @class ModelLoader
 * @brief A utility class for loading 3D model data from files.
 * @author Ricardo Rabell
 * @date 2025-11-05
 *
 * This class provides functionality to parse a 3D model file (like a .obj file)
 * and load its geometry data (vertices, texture coordinates, normals, and faces)
 * into a MeshComponent object.
 */
class ModelLoader {
public:
	/**
	 * @brief Default constructor.
	 */
	ModelLoader() = default;

	/**
	 * @brief Default destructor.
	 */
	~ModelLoader() = default;

	/**
	 * @brief Loads a model from a specified file and populates a MeshComponent.
	 * @param fileName The path to the model file (e.g., "myModel.obj").
	 * @param outMesh A reference to the MeshComponent object that will be
	 * populated with the loaded vertex and index data.
	 * @return bool Returns true if the model was loaded and parsed successfully,
	 * false otherwise.
	 */
	bool
		loadModel(const std::string& fileName, MeshComponent& outMesh);

	/**
	 * @brief A utility function to parse a 2-component vector (XMFLOAT2) from a string stream.
	 * @note This is likely a helper function for loadModel, used for parsing
	 * texture coordinates (vt) from a .obj file.
	 * @param streamLine The string stream containing the line of text to parse.
	 * @param outVector The vector where the parsed XMFLOAT2 will be stored.
	 */
	void
		parseVec2(std::stringstream& streamLine, std::vector<XMFLOAT2>& outVector);

	/**
	 * @brief A utility function to parse a 3-component vector (XMFLOAT3) from a string stream.
	 * @note This is likely a helper function for loadModel, used for parsing
	 * vertex positions (v) or normals (vn) from a .obj file.
	 * @param streamLine The string stream containing the line of text to parse.
	 * @param outVector The vector where the parsed XMFLOAT3 will be stored.
	 */
	void
		parseVec3(std::stringstream& streamLine, std::vector<XMFLOAT3>& outVector);
};