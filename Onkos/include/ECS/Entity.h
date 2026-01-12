#pragma once
#include "Prerequisites.h"
#include "Component.h"

// Forward declaration
class DeviceContext;

/**
 * @class Entity
 * @brief Abstract base class for all game objects in the scene.
 * @author Ricardo Rabell
 * @date 2025-11-21
 *
 * An Entity acts as a container for Components. It manages the lifecycle
 * of its attached components (init, update, render, destroy) and provides
 * mechanisms to add and retrieve them.
 */
class 
Entity {
public:
	/**
	 * @brief Default constructor.
	 */
	Entity() = default;

	/**
	 * @brief Virtual destructor.
	 */
	virtual
	~Entity() = default;

	virtual void
	awake() = 0;

	/**
	 * @brief Initializes the entity and potentially its components.
	 */
	virtual void
	init() = 0;

	/**
	 * @brief Updates the entity and its components.
	 * @param deltaTime The time elapsed since the last frame.
	 * @param deviceContext The graphics device context.
	 */
	virtual void
	update(float deltaTime, DeviceContext& deviceContext) = 0;

	/**
	 * @brief Renders the entity.
	 * Typically delegates the render call to visual components (like MeshComponent).
	 * @param deviceContext The graphics device context.
	 */
	virtual void
	render(DeviceContext& deviceContext) = 0;

	/**
	 * @brief Destroys the entity and releases its components.
	 */
	virtual void
	destroy() = 0;

	/**
	 * @brief Adds a component to the entity.
	 * @tparam T The specific type of the component (must derive from Component).
	 * @param component A shared pointer to the component to add.
	 */
	template <typename T> 
	void
	addComponent(EU::TSharedPointer<T> component) {

		static_assert(std::is_base_of<Component, 
									T>::value,
									"T must be derived from Component");

		m_components.push_back(component.template dynamic_pointer_cast<Component>());
	}

	/**
	 * @brief Retrieves a component of a specific type.
	 * @tparam T The type of component to retrieve.
	 * @return EU::TSharedPointer<T> A shared pointer to the component if found, otherwise an empty pointer.
	 */
	template<typename T>
	EU::TSharedPointer<T>
	getComponent() {

		for (auto& component : m_components) {
			EU::TSharedPointer<T> specificComponent = component.template dynamic_pointer_cast<T>();
			
			if (specificComponent) {
				return specificComponent;
			}
		}

		return EU::TSharedPointer<T>();
	}

protected:
	/** @brief Flag indicating if the entity is active in the scene. */
	bool m_isActive;

	/** @brief Unique identifier for the entity. */
	int m_id;

	/** @brief List of components attached to this entity. */
	std::vector<EU::TSharedPointer<Component>> m_components;
};