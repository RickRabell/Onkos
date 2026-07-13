/**
 * @file TransformCommand.cpp
 * @brief Implementa la lógica de TransformCommand para operaciones de transformación reversibles.
 * @ingroup editor
 */
#include "Editor/TransformCommand.h"
#include "ECS/Actor.h"
#include "ECS/Transform.h"

TransformCommand::TransformCommand(
  EU::TSharedPointer<Actor> actor,
  const EU::Vector3& newPosition,
  const EU::Vector3& newRotation,
  const EU::Vector3& newScale,
  const char* operationType
)
  : m_actor(actor)
  , m_newPosition(newPosition)
  , m_newRotation(newRotation)
  , m_newScale(newScale)
  , m_operationType(operationType)
  , m_oldPosition()
  , m_oldRotation()
  , m_oldScale()
{
  if (!actor.isNull()) {
    auto transform = actor->getComponent<Transform>();
    if (!transform.isNull()) {
      m_oldPosition = transform->getPosition();
      m_oldRotation = transform->getRotation();
      m_oldScale = transform->getScale();
    }
  }
}

void TransformCommand::execute() {
  if (m_actor.isNull()) return;

  auto transform = m_actor->getComponent<Transform>();
  if (transform.isNull()) return;

  transform->setPosition(m_newPosition);
  transform->setRotation(m_newRotation);
  transform->setScale(m_newScale);
}

void TransformCommand::undo() {
  if (m_actor.isNull()) return;

  auto transform = m_actor->getComponent<Transform>();
  if (transform.isNull()) return;

  transform->setPosition(m_oldPosition);
  transform->setRotation(m_oldRotation);
  transform->setScale(m_oldScale);
}

void TransformCommand::redo() {
  execute();
}

const char* TransformCommand::getDescription() const {
  return m_operationType.c_str();
}
