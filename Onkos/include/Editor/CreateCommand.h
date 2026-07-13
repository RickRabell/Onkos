#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"
#include "EngineUtilities/Vectors/Vector3.h"

// Forward declarations
class Actor;
class Device;
class SceneGraph;
class MeshComponent;

/**
 * @class CreateCommand
 * @brief Command for creating a new actor in the scene.
 * @author Ricardo Rabell
 * @date 2025-12-15
 *
 * Creates an empty actor or a mesh actor with the specified parameters.
 */
class 
CreateCommand : public Command {
public:
  /**
   * @brief Constructor for creating an empty actor.
   * @param actorName The name for the new actor.
   * @param position The initial position.
   * @param device The graphics device (for mesh actors).
   * @param sceneGraph The scene graph for adding the actor.
   */
  CreateCommand(const std::string& actorName,
                const EU::Vector3& position,
                Device* device,
                SceneGraph* sceneGraph);

  /**
   * @brief Destructor.
   */
  ~CreateCommand() override = default;

  /**
   * @brief Executes the command by creating and adding the actor to the scene.
   */
  void 
  execute() override;

  /**
   * @brief Undoes the command by removing the created actor.
   */
  void 
  undo() override;

  /**
   * @brief Redoes the command by recreating the actor.
   */
  void 
  redo() override;

  /**
   * @brief Gets the command description.
   * @return "Create Actor".
   */
  const char* 
  getDescription() const override { return "Create Actor"; }

  /**
   * @brief Gets the created actor.
   * @return Shared pointer to the created actor (may be null if not yet created).
   */
  EU::TSharedPointer<Actor> 
  getCreatedActor() const { return m_actor; }

private:
  /** @brief Name of the actor to be created. */
  std::string m_actorName;
  /** @brief Initial position of the actor. */
  EU::Vector3 m_position;
  /** @brief Graphics device (for mesh actors). */
  Device* m_device;
  /** @brief Scene graph for adding the actor. */
  SceneGraph* m_sceneGraph;
  /** @brief Shared pointer to the created actor. */
  EU::TSharedPointer<Actor> m_actor;
  /** @brief Flag indicating whether the actor has been created. */
  bool m_isCreated;
};
