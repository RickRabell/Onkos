#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"

class DeviceContext;
class Entity;

/**
 * @class HierarchyComponent
 * @brief Grants an Entity the ability to participate in a parent-child relationship.
 * @author Ricardo Rabell
 * @date 2026-02-15
 *
 * Analogy: Think of a car model in a game. The "Car Body" is the parent entity,
 * and the "Wheels" are child entities. If you move the Car Body forward, the Wheels
 * must automatically move forward with it. This component holds the pointers that
 * link the Wheels to the Car Body. It stores relationships, not the math itself.
 */
class 
HierarchyComponent : public Component {
public:
	/**
	 * @brief Default constructor. Sets the component type to HIERARCHY.
	 */
	HierarchyComponent() : Component(ComponentType::HIERARCHY) {}

	/**
	 * @brief Default destructor.
	 */
	~HierarchyComponent() = default;

	/** @brief Lifecycle init. (Empty for hierarchy, as setup is done via API functions). */
	void 
	init() override {}

	/** @brief Lifecycle update. */
	void 
	update(float) override {}

	/** @brief Lifecycle render. */
	void 
	render(DeviceContext& deviceContext) override {}

	/**
	 * @brief Cleans up relationships before destruction to prevent dangling pointers.
	 */
	void 
	destroy() override {
		m_children.clear();
		m_parent = nullptr;
	}

	//--------------------------------------------------------------------------------------
	// SceneGraph API
	//--------------------------------------------------------------------------------------

	/**
	 * @brief Sets the parent of this entity.
	 * @param parent Pointer to the parent Entity.
	 */
	void 
	setParent(Entity* parent) {
		m_parent = parent;
	}

	/**
	 * @brief Checks if this entity is at the very top of a hierarchy chain.
	 * @return bool True if it has no parent.
	 */
	bool 
	isRoot() const {
		return m_parent == nullptr;
	}

	/**
	 * @brief Checks if this entity has dependents.
	 * @return bool True if the children list is not empty.
	 */
	bool 
	hasChildren() const {
		return !m_children.empty();
	}

	/**
	 * @brief Registers another entity as a child of this one.
	 * @note Prevents adding nullptrs and duplicate children.
	 * @param child Pointer to the child Entity.
	 */
	void 
	addChild(Entity* child) {
		if (!child) {
			return;
		}

		// Prevent duplicate children
		if (std::find(m_children.begin(), m_children.end(), child) != m_children.end()) {
			return;
		}

		m_children.push_back(child);
	}

	/**
	 * @brief Removes a specific child from this entity's list.
	 * @param child Pointer to the child Entity to remove.
	 */
	void 
	removeChild(Entity* child) {
		if (!child) return;

		m_children.erase(
			std::remove(m_children.begin(), m_children.end(), child),
			m_children.end()
		);
	}

public:
	/** @brief Pointer to the parent entity. Null if this is a root node. */
	Entity* m_parent = nullptr;

	/** @brief List of pointers to all immediate child entities. */
	std::vector<Entity*> m_children;
};