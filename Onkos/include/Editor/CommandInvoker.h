#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"
#include <stack>

/**
 * @class CommandInvoker
 * @brief Manages the command history for Undo/Redo functionality.
 * @author Ricardo Rabell
 * @date 2025-12-15
 *
 * The CommandInvoker maintains two stacks: one for undo operations and one for redo operations.
 * When a command is executed, it's pushed to the undo stack and the redo stack is cleared.
 * Undo moves commands from undo stack to redo stack, and redo moves them back.
 */
class CommandInvoker {
public:
  /**
   * @brief Default constructor.
   * Initializes empty undo and redo stacks.
   */
  CommandInvoker() = default;

  /**
   * @brief Destructor.
   * Clears all stacks.
   */
  ~CommandInvoker();

  /**
   * @brief Executes and records a command for undo/redo.
   * Clears the redo stack when a new command is executed.
   * @param command The command to execute.
   */
  void executeCommand(EU::TSharedPointer<Command> command);

  /**
   * @brief Undoes the last command.
   * Moves the last command from undo stack to redo stack.
   * @return true if undo was successful, false if undo stack is empty.
   */
  bool undo();

  /**
   * @brief Redoes the last undone command.
   * Moves a command from redo stack back to undo stack.
   * @return true if redo was successful, false if redo stack is empty.
   */
  bool redo();

  /**
   * @brief Checks if undo is available.
   * @return true if there are commands to undo.
   */
  bool canUndo() const { return !m_undoStack.empty(); }

  /**
   * @brief Checks if redo is available.
   * @return true if there are commands to redo.
   */
  bool canRedo() const { return !m_redoStack.empty(); }

  /**
   * @brief Gets the description of the next undo operation.
   * @return Description string, or empty string if no undo available.
   */
  const char* getUndoDescription() const;

  /**
   * @brief Gets the description of the next redo operation.
   * @return Description string, or empty string if no redo available.
   */
  const char* getRedoDescription() const;

  /**
   * @brief Gets the maximum number of commands to store (history limit).
   * @return The max command history size.
   */
  size_t getMaxHistorySize() const { return m_maxHistorySize; }

  /**
   * @brief Sets the maximum number of commands to store.
   * @param maxSize The new maximum size.
   */
  void setMaxHistorySize(size_t maxSize) { m_maxHistorySize = maxSize; }

  /**
   * @brief Gets the current number of undo commands.
   * @return The size of the undo stack.
   */
  size_t getUndoStackSize() const { return m_undoStack.size(); }

  /**
   * @brief Gets the current number of redo commands.
   * @return The size of the redo stack.
   */
  size_t getRedoStackSize() const { return m_redoStack.size(); }

  /**
   * @brief Clears all command history.
   */
  void clear();

private:
  /** @brief Stack of executed commands for undo. */
  std::stack<EU::TSharedPointer<Command>> m_undoStack;

  /** @brief Stack of undone commands for redo. */
  std::stack<EU::TSharedPointer<Command>> m_redoStack;

  /** @brief Maximum number of commands to keep in history. */
  size_t m_maxHistorySize = 100;
};
