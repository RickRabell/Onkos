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
	for (Entity* e : m_entities) {
		if (!e) {
			continue;
		}
		auto hierarchy = e->getComponent<HierarchyComponent>();
		if (hierarchy) {
			hierarchy->m_parent == nullptr;
			hierarchy->m_children.clear();
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

void
SceneGraph::removeEntity(Entity* entity) {
	if (!entity) return;
	if (!isRegistered(entity)) return;

	// 1) Detach from his parent (if any)
	detach(entity);

	// 2) Reparent of children to null (roots) or total detach
	auto h = entity->getComponent<HierarchyComponent>();
	if (h)
	{
		// Local copy to avoid invalidation while iterating
		auto childrenCopy = h->m_children;
		for (Entity* c : childrenCopy)
		{
			if (!c) continue;
			// Detach from parent (which is entity)
			auto hc = c->getComponent<HierarchyComponent>();
			if (hc && hc->m_parent == entity)
				hc->m_parent = nullptr;

			// Remove reference who is entity
			h->removeChild(c);

			// Mark Dirty to recalculate world
			auto wt = c->getComponent<Transform>();
			//if (wt) wt->dirty = true;
			//markWorldDirtyRecursive(wt);
		}

		h->m_children.clear();
	}

	// 3) Delete from register
	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), 
									 m_entities.end());
}

bool
SceneGraph::isAncestor(Entity* possibleAncestor, Entity* node) const {
	// It goes up from node: if it finds possibleAncestor, there is a cycle
	if (!possibleAncestor || !node) return false;

	auto h = node->getComponent<HierarchyComponent>();
	while (h && h->m_parent)
	{
		if (h->m_parent == possibleAncestor) return true;
		node = h->m_parent;
		EU::TSharedPointer<HierarchyComponent> h;
		if (node)
			h = node->getComponent<HierarchyComponent>();

	}
	return false;
}

bool
SceneGraph::isRoot(Entity* entity) const {

	if (!entity) return false;

	auto h = entity->getComponent<HierarchyComponent>();
	return (!h || h->m_parent == nullptr);
}

bool
SceneGraph::isRegistered(Entity* entity) const {
	return std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end();
}

bool
SceneGraph::attach(Entity* child, Entity* parent)
{
	if (!child || !parent) return false;
	if (child == parent) return false;

	// Automatic Register
	addEntity(child);
	addEntity(parent);

	// Avoids Cycles: parent cannot be under child
	if (isAncestor(child, parent)) return false;

	// If child already has a parent, detach first
	detach(child);

	auto hc = child->getComponent<HierarchyComponent>();
	auto hp = parent->getComponent<HierarchyComponent>();
	if (!hc || !hp) return false;

	hc->m_parent = parent;
	hp->addChild(child);

	//markWorldDirtyRecursive(wt);
	return true;
}

bool
SceneGraph::detach(Entity* child) {
	if (!child) return false;

	auto hc = child->getComponent<HierarchyComponent>();
	if (!hc) return false;

	Entity* parent = hc->m_parent;
	if (!parent) return true; // it was already detached

	auto hp = parent->getComponent<HierarchyComponent>();
	if (hp) hp->removeChild(child);

	hc->m_parent = nullptr;

	//markWorldDirtyRecursive(wt);
	return true;
}

void
SceneGraph::update(float deltaTime, DeviceContext& deviceContext) {
	// Update all entities
	for (Entity* e : m_entities)
	{
		if (!e) continue;
		e->update(deltaTime, deviceContext);
	}

	// 2) Propagación World: procesa roots
	for (Entity* e : m_entities)
	{
		if (!e) continue;
		if (isRoot(e))
		{
			updateWorldRecursive(e, XMMatrixIdentity());
		}
	}
}

void
SceneGraph::updateWorldRecursive(Entity* node, const XMMATRIX& parentWorld) {
	auto t = node->getComponent<Transform>();
	
	// Dirty Matrix
	auto h = node->getComponent<HierarchyComponent>();

	if (!t || !h) {
		return;
	}

	// Transform::matrix is LOCAL (S + R + T)
	// World = Local * ParentWorld
	auto worldMatrix = t->matrix * parentWorld;

	for (Entity* c : h->m_children) {
		updateWorldRecursive(c, worldMatrix);
	}
}

void SceneGraph::render(DeviceContext& deviceContext) {
	// Render all entities
	for (auto& e : m_entities) {
		if (e) {
			e->render(deviceContext);
		}
	}
}