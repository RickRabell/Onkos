# DCC Camera Controller - Guía de Uso

## Resumen

Se ha implementado un controlador de cámara estilo **DCC (Digital Content Creation)** que permite navegación intuitiva del viewport 3D, similar a la que encuentras en herramientas profesionales como Maya, Blender o 3ds Max.

## Características Implementadas

### 1. **Orbit** (Rotación)
- **Control**: Botón derecho del ratón + arrastrar
- **Comportamiento**: La cámara rota alrededor de un punto focal, permitiendo ver el objeto desde todos los ángulos
- **Sensibilidad**: Ajustable mediante `m_cameraController.setSensitivity(valor)`

### 2. **Pan** (Desplazamiento)
- **Control**: Botón central del ratón (rueda) + arrastrar
- **Comportamiento**: Mueve la cámara dentro del plano de vista, reposicionando el punto focal
- **Distancia de Desplazamiento**: Basada en la distancia orbital actual para mantener proporciones consistentes

### 3. **Zoom** (Acercamiento/Alejamiento)
- **Control**: Rueda del ratón O Shift + botón derecho + arrastrar
- **Comportamiento**: Zoom en/out suave con límites configurables
- **Sensibilidad de Zoom**: Ajustable mediante `m_cameraController.setZoomSensitivity(valor)`
- **Límites**: `setMinDistance()` y `setMaxDistance()` previenen acercarse demasiado o alejarse excesivamente

### 4. **Focus on Selection** (Enfocar Selección)
- **Control**: Tecla **F**
- **Comportamiento**: Automáticamente encuadra el actor seleccionado en el viewport con distancia óptima
- **Requisito**: Un actor debe estar seleccionado en el inspector/outliner

## Archivo de Configuración

### Encabezado
```cpp
#include "EngineUtilities\Utilities\DCCCameraController.h"
```

### Inicialización en BaseApp
Ya está implementada en el método `init()`:

```cpp
EU::Vector3 initialFocusPoint(0.0f, 1.0f, 10.0f);
m_cameraController.init(&m_camera, initialFocusPoint);
m_cameraController.setSensitivity(1.0f);
m_cameraController.setZoomSensitivity(0.15f);
m_cameraController.setConstrainPitch(true);
```

### Actualización en Cada Frame
Ya está implementada en el método `update()`:

```cpp
// Handle DCC-style camera navigation
ImGuiIO& io = ImGui::GetIO();
if (m_gui.m_viewportHovered && !m_gui.m_isUsingGizmo) {
    int currentMouseX = static_cast<int>(io.MousePos.x);
    int currentMouseY = static_cast<int>(io.MousePos.y);
    bool rightMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);
    bool middleMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
    int scrollDelta = static_cast<int>(io.MouseWheel);
    bool leftShiftHeld = io.KeyShift;

    // Handle "Focus on Selection" with F key
    if (ImGui::IsKeyPressed(ImGuiKey_F) && m_gui.selectedActorIndex >= 0) {
        EU::TSharedPointer<Actor> selectedActor = m_actors[m_gui.selectedActorIndex];
        m_cameraController.focusOnActor(selectedActor.get(), 
                                       m_editorViewportPass.getWidth(), 
                                       m_editorViewportPass.getHeight());
    }

    m_cameraController.update(currentMouseX, currentMouseY, rightMouseDown, 
                             middleMouseDown, scrollDelta, leftShiftHeld);
}
```

## Archivos Modificados

### Nuevos Archivos Creados
1. `include/EngineUtilities/Utilities/DCCCameraController.h` - Declaración de la clase
2. `source/EngineUtilities/Utilities/DCCCameraController.cpp` - Implementación de la clase

### Archivos Modificados
1. `include/BaseApp.h`:
   - Agregado include: `#include "EngineUtilities\Utilities\DCCCameraController.h"`
   - Agregado miembro: `DCCCameraController m_cameraController;`

2. `source/BaseApp.cpp`:
   - Inicialización del controlador en `init()`
   - Manejo de entrada en `update()`
   - Soporte para tecla F para "Focus on Selection"

## Métodos Públicos del DCCCameraController

### Inicialización
```cpp
void init(Camera* camera, const EU::Vector3& initialFocusPoint);
```

### Actualización
```cpp
void update(int currentMouseX, int currentMouseY, bool rightMouseDown,
            bool middleMouseDown, int scrollDelta, bool leftShiftHeld);
```

### Zoom-to-Fit
```cpp
void zoomToFit(const EU::Vector3& boundingBoxMin,
               const EU::Vector3& boundingBoxMax,
               unsigned int viewportWidth,
               unsigned int viewportHeight);
```

### Focus on Actor
```cpp
void focusOnActor(Actor* actor, unsigned int viewportWidth, unsigned int viewportHeight);
```

### Configuración de Parámetros
```cpp
void setSensitivity(float sensitivity);                    // 0.1f a 5.0f
void setZoomSensitivity(float zoomSensitivity);           // 0.05f a 0.5f
void setMinDistance(float minDistance);                    // Mínimo de zoom
void setMaxDistance(float maxDistance);                    // Máximo de zoom
void setConstrainPitch(bool constrainPitch);              // Prevenir gimbal lock
void setDampingEnabled(bool enable);                      // Suavizado de movimiento
void setDampingFactor(float damping);                     // 0.0f a 1.0f (suavidad)
```

### Getters
```cpp
float getOrbitDistance() const;
EU::Vector3 getFocusPoint() const;
float getYaw() const;
float getPitch() const;
float getSensitivity() const;
float getZoomSensitivity() const;
float getMinDistance() const;
float getMaxDistance() const;
```

## Condiciones Operacionales

La navegación de cámara **solo está activa cuando**:
1. El ratón está dentro del viewport (`m_gui.m_viewportHovered` == true)
2. El usuario NO está usando el Gizmo (`m_gui.m_isUsingGizmo` == false)

Esto previene conflictos con la manipulación de objetos mediante el gizmo de transformación.

## Futuras Mejoras (Como se Mencionó)

### 1. **Picking en Viewport (Raycast)**
- Implementar raycasting desde la posición del mouse hacia el mundo 3D
- Detectar qué objeto está bajo el cursor al hacer clic
- Seleccionar el objeto automáticamente y mostrarlo en el inspector

### 2. **Focus on Selection (Tecla F)**
? **YA IMPLEMENTADO** - Presiona F para enfocar el actor seleccionado

### 3. **Grid con Snap**
- Implementar una grilla de fondo en el viewport
- Agregar opción de "snap" para encajar transformaciones a la grilla

## Notas de Implementación

- **Gimbal Lock Prevention**: El controlador limita el pitch entre -90° y +90° para evitar gimbal lock
- **Damping (Suavizado)**: Puede habilitarse para movimientos más orgánicos (deshabilitado por defecto)
- **Collision Detection**: Actualmente no hay colisiones; la cámara puede atravesar objetos
- **Bounding Box**: La función `focusOnActor()` estima el bounding box basándose en un radio unitario (mejora futura: integrar con datos reales del mesh)

## Ejemplo de Uso Avanzado

```cpp
// En BaseApp::init()
m_cameraController.init(&m_camera, EU::Vector3(0.0f, 1.0f, 10.0f));
m_cameraController.setSensitivity(1.5f);
m_cameraController.setZoomSensitivity(0.2f);
m_cameraController.setMinDistance(0.5f);
m_cameraController.setMaxDistance(500.0f);
m_cameraController.setDampingEnabled(true);
m_cameraController.setDampingFactor(0.15f);

// En BaseApp::update()
if (ImGui::IsKeyPressed(ImGuiKey_Home)) {
    // Reset camera to default view
    m_cameraController.setYaw(0.0f);
    m_cameraController.setPitch(XM_PIDIV4);
    m_cameraController.setOrbitDistance(10.0f);
    m_cameraController.setFocusPoint(EU::Vector3(0.0f, 1.0f, 10.0f));
}
```

---

¡El sistema está listo para usar! Las futuras mejoras de picking y grid snap pueden agregarse sin afectar el código existente del controlador de cámara.
