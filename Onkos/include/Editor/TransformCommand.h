#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"
#include "EngineUtilities/Vectors/Vector3.h"

// Forward declarations
class Actor;

/**
 * @class TransformCommand
 * @brief Command for transforming (move, rotate, scale) an actor.
 * @author Ricardo Rabell
 * @date 2025-12-15
 *
 * Captures the initial state of an actor's transform, allows modification,
 * and can undo/redo those changes.
 */
class TransformCommand : public Command {
public:
  /**
   * @brief Constructor.
   * @param actor The actor to transform.
   * @param newPosition The new position.
   * @param newRotation The new rotation (in radians).
   * @param newScale The new scale.
   * @param operationType A string describing the operation (e.g., "Move", "Rotate", "Scale").
   */
  TransformCommand(
    EU::TSharedPointer<Actor> actor,
    const EU::Vector3& newPosition,
    const EU::Vector3& newRotation,
    const EU::Vector3& newScale,
    const char* operationType = "Transform"
  );

  /**
   * @brief Destructor.
   */
  ~TransformCommand() override = default;

  /**
   * @brief Executes the command by applying the new transform.
   */
  void execute() override;

  /**
   * @brief Undoes the command by restoring the old transform.
   */
  void undo() override;

  /**
   * @brief Redoes the command by reapplying the new transform.
   */
  void redo() override;

  /**
   * @brief Gets the command description.
   * @return A description like "Move", "Rotate", "Scale", etc.
   */
  const char* getDescription() const override;

private:
  EU::TSharedPointer<Actor> m_actor;
  EU::Vector3 m_oldPosition;
  EU::Vector3 m_oldRotation;
  EU::Vector3 m_oldScale;
  EU::Vector3 m_newPosition;
  EU::Vector3 m_newRotation;
  EU::Vector3 m_newScale;
  std::string m_operationType;
};
