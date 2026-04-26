#pragma once
#include "Prerequisites.h"
#include "Buffer.h"

/**
 * @struct Submesh
 * @brief A localized chunk of geometry that uses a single Material Instance.
 * * @details
 * **Why it exists:** A 3D artist might export a Car as a single file. However,
 * the car has a metal body, rubber tires, and glass windows. A GPU draw call
 * can only use ONE material at a time. Therefore, the engine splits the Car into
 * three `Submesh`es.
 */
struct 
Submesh {
	Buffer vertexBuffer;          ///< GPU buffer holding vertices for this specific part.
	Buffer indexBuffer;           ///< GPU buffer holding the connection order of the vertices.
	unsigned int indexCount = 0;  ///< Number of indices to draw.
	unsigned int startIndex = 0;  ///< Offset into the index buffer.
	unsigned int materialSlot = 0;///< Maps this geometry to a specific MaterialInstance array index on the Actor.
};

/**
 * @class Mesh
 * @brief A container representing a complete 3D model, composed of one or more Submeshes.
 * @author Ricardo Rabell
 * @date 2026-04-25
 *
 * @details
 * **Context:** This is the top-level asset loaded from an OBJ or FBX file.
 * * **Analogy:** The `Mesh` is an entire action figure toy. The `Submesh`es are the
 * individual plastic pieces molded in the factory (an arm, the torso, a weapon)
 * that are glued together to form the whole.
 */
class 
Mesh {
public:
	/**
	 * @brief Retrieves the list of geometry parts.
	 * @return std::vector<Submesh>& Mutable reference to the submeshes.
	 */
	std::vector<Submesh>&
	getSubmeshes() { return m_submeshes; }

	/**
	 * @brief Retrieves the list of geometry parts (read-only).
	 * @return const std::vector<Submesh>&
	 */
	const std::vector<Submesh>&
	getSubmeshes() const { return m_submeshes; }

	/**
	 * @brief Releases GPU memory for all associated vertex and index buffers.
	 */
	void
	destroy() {
		for (Submesh& submesh : m_submeshes) {
			submesh.vertexBuffer.destroy();
			submesh.indexBuffer.destroy();
		}

		m_submeshes.clear();
	}

private:
	/** @brief The collection of geometry parts that make up this model. */
	std::vector<Submesh> m_submeshes;
};