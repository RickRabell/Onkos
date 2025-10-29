#pragma once
#include "Prerequisites.h"

// Forward declarations
class DeviceContext;

/**
 * @class MeshComponent
 * @brief A data component holding CPU-side geometry.
 * @author Ricardo Rabell
 * @date 2025-10-17
 *
 * This class represents the raw geometry for a 3D model, storing the list of
 * vertices and indices in system memory (on the CPU). It acts as a data
 * container that is typically used by a `Buffer` to create GPU-side vertex
 * and index buffers. It is likely part of an Entity-Component System (ECS)
 * architecture.
 */
class 
MeshComponent {
public:
	/**
	 * @brief Default constructor. Initializes vertex and index counts to zero.
	 */
	MeshComponent() : m_numVertex(0), m_numIndex(0) {}

	/**
	 * @brief Virtual destructor.
	 */
	virtual
	~MeshComponent() = default;

	/**
	 * @brief Initializes the mesh component.
	 * @note Can be used to load mesh data from a file (e.g., .obj, .fbx)
	 * or to procedurally generate geometry.
	 */
	void
	init();

	/**
	 * @brief Per-frame update logic for the mesh.
	 * @note Useful for CPU-side procedural mesh manipulation or animation.
	 * @param deltaTime Time elapsed since the last frame.
	 */
	void
	update(float deltaTime);

	/**
	 * @brief Issues the final draw call for this mesh.
	 * @note This method assumes that the associated Vertex and Index buffers
	 * have already been bound to the pipeline by another system. It simply
	 * calls DrawIndexed using the stored index count.
	 * @param deviceContext The device context to issue the draw command.
	 */
	void
	render(DeviceContext& deviceContext);

	/**
	 * @brief Clears the CPU-side vertex and index data.
	 */
	void
	destroy();

public:
	/** @brief An identifier name for the mesh (e.g., "cube", "sphere_mesh"). */
	std::string m_name;

	/** @brief The list of vertices (position, texcoords) stored in system memory. */
	std::vector<SimpleVertex> m_vertex;

	/** @brief The list of indices stored in system memory. */
	std::vector<unsigned int> m_index;

	/** @brief Cached count of the number of vertices. */
	int m_numVertex;

	/** @brief Cached count of the number of indices. This is used for draw calls. */
	int m_numIndex;
};