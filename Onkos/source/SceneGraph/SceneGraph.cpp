#include "SceneGraph/SceneGraph.h"
#include "SceneGraph/HierarchyComponent.h"
#include "ECS/Entity.h"
#include "ECS/Transform.h"
#include "DeviceContext.h"

void
SceneGraph::init() {
	m_entities.clear();
}

void
SceneGraph::addEntity(const EU::TSharedPointer<Entity>& entity) {
	if (!entity) {
		return;
	}

	// Register if not exists
	for (auto& it : m_entities) {
		if (it == entity) {
			return;
		}
	}

	m_entities.push_back(entity);

	// Validate that exists the minimum components
	if (!entity->getComponent<Transform>()) {
		entity->addComponent(EU::MakeShared<Transform>());
		entity->getComponent<Transform>()->init();
	}

	if (!entity->getComponent<HierarchyComponent>()) {
		entity->addComponent(EU::MakeShared<HierarchyComponent>());
		entity->getComponent<HierarchyComponent>()->init();
	}
}

void
SceneGraph::attach(const EU::TSharedPointer<Entity>& child,
									 const EU::TSharedPointer<Entity>& parent) {
	if (!child || !parent || child == parent) {
		return;
	}

	addEntity(child);
	addEntity(parent);

	auto childHierarchy = child->getComponent<HierarchyComponent>();
	auto parentHierarchy = parent->getComponent<HierarchyComponent>();

	// Detach from previous parent if any
	if (auto oldParent = childHierarchy->getParent()) {
		detach(child);
	}

	// Set new parent
	childHierarchy->setParent(parent);
	parentHierarchy->addChild(child);

	// Update child's transform relative to new parent - Dirty World Transform
}

void
SceneGraph::detach(const EU::TSharedPointer<Entity>& child) {
	if (!child) {
		return;
	}

	auto childHierarchy = child->getComponent<HierarchyComponent>();
	if (!childHierarchy) {
		return;
	}

	auto parent = childHierarchy->getParent();
	if (parent) {
		auto parentHierarchy = parent->getComponent<HierarchyComponent>();
		if (parentHierarchy) {
			parentHierarchy->removeChild(child);
		}
	}

	childHierarchy->setParent(EU::TSharedPointer<Entity>());

	// Update chil's transform relative to new parent - Dirty World Transform
}

bool
SceneGraph::isRoot(const EU::TSharedPointer<Entity>& entity) const {
	auto hierarchy = entity->getComponent<HierarchyComponent>();

	if (!hierarchy) {
		return true;
	}

	return (hierarchy->getParent() == EU::TSharedPointer<Entity>());
}

void
SceneGraph::update(float deltaTime, DeviceContext& deviceContext) {
	// Update all entities
	for (auto& e : m_entities) {
		if (e) {
			e->update(deltaTime, deviceContext);
		}
	}

	// Update the global transformations with recursivity
	for (auto& e : m_entities) {
		if (e && isRoot(e)) {
			XMMATRIX identity = XMMatrixIdentity();
			updateWorldRecursive(e, identity);
		}
	}
}

void
SceneGraph::updateWorldRecursive(const EU::TSharedPointer<Entity>& node,
																 const XMMATRIX& parentWorld) {
	auto t = node->getComponent<Transform>();

	// Dirty Matrix
	auto h = node->getComponent<HierarchyComponent>();

	if (!t || !h) {
		return;
	}

	// Transform::matrix is LOCAL (S + R + T)
	// World = Local * ParentWorld
	auto worldMatrix = t->matrix * parentWorld;

	for (auto& wChild : h->getChildren()) {
		auto child = wChild.lock(); // The lock is to avoid duplicates
		if (child) {
			updateWorldRecursive(child, worldMatrix);
		}
	}
}

void
SceneGraph::render(DeviceContext& deviceContext) {
	// Render all entities
	for (auto& e : m_entities) {
		if (e) {
			e->render(deviceContext);
		}
	}
}