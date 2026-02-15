#pragma once
#include "Prerequisites.h"

class Entity;
class DeviceContext;

class 
SceneGraph {
public:
	SceneGraph() = default;
	~SceneGraph() = default;

	void
	init();

	void
	addEntity(Entity* entity); // Registers in the Graph

	void
	removeEntity(Entity* entity);

	bool
	isAncestor(Entity* possibleAncestor, Entity* node) const;

	bool
	attach(Entity* child, Entity* parent);

	bool
	detach(Entity* child);

	void
	update(float deltaTime, DeviceContext& deviceContext);

	void
	render(DeviceContext& deviceContext);

	void
	destroy();

private:
	void
	updateWorldRecursive(Entity* node,
											 const XMMATRIX& parentWorld);

	bool
	isRoot(Entity* entity) const;

	bool
	isRegistered(Entity* entity) const;

private:
	std::vector<Entity*> m_entities;
};