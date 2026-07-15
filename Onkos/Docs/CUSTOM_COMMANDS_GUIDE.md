# Guía: Extender el Sistema de Undo/Redo

Esta guía te muestra cómo crear nuevos tipos de comandos personalizados para el sistema de Undo/Redo.

## Paso 1: Crear el Archivo Header

Crea `include/Editor/MyCustomCommand.h`:

```cpp
#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"

// Forward declarations si es necesario
class MyClass;

/**
 * @class MyCustomCommand
 * @brief Comando personalizado para operación X.
 * @author Tu Nombre
 * @date 2025-12-15
 *
 * Descripción de qué hace este comando.
 */
class MyCustomCommand : public Command {
public:
    /**
     * @brief Constructor.
     * @param targetObject El objeto a modificar.
     * @param newValue El nuevo valor.
     */
    MyCustomCommand(EU::TSharedPointer<MyClass> targetObject, int newValue);

    /**
     * @brief Destructor.
     */
    ~MyCustomCommand() override = default;

    /**
     * @brief Executa el comando.
     */
    void execute() override;

    /**
     * @brief Deshace el comando.
     */
    void undo() override;

    /**
     * @brief Rehace el comando.
     */
    void redo() override;

    /**
     * @brief Obtiene la descripción del comando.
     */
    const char* getDescription() const override { return "My Custom Operation"; }

private:
    EU::TSharedPointer<MyClass> m_target;
    int m_oldValue;
    int m_newValue;
};
```

## Paso 2: Implementar el Archivo Source

Crea `source/Editor/MyCustomCommand.cpp`:

```cpp
/**
 * @file MyCustomCommand.cpp
 * @brief Implementa la lógica de MyCustomCommand.
 * @ingroup editor
 */
#include "Editor/MyCustomCommand.h"
#include "MyClass.h"  // Incluye la clase del objeto a modificar

MyCustomCommand::MyCustomCommand(EU::TSharedPointer<MyClass> targetObject, int newValue)
    : m_target(targetObject)
    , m_newValue(newValue)
    , m_oldValue(0)
{
    if (!targetObject.isNull()) {
        m_oldValue = targetObject->getValue();  // Captura el estado actual
    }
}

void MyCustomCommand::execute() {
    if (m_target.isNull()) return;

    m_target->setValue(m_newValue);  // Aplica el nuevo valor
}

void MyCustomCommand::undo() {
    if (m_target.isNull()) return;

    m_target->setValue(m_oldValue);  // Restaura el valor antiguo
}

void MyCustomCommand::redo() {
    execute();  // Simplemente ejecuta de nuevo
}
```

## Paso 3: Usar el Comando

En el código donde quieras usar el comando:

```cpp
#include "Editor/CommandInvoker.h"
#include "Editor/MyCustomCommand.h"

// Obtener el invocador de comandos
CommandInvoker& invoker = baseApp.getCommandInvoker();

// Crear el comando
auto command = EU::MakeShared<MyCustomCommand>(myObject, newValue);

// Ejecutarlo (también lo agrega al historial)
invoker.executeCommand(command.template dynamic_pointer_cast<Command>());
```

## Ejemplo 1: Cambiar Nombre de Actor

### Header (include/Editor/RenameActorCommand.h):

```cpp
#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"

class Actor;

class RenameActorCommand : public Command {
public:
    RenameActorCommand(
        EU::TSharedPointer<Actor> actor,
        const std::string& newName
    );

    ~RenameActorCommand() override = default;

    void execute() override;
    void undo() override;
    void redo() override;
    const char* getDescription() const override { return "Rename Actor"; }

private:
    EU::TSharedPointer<Actor> m_actor;
    std::string m_oldName;
    std::string m_newName;
};
```

### Implementación (source/Editor/RenameActorCommand.cpp):

```cpp
#include "Editor/RenameActorCommand.h"
#include "ECS/Actor.h"

RenameActorCommand::RenameActorCommand(
    EU::TSharedPointer<Actor> actor,
    const std::string& newName
)
    : m_actor(actor)
    , m_newName(newName)
{
    if (!actor.isNull()) {
        m_oldName = actor->getName();
    }
}

void RenameActorCommand::execute() {
    if (m_actor.isNull()) return;
    m_actor->setName(m_newName);
}

void RenameActorCommand::undo() {
    if (m_actor.isNull()) return;
    m_actor->setName(m_oldName);
}

void RenameActorCommand::redo() {
    execute();
}
```

## Ejemplo 2: Cambiar Propiedades de Componente

```cpp
#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"
#include "EngineUtilities/Vectors/Vector3.h"

class Actor;
class LightComponent;

class ChangeLightColorCommand : public Command {
public:
    ChangeLightColorCommand(
        EU::TSharedPointer<Actor> actor,
        const EU::Vector3& newColor
    );

    ~ChangeLightColorCommand() override = default;

    void execute() override;
    void undo() override;
    void redo() override;
    const char* getDescription() const override { return "Change Light Color"; }

private:
    EU::TSharedPointer<Actor> m_actor;
    EU::Vector3 m_oldColor;
    EU::Vector3 m_newColor;
};
```

## Ejemplo 3: Operación Compuesta (Macro)

Para operaciones que requieren múltiples comandos, puedes crear un contenedor:

```cpp
#pragma once
#include "Prerequisites.h"
#include "Editor/Command.h"
#include <vector>

class MacroCommand : public Command {
public:
    MacroCommand(const std::string& description)
        : m_description(description) {}

    ~MacroCommand() override = default;

    void addCommand(EU::TSharedPointer<Command> command) {
        m_commands.push_back(command);
    }

    void execute() override {
        for (auto& cmd : m_commands) {
            if (!cmd.isNull()) cmd->execute();
        }
    }

    void undo() override {
        // Deshacer en orden inverso
        for (int i = static_cast<int>(m_commands.size()) - 1; i >= 0; --i) {
            if (!m_commands[i].isNull()) {
                m_commands[i]->undo();
            }
        }
    }

    void redo() override {
        execute();
    }

    const char* getDescription() const override {
        return m_description.c_str();
    }

private:
    std::vector<EU::TSharedPointer<Command>> m_commands;
    std::string m_description;
};
```

### Uso de MacroCommand:

```cpp
CommandInvoker& invoker = baseApp.getCommandInvoker();

auto macro = EU::MakeShared<MacroCommand>("Move and Rename");
macro->addCommand(EU::MakeShared<TransformCommand>(...));
macro->addCommand(EU::MakeShared<RenameActorCommand>(...));

invoker.executeCommand(macro.template dynamic_pointer_cast<Command>());
```

## Mejores Prácticas

### 1. Capturar Estado en Constructor
```cpp
// ? BIEN: Capturar en constructor
MyCommand(MyObject* obj) : m_object(obj), m_oldValue(obj->getValue()) {}

// ? MAL: Capturar en execute()
void execute() { m_oldValue = m_object->getValue(); }  // Muy tarde!
```

### 2. Verificar Punteros Nulos
```cpp
void execute() override {
    if (m_object.isNull()) return;  // Proteger contra nulos
    // ... hacer algo
}
```

### 3. Comparar Estados
```cpp
// ? BIEN: Verificar si hay cambios reales
if (m_oldValue != m_newValue) {
    invoker.executeCommand(command);
}
```

### 4. Usar Descripción Significativa
```cpp
// ? BIEN
return "Move Actor Forward";

// ? MAL
return "Execute";
```

### 5. Manejar Dependencias
```cpp
// Si tu comando depende de otro, guarda ambos
class DependentCommand : public Command {
private:
    EU::TSharedPointer<Command> m_prerequisite;
    // ...
};
```

## Testing

Para probar tus comandos:

```cpp
void testMyCommand() {
    auto obj = EU::MakeShared<MyClass>();

    // Test 1: Execute
    auto cmd = EU::MakeShared<MyCustomCommand>(obj, 42);
    cmd->execute();
    assert(obj->getValue() == 42);

    // Test 2: Undo
    cmd->undo();
    assert(obj->getValue() == 0);  // valor anterior

    // Test 3: Redo
    cmd->redo();
    assert(obj->getValue() == 42);
}
```

## Integración con GUI

Para mostrar los comandos disponibles en la interfaz:

```cpp
// En GUI.cpp
void GUI::drawUndoRedoPanel() {
    ImGui::Begin("Undo/Redo");

    if (m_commandInvoker) {
        if (ImGui::Button("Undo") || (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))) {
            if (m_commandInvoker->canUndo()) {
                m_commandInvoker->undo();
            }
        }
        ImGui::SameLine();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Undo: %s", m_commandInvoker->getUndoDescription());
        }

        if (ImGui::Button("Redo") || (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))) {
            if (m_commandInvoker->canRedo()) {
                m_commandInvoker->redo();
            }
        }
        ImGui::SameLine();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Redo: %s", m_commandInvoker->getRedoDescription());
        }
    }

    ImGui::End();
}
```

## Troubleshooting

### "dynamic_pointer_cast failed"
- Asegúrate de heredar de `Command`
- Verifica que no haya errores de compilación en tu clase

### Comando no se deshace correctamente
- Verifica que `undo()` restaure correctamente el estado anterior
- Comprueba que estés capturando el estado en el constructor

### Historial lleno
- Aumenta `setMaxHistorySize()` o implementa un mecanismo de limpieza
- Los comandos más antiguos se eliminan automáticamente

---

¡Ahora estás listo para crear tus propios comandos personalizados!
