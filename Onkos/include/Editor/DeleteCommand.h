#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"
#include "EngineUtilities/Vectors/Vector3.h"
#include "Rendering/Mesh.h"
#include "Rendering/Material.h"

// Forward declarations
class Actor;
class Device;
class SceneGraph;

/**
 * @class DeleteCommand
 * @brief Command for deleting an actor from the scene.
 * @author Ricardo Rabell
 * @date 2025-12-15
 *
 * Stores actor data and can restore the actor if undo is called.
 */
class 
DeleteCommand : public Command {
public:
  /**
   * @brief Constructor that captures the actor to delete.
   * @param actor The actor to delete.
   * @param sceneGraph The scene graph for adding/removing the actor.
   */
  DeleteCommand(EU::TSharedPointer<Actor> actor, SceneGraph* sceneGraph);

  /**
   * @brief Destructor.
   */
  ~DeleteCommand() override = default;

  /**
   * @brief Executes the command by removing the actor from the scene.
   */
  void 
  execute() override;

  /**
   * @brief Undoes the command by restoring the actor to the scene.
   */
  void 
  undo() override;

  /**
   * @brief Redoes the command by removing the actor again.
   */
  void 
  redo() override;

  /**
   * @brief Gets the command description.
   * @return "Delete Actor".
   */
  const char* 
  getDescription() const override { return "Delete Actor"; }

private:
  /** @brief Shared pointer to the actor to be deleted. */
  EU::TSharedPointer<Actor> m_actor;
  /** @brief Scene graph for adding/removing the actor. */
  SceneGraph* m_sceneGraph;
  /** @brief Flag indicating whether the actor has been deleted. */
  bool m_isDeleted;
};
