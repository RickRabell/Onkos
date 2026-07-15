# Sistema de Undo/Redo - Command Pattern

## Descripción General

Se ha implementado un sistema completo de Undo/Redo usando el patrón Command que permite deshacer y rehacer múltiples operaciones en el editor. El sistema soporta:

- **N pasos**: Historial ilimitado (configurable hasta 100 comandos por defecto)
- **Transformaciones**: Mover (Translate), Rotar (Rotate), Escalar (Scale)
- **Operaciones de Escena**: Crear actores, Eliminar actores
- **Controles**: Ctrl+Z para deshacer, Ctrl+Y para rehacer

## Arquitectura

### Componentes Principales

#### 1. **Command.h**
Clase base abstracta que define la interfaz para todos los comandos.

```cpp
class Command {
    virtual void execute() = 0;    // Ejecuta el comando
    virtual void undo() = 0;       // Deshace el comando
    virtual void redo() = 0;       // Rehace el comando
    virtual const char* getDescription() const = 0;  // Descripción del comando
};
```

#### 2. **CommandInvoker.h**
Gestor principal del historial de comandos. Mantiene dos pilas: una de deshacer y otra de rehacer.

```cpp
class CommandInvoker {
public:
    void executeCommand(EU::TSharedPointer<Command> command);
    bool undo();
    bool redo();
    bool canUndo() const;
    bool canRedo() const;
    const char* getUndoDescription() const;
    const char* getRedoDescription() const;
    void setMaxHistorySize(size_t maxSize);  // Ajustar límite (por defecto 100)
};
```

#### 3. **TransformCommand.h / TransformCommand.cpp**
Comando para operaciones de transformación (posición, rotación, escala).

- Captura el estado inicial antes de la transformación
- Puede deshacer/rehacer movimientos, rotaciones y escalados
- Tipo: "Move", "Rotate" o "Scale"

#### 4. **DeleteCommand.h / DeleteCommand.cpp**
Comando para eliminar actores de la escena.

- Guarda la referencia del actor eliminado
- Puede restaurar el actor con undo
- Vuelve a eliminar con redo

#### 5. **CreateCommand.h / CreateCommand.cpp**
Comando para crear nuevos actores en la escena.

- Crea un nuevo actor con nombre y posición
- Puede eliminar el actor con undo
- Vuelve a crear con redo

## Integración en el Sistema

### BaseApp.h
- Posee un miembro `m_commandInvoker` de tipo `CommandInvoker`
- Método público: `getCommandInvoker()` para acceder al invocador de comandos

### GUI.h / GUI.cpp
- Mantiene una referencia a `CommandInvoker` mediante `setCommandInvoker()`
- Detecta los atajos de teclado:
  - **Ctrl+Z**: Llama a `m_commandInvoker->undo()`
  - **Ctrl+Y**: Llama a `m_commandInvoker->redo()`
- Integración con ImGuizmo: Crea comandos de transformación al soltar el gizmo

### Flujo de Transformación con Gizmo

1. Usuario comienza a usar el gizmo (mouse presionado)
   - Se guarda el estado inicial del transform

2. Usuario manipula el objeto con ImGuizmo
   - El transform se actualiza en tiempo real (sin comandos)

3. Usuario suelta el mouse (gizmo usado)
   - Se compara el estado final con el inicial
   - Si hay diferencia, se crea un `TransformCommand`
   - El comando se ejecuta y se añade al historial

## Uso Práctico

### Para Desarrolladores

#### Crear un Nuevo Tipo de Comando

```cpp
#include "Editor/Command.h"

class MyCustomCommand : public Command {
private:
    // Estado anterior y nuevo

public:
    void execute() override {
        // Aplicar cambios
    }

    void undo() override {
        // Restaurar estado anterior
    }

    void redo() override {
        // Reaplica cambios
    }

    const char* getDescription() const override {
        return "Mi Comando Personalizado";
    }
};
```

#### Ejecutar un Comando

```cpp
// Obtener el CommandInvoker desde BaseApp
CommandInvoker& invoker = baseApp.getCommandInvoker();

// Crear y ejecutar un comando
auto command = EU::MakeShared<MyCustomCommand>(...);
invoker.executeCommand(command.template dynamic_pointer_cast<Command>());
```

### Para Usuarios del Editor

| Atajo | Acción |
|-------|--------|
| **Ctrl+Z** | Deshacer última acción |
| **Ctrl+Y** | Rehacer última acción deshecha |
| Arrastra ImGuizmo | Registra transformación automáticamente |

## Características Implementadas

? **Patrón Command**: Abstracción de operaciones editables
? **Pila Dual**: Historial de deshacer y rehacer independientes
? **N Pasos**: Soporte para múltiples operaciones consecutivas
? **Auto-Clear Redo**: Al ejecutar un nuevo comando, se limpia el historial de redo
? **Límite de Historial**: Configurable (por defecto 100 comandos)
? **Integración ImGuizmo**: Transformaciones capturadas automáticamente
? **Detección de Cambios**: Solo crea comandos si hubo cambios reales
? **Descripción de Comandos**: Interfaz UI-friendly para mostrar qué se deshace/rehace

## Ejemplo Completo

```cpp
// En BaseApp.cpp
void BaseApp::update(float deltaTime) {
    // ...

    // El sistema de undo/redo funciona automáticamente:
    // 1. ImGuizmo detecta transformaciones
    // 2. GUI.editTransform() crea comandos automáticamente
    // 3. Ctrl+Z/Ctrl+Y manipulan el historial
}

// En GUI.cpp (editTransform)
// Cuando el usuario suelta el gizmo después de mover un objeto:
auto command = EU::MakeShared<TransformCommand>(
    actor,
    newPosition,
    newRotation,
    newScale,
    "Move"  // o "Rotate" o "Scale"
);
m_commandInvoker->executeCommand(command.template dynamic_pointer_cast<Command>());

// El usuario presiona Ctrl+Z
// El objeto vuelve a su posición anterior
```

## Configuración Avanzada

### Cambiar el Límite del Historial

```cpp
BaseApp app;
CommandInvoker& invoker = app.getCommandInvoker();
invoker.setMaxHistorySize(200);  // Permite hasta 200 comandos en historial
```

### Limpiar Historial

```cpp
invoker.clear();  // Elimina todos los comandos de deshacer y rehacer
```

### Verificar Disponibilidad

```cpp
if (invoker.canUndo()) {
    invoker.undo();
}

if (invoker.canRedo()) {
    invoker.redo();
}
```

## Notas de Implementación

- Los comandos se guardan como `TSharedPointer<Command>` para gestión automática de memoria
- La conversión de tipos usa `dynamic_pointer_cast<>` para mantener compatibilidad
- Los cambios de transform se comparan con epsilon (0.0001f) para evitar comandos por cambios insignificantes
- El estado anterior se captura en el constructor del comando
- El CommandInvoker se inicializa en `BaseApp::run()` antes de cargar la GUI
