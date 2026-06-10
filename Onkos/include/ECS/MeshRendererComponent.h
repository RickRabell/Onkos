#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"

// Forward Declarations
class Mesh;
class MaterialInstance;
class DeviceContext;

/**
 * @class MeshRendererComponent
 * @brief Links a 3D Mesh and its Material Instances to an Entity, making it visible.
 * @author Ricardo Rabell
 * @date 2026-05-13
 *
 * @details
 * **Context:** This is the visual "skin" of an Entity. Every frame, a "Render System" will
 * look for all entities that have both a `Transform` and a `MeshRendererComponent`. It
 * takes the matrices from the Transform, and the geometry/textures from this component,
 * packages them into a `RenderObject`, and sends them to the `RenderScene` queue.
 * * **Why it exists:** It decouples the abstract concept of a game object from its visual
 * representation. An invisible trigger-box entity won't have this component. A highly
 * detailed character will.
 * * **Why a vector of MaterialInstances?** As seen in `Mesh.h`, a single 3D model can be
 * composed of multiple `Submeshes` (e.g., a car's metal body and rubber tires). The vector
 * `m_materialInstances` maps perfectly to those submeshes. Index 0 of the material vector
 * is applied to Index 0 of the submesh vector, and so on.
 * * **Analogy:** If the Entity is a mannequin, the `Mesh` is the shape of the mannequin's
 * body, and the `MaterialInstances` are the clothes you put on it. This component holds
 * them both together.
 */
class 
MeshRendererComponent : public Component {

public:
	/** @brief Default constructor. Registers as a MESH component type in the ECS. */
	MeshRendererComponent()
		: Component(ComponentType::MESH) {
	}

	/** @brief Lifecycle init. Setup is usually handled via the setters. */
	void 
	init() override {}
	
	/** @brief Lifecycle update. Empty because visual data only updates when replaced. */
	void 
	update(float deltaTime) override {}
	
	/** @brief Lifecycle render. Empty because the ForwardRenderer handles actual D3D11 draw calls. */
	void 
	render(DeviceContext& deviceContext) override {}
	
	/** @brief Lifecycle destroy. Does not delete pointers, as they are owned by the ResourceManager. */
	void
	destroy() override {}

	/**
	 * @brief Assigns the 3D geometry to be drawn.
	 * @param mesh Pointer to the loaded Mesh asset.
	 */
	void 
	setMesh(Mesh* mesh) { m_mesh = mesh; }

	/** @brief Retrieves the assigned geometry. */
	Mesh* 
	getMesh() const { return m_mesh; }

	/**
	 * @brief Assigns a single material instance and clears any existing ones.
	 * @note Useful for simple objects that only have one Submesh (like a basic cube).
	 * @param materialInstance The material to apply to the entire mesh.
	 */
	void 
	setMaterialInstance(MaterialInstance* materialInstance) {
		m_materialInstance = materialInstance;
		m_materialInstances.clear();
		if (materialInstance) {
			m_materialInstances.push_back(materialInstance);
		}
	}

	/** @brief Retrieves the primary (or only) material instance. */
	MaterialInstance* 
	getMaterialInstance() const { return m_materialInstance; }

	/**
	 * @brief Overwrites the entire list of materials.
	 * @note The size of this vector should ideally match the number of Submeshes in the assigned Mesh.
	 * @param materialInstances Vector of materials to apply.
	 */
	void 
	setMaterialInstances(const std::vector<MaterialInstance*>& materialInstances) {
		m_materialInstances = materialInstances;
		m_materialInstance = m_materialInstances.empty() ? 
												 nullptr : m_materialInstances.front();
	}

	/**
	 * @brief Appends a new material instance to the list.
	 * @param materialInstance The material to add.
	 */
	void 
	addMaterialInstance(MaterialInstance* materialInstance) {
		if (!materialInstance) {
			return;
		}

		// Ensure the legacy single-pointer matches the first element
		if (!m_materialInstance) {
			m_materialInstance = materialInstance;
		}

		m_materialInstances.push_back(materialInstance);
	}

	/** @brief Retrieves the full list of materials applied to the submeshes. */
	const std::vector<MaterialInstance*>& 
	getMaterialInstances() const { return m_materialInstances; }

	/** @brief Checks if this entity should be submitted to the renderer. */
	bool 
	isVisible() const { return m_visible; }
	
	/** @brief Toggles the visibility of this entity. */
	void 
	setVisible(bool visible) { m_visible = visible; }

	/** @brief Checks if this geometry should block light in the shadow pass. */
	bool 
	canCastShadow() const { return m_castShadow; }
	
	/** @brief Toggles whether this geometry casts a shadow. */
	void 
	setCastShadow(bool value) { m_castShadow = value; }

private:
	/** @brief Pointer to the raw geometry data (vertices/indices). Owned by ResourceManager. */
	Mesh* m_mesh = nullptr;

	/** @brief Legacy pointer to the first material. Maintained for backwards compatibility. */
	MaterialInstance* m_materialInstance = nullptr;

	/** @brief Array of materials. Index corresponds to the Submesh index in the Mesh. */
	std::vector<MaterialInstance*> m_materialInstances;

	/** @brief Master switch to hide/show the entity without removing the component. */
	bool m_visible = true;

	/** @brief Switch to allow the object to render normally but be ignored by the shadow map. */
	bool m_castShadow = true;
};