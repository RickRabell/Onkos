#pragma once
#include "Prerequisites.h"

class Entity;
class DeviceContext;

/**
* @class SceneGraph
* @brief Manages the global hierarchy and spatial propagation of all entities.
* @author Ricardo Rabell
* @date 2026-02-15
*
* While the `HierarchyComponent` stores *who* is connected to whom, the `SceneGraph`
* does the actual work of traversing those connections.
* * Context: Every frame, the SceneGraph finds all "Root" entities (objects with no parents)
* and calculates their World Matrix. Then, it cascades down the tree, multiplying
* the parent's World Matrix by the child's Local Matrix. This ensures that a character's
* hand moves correctly when the shoulder rotates.
*/
class 
SceneGraph {
public:
	/** @brief Default constructor. */
	SceneGraph() = default;

	/** @brief Default destructor. */
	~SceneGraph() = default;

	/** @brief Initializes the Scene Graph system. */
	void 
	init();

	/**
		* @brief Registers a new entity into the Scene Graph's master list.
		* @param entity The entity to add.
		*/
	void 
	addEntity(Entity* entity);

	/**
		* @brief Unregisters an entity from the graph, effectively removing it from the scene.
		* @param entity The entity to remove.
		*/
	void 
	removeEntity(Entity* entity);

	/**
		* @brief Verifies if an entity is an ancestor (parent, grandparent, etc.) of another.
		* Context: This is a crucial safety check used before attaching objects to prevent
		* "circular dependencies" (e.g., preventing a parent from becoming a child of its own child,
		* which would cause an infinite loop during matrix calculation).
		* @param possibleAncestor The node suspected to be higher in the chain.
		* @param node The node suspected to be lower in the chain.
		* @return bool True if the lineage exists.
		*/
	bool 
	isAncestor(Entity* possibleAncestor, Entity* node) const;

	/**
		* @brief Establishes a parent-child relationship between two entities.
		* @param child The entity that will follow the parent.
		* @param parent The entity that will lead the child.
		* @return bool True if successful, false if it failed (e.g., due to a circular dependency check).
		*/
	bool 
	attach(Entity* child, Entity* parent);

	/**
		* @brief Severs an entity from its parent, turning it into a root node.
		* @param child The entity to detach.
		* @return bool True if successful.
		*/
	bool 
	detach(Entity* child);

	/**
		* @brief The core of the Scene Graph. Triggers the recursive matrix updates.
		* @param deltaTime Time elapsed since last frame.
		* @param deviceContext D3D11 Device context.
		*/
	void 
	update(float deltaTime, DeviceContext& deviceContext);

	/**
		* @brief Initiates the rendering sequence for all registered entities.
		* @param deviceContext D3D11 Device context.
		*/
	void 
	render(DeviceContext& deviceContext);

	/**
		* @brief Clears the entire graph.
		*/
	void 
	destroy();

private:
	/**
		* @brief Recursively cascades matrix transformations down the hierarchy tree.
		* * Context: This function performs the core math:
		* `ChildWorldMatrix = ChildLocalMatrix * ParentWorldMatrix` (depending on API row/col major).
		* It ensures that spatial transformations accumulate properly through the lineage.
		* * @param node The current entity being processed.
		* @param parentWorld The pre-calculated World Matrix of this node's parent.
		*/
	void 
	updateWorldRecursive(Entity* node, const XMMATRIX& parentWorld);

	/**
		* @brief Checks if an entity is at the top level of the graph.
		* @param entity The entity to check.
		* @return bool True if it has no parent.
		*/
	bool 
	isRoot(Entity* entity) const;

	/**
		* @brief Checks if an entity is already tracked by the graph.
		* @param entity The entity to check.
		* @return bool True if it exists in m_entities.
		*/
	bool 
	isRegistered(Entity* entity) const;

public:
	/** @brief The master list of all entities currently managed by the scene. */
	std::vector<Entity*> m_entities;
};