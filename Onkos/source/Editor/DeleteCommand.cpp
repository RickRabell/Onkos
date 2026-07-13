/**
 * @file DeleteCommand.cpp
 * @brief Implementa la lógica de DeleteCommand para eliminar actores de forma reversible.
 * @ingroup editor
 */
#include "Editor/DeleteCommand.h"
#include "ECS/Actor.h"
#include "SceneGraph/SceneGraph.h"

DeleteCommand::DeleteCommand(EU::TSharedPointer<Actor> actor, SceneGraph* sceneGraph)
  : m_actor(actor)
  , m_sceneGraph(sceneGraph)
  , m_isDeleted(false)
{
}

void DeleteCommand::execute() {
  if (m_actor.isNull() || !m_sceneGraph) return;

  m_sceneGraph->removeEntity(m_actor.get());
  m_isDeleted = true;
}

void DeleteCommand::undo() {
  if (m_actor.isNull() || !m_sceneGraph) return;

  m_sceneGraph->addEntity(m_actor.get());
  m_isDeleted = false;
}

void DeleteCommand::redo() {
  execute();
}
