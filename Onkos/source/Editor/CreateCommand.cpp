/**
 * @file CreateCommand.cpp
 * @brief Implementa la lógica de CreateCommand para crear actores de forma reversible.
 * @ingroup editor
 */
#include "Editor/CreateCommand.h"
#include "ECS/Actor.h"
#include "ECS/Transform.h"
#include "Device.h"
#include "SceneGraph/SceneGraph.h"

CreateCommand::CreateCommand(
  const std::string& actorName,
  const EU::Vector3& position,
  Device* device,
  SceneGraph* sceneGraph
)
  : m_actorName(actorName)
  , m_position(position)
  , m_device(device)
  , m_sceneGraph(sceneGraph)
  , m_actor(nullptr)
  , m_isCreated(false)
{
}

void CreateCommand::execute() {
  if (!m_device || !m_sceneGraph) return;

  // Create a new actor
  m_actor = EU::MakeShared<Actor>(*m_device);
  if (m_actor.isNull()) return;

  m_actor->setName(m_actorName);

  // Add transform component
  auto transform = EU::MakeShared<Transform>();
  if (!transform.isNull()) {
    transform->init();
    transform->setPosition(m_position);
    m_actor->addComponent(transform);
  }

  // Initialize the actor
  m_actor->awake();
  m_actor->init();

  // Add to scene
  m_sceneGraph->addEntity(m_actor.get());
  m_isCreated = true;
}

void CreateCommand::undo() {
  if (m_actor.isNull() || !m_sceneGraph) return;

  m_sceneGraph->removeEntity(m_actor.get());
  m_isCreated = false;
}

void CreateCommand::redo() {
  if (m_actor.isNull() || !m_sceneGraph) return;

  m_sceneGraph->addEntity(m_actor.get());
  m_isCreated = true;
}
