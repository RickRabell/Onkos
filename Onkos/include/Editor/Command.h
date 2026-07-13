#pragma once
#include "Prerequisites.h"

/**
 * @class Command
 * @brief Abstract base class for the Command Pattern.
 * @author Ricardo Rabell
 * @date 2025-12-15
 *
 * Represents a command that can be executed, undone, and redone.
 * All editor operations (transform, create, delete, etc.) should inherit from this class.
 */
class Command {
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~Command() = default;

  /**
   * @brief Executes the command.
   * This is called when the command is first invoked.
   */
  virtual void execute() = 0;

  /**
   * @brief Undoes the command.
   * Reverts the state changed by execute() or redo().
   */
  virtual void undo() = 0;

  /**
   * @brief Redoes the command.
   * Re-applies the state changed by execute() after an undo().
   */
  virtual void redo() = 0;

  /**
   * @brief Gets a description of the command for UI/logging purposes.
   * @return A human-readable description of what this command does.
   */
  virtual const char* getDescription() const = 0;
};
