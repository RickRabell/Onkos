#pragma once
#include "Prerequisites.h"

// Forward declaration
class DeviceContext;

/**
 * @class Component
 * @brief Abstract base class for all components attached to entities.
 * @author Ricardo Rabell
 * @date 2025-11-21
 *
 * In the Entity-Component System (ECS), a component represents a modular piece
 * of data or logic (e.g., a Mesh, a Script, a Transform). This interface enforces
 * the lifecycle methods that all components must implement.
 */
class 
Component {
public:
	/**
	 * @brief Default constructor.
	 */
	Component() = default;

	/**
	 * @brief Constructor with specific type.
	 * @param type The specific ComponentType enum identifier.
	 */
	Component(const ComponentType type) : m_type(type) {}

	/**
	 * @brief Virtual destructor.
	 */
	virtual
	~Component() = default;

	/**
	 * @brief Initializes the component.
	 * Called once when the component is created or added to an entity.
	 */
	virtual void
	init() = 0;

	/**
	 * @brief Updates the component logic.
	 * Called every frame.
	 * @param deltaTime The time elapsed since the last frame (in seconds).
	 */
	virtual void
	update(float deltaTime) = 0;

	/**
	 * @brief Handles rendering-specific operations.
	 * @param deviceContext The device context used for drawing or binding resources.
	 */
	virtual void
	render(DeviceContext& deviceContext) = 0;

	/**
	 * @brief Releases resources associated with the component.
	 */
	virtual void
	destroy() = 0;

	/**
	 * @brief Gets the type identifier of this component.
	 * @return ComponentType The enum value representing the component type.
	 */
	ComponentType
	getType() const { return m_type; }

protected:
	/** @brief The type identifier for this component instance. */
	ComponentType m_type;
};