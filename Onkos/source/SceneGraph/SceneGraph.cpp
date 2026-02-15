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
SceneGraph::destroy() {
	for (auto& e : m_entities) {
		if (!e) {
			continue;
		}
		auto hierarchy = e->getComponent<HierarchyComponent>();
		if (hierarchy) {
			hierarchy->getParent() == nullptr;
			hierarchy->getChildren();
		}
		m_entities.clear();
	}
}

void
	SceneGraph::addEntity(Entity* entity) {
	if (!entity) {
		return;
	}
	if (isRegistered(entity)) {
		return;
	}

	//	// Validar que existen los componentes minimos
	if (!entity->getComponent<Transform>()) {
		entity->addComponent(EU::MakeShared<Transform>());
		entity->getComponent<Transform>()->init();
	}
	if (!entity->getComponent<HierarchyComponent>()) {
		entity->addComponent(EU::MakeShared<HierarchyComponent>());
		entity->getComponent<HierarchyComponent>()->init();
	}

	m_entities.push_back(entity);
}

bool
SceneGraph::isRoot(const EU::TSharedPointer<Entity>& entity) const {
	auto hierarchy = entity->getComponent<HierarchyComponent>();

	if (!hierarchy) {
		return true;
	}

	return (hierarchy->getParent() == EU::TSharedPointer<Entity>());
}

bool
SceneGraph::isRegistered(Entity* entity) const {
	return std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end();
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