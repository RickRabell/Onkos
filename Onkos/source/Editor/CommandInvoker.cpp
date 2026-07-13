/**
 * @file CommandInvoker.cpp
 * @brief Implementa la lógica del CommandInvoker para gestionar el historial de comandos.
 * @ingroup editor
 */
#include "Editor/CommandInvoker.h"

CommandInvoker::~CommandInvoker() {
  clear();
}

void CommandInvoker::executeCommand(EU::TSharedPointer<Command> command) {
  if (command.isNull()) return;

  // Execute the command
  command->execute();

  // Add to undo stack
  m_undoStack.push(command);

  // Clear redo stack when new command is executed
  while (!m_redoStack.empty()) {
    m_redoStack.pop();
  }

  // Enforce max history size
  while (m_undoStack.size() > m_maxHistorySize) {
    m_undoStack.pop();
  }
}

bool CommandInvoker::undo() {
  if (m_undoStack.empty()) return false;

  auto command = m_undoStack.top();
  m_undoStack.pop();

  if (!command.isNull()) {
    command->undo();
    m_redoStack.push(command);
    return true;
  }

  return false;
}

bool CommandInvoker::redo() {
  if (m_redoStack.empty()) return false;

  auto command = m_redoStack.top();
  m_redoStack.pop();

  if (!command.isNull()) {
    command->redo();
    m_undoStack.push(command);
    return true;
  }

  return false;
}

const char* CommandInvoker::getUndoDescription() const {
  if (m_undoStack.empty()) return "";
  return m_undoStack.top()->getDescription();
}

const char* CommandInvoker::getRedoDescription() const {
  if (m_redoStack.empty()) return "";
  return m_redoStack.top()->getDescription();
}

void CommandInvoker::clear() {
  while (!m_undoStack.empty()) {
    m_undoStack.pop();
  }
  while (!m_redoStack.empty()) {
    m_redoStack.pop();
  }
}
