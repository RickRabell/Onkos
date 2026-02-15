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
	attach(Entity* child, Entity* parent);

	void
	detach(Entity* child);

	void
	update(float deltaTime, DeviceContext& deviceContext);

	void
	render(DeviceContext& deviceContext);

	void
	destroy();

private:
	void
	updateWorldRecursive(const EU::TSharedPointer<Entity>& node,
											 const XMMATRIX& parentWorld);

	bool
	isRoot(const EU::TSharedPointer<Entity>& e) const;

	bool
	isRegistered(Entity* entity) const;

private:
	std::vector<Entity*> m_entities;
};