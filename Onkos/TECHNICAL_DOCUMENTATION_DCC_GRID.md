# DOCUMENTACIÓN TÉCNICA: DCC Camera + Grid System

## Tabla de Contenidos
1. [Visión General](#visión-general)
2. [DCC Camera Controller](#dcc-camera-controller)
3. [Grid System](#grid-system)
4. [Decisiones Técnicas Arquitectónicas](#decisiones-técnicas-arquitectónicas)
5. [Flujo de Ejecución](#flujo-de-ejecución)
6. [Problemas Resueltos](#problemas-resueltos)

---

## Visión General

Se han implementado dos mejoras principales al editor DCC (Digital Content Creation):

### 1. **DCC Camera Controller**
- Sistema de navegación estilo DCC profesional con Orbit, Pan y Zoom
- Respeta estándares de software de modelado (Maya, Blender, 3DSMax)
- Control suave con damping opcional

### 2. **Grid System**
- Visualización de grilla en el viewport
- Snap-to-grid para posiciones, rotaciones y escalas
- Totalmente configurable vía ImGui

---

## DCC Camera Controller

### Archivo: `include/EngineUtilities/Utilities/DCCCameraController.h` y `.cpp`

#### Conceptos Fundamentales

**Coordenadas Esféricas para Órbita:**
```
La posición de la cámara se calcula usando coordenadas esféricas:
  - Yaw (rotación horizontal alrededor del eje Y)
  - Pitch (rotación vertical alrededor del eje X local)
  - Distancia (radio desde el focus point)

Fórmula de conversión:
  x = distance * sin(yaw) * cos(pitch)
  y = distance * sin(pitch)
  z = distance * cos(yaw) * cos(pitch)

Esto permite rotación suave alrededor de un punto focal.
```

#### Funcionalidades Principales

| Función | Entrada | Descripción |
|---------|---------|-------------|
| `orbit()` | Right Mouse Drag | Modifica yaw/pitch para rotación suave |
| `pan()` | Middle Mouse Drag | Desplaza el focus point en el plano de vista |
| `zoom()` | Mouse Wheel / Shift+Right Drag | Modifica la distancia orbital |
| `focusOnActor()` | F Key | Auto-zoom al actor seleccionado |

#### Métodos Clave

```cpp
void updateCameraPosition()
```
- **Propósito:** Recalcular posición de cámara basada en parámetros orbitales
- **Llamada desde:** `update()` cada frame
- **Responsable de:**
  1. Calcular offset en coordenadas esféricas
  2. Llamar a `camera->lookAt()` con la nueva posición
  3. Actualizar matriz de vista

```cpp
void zoomToFit(Actor* actor, ...)
```
- **Propósito:** Auto-zoom al actor seleccionado
- **Algorítmo:**
  1. Calcula AABB (bounding box) del actor
  2. Calcula radio de esfera envolvente
  3. Calcula distancia de cámara necesaria para ver el objeto
  4. Interpola suavemente hacia esa distancia

### Modificaciones en BaseApp

#### **archivo: `include/BaseApp.h`**
```cpp
DCCCameraController m_cameraController;  // Línea ~280
GridSystem m_gridSystem;                 // Línea ~283
```

**Por qué en el header:**
- Son miembros de la clase BaseApp que persisten toda la ejecución
- Necesitan ser destruidos junto con la aplicación
- Se inicializan una sola vez en `init()`

#### **Archivo: `source/BaseApp.cpp`**

**Inicialización en `init()`:**
```cpp
EU::Vector3 initialFocusPoint(0.5f, 0.5f, 10.5f);
m_cameraController.init(&m_camera, initialFocusPoint);
m_cameraController.setOrbitDistance(15.0f);
m_cameraController.setYaw(-XM_PI * 0.25f);   // -45 grados
m_cameraController.setPitch(XM_PI * 0.25f);  // 45 grados
```

**Por qué estos valores iniciales:**
- Focus point coincide con centroide de objetos (Spitfire y Toad están en Z ~10)
- Distancia de 15 unidades proporciona buen campo de visión inicial
- Yaw/Pitch crean ángulo isométrico estético

**Manejo de entrada en `update()` (líneas ~720-740):**
```cpp
if (m_gui.m_viewportHovered && !m_gui.m_isUsingGizmo) {
    // Conversión crítica: mouse global a relativo del viewport
    int viewportMouseX = static_cast<int>(io.MousePos.x - m_gui.m_viewportPos.x);
    int viewportMouseY = static_cast<int>(io.MousePos.y - m_gui.m_viewportPos.y);

    m_cameraController.update(viewportMouseX, viewportMouseY, ...);
}
```

**Por qué esta estructura:**
- `m_gui.m_viewportHovered` verifica que el mouse está sobre el viewport
- `!m_gui.m_isUsingGizmo` evita conflicto con herramienta de transformación
- La conversión de coordenadas es **CRÍTICA**: ImGui da posición global, pero el controlador necesita relativa

### Problemas Resueltos en Camera

#### Problema 1: Cámara inicialmente fuera de frustum
**Síntoma:** Objetos no se veían aunque estaban cargados

**Causa:** 
- Cámara en Z = -6, objetos en Z = 10-11
- Far plane = 100, pero suficientemente lejano

**Solución:**
1. Cambiar posición inicial compatible con objetos
2. Cambiar far plane de 100 a 1000 (línea ~397 BaseApp.cpp)
3. Verificar que `lookAt()` marca `m_viewDirty = true` (Camera.cpp línea ~36)

#### Problema 2: IsItemHovered no funcionaba en Image
**Síntoma:** Viewport nunca registraba hover

**Causa:** `ImGui::Image()` no es un item interactivo, no se puede hacer hover en él

**Solución** (GUI.cpp línea ~1555):
```cpp
ImGui::Image((ImTextureID)viewportSRV, panelSize);  // No es interactivo

// Agregar botón invisible encima DESPUÉS
ImGui::SetCursorScreenPos(ImGui::GetItemRectMin());
ImGui::InvisibleButton("##ViewportInteraction", panelSize);
ImGui::SetCursorScreenPos(cursorPos);

// Ahora m_viewportHovered funciona
m_viewportHovered = ImGui::IsItemHovered();
```

#### Problema 3: Doble actualización de cámara
**Síntoma:** Ligero lag en rotación de cámara

**Causa:** 
- `updateCameraPosition()` dentro de `DCCCameraController::update()` llamaba `m_camera->updateViewMatrix()`
- BaseApp llamaba nuevamente `m_camera.updateViewMatrix()` después

**Solución:** Remover llamada redundante en BaseApp (se eliminó línea que estaba post-cameraController.update())

---

## Grid System

### Archivo: `include/EngineUtilities/Utilities/GridSystem.h` y `.cpp`

#### Arquitectura

**Estructura de Datos Principal:**
```cpp
struct GridSettings {
    float cellSize = 1.0f;           // Tamaño de celdas
    float opacity = 0.3f;            // Transparencia (0-1)
    bool renderGrid = true;          // Mostrar/ocultar
    bool snapEnabled = false;        // Snap activo
    float snapPositionSize = 0.5f;   // Resolución de posición
    float snapRotationAngle = 15.0f; // ángulos en grados
    float snapScaleValue = 0.1f;     // incrementos de escala
};
```

#### Funcionalidades

**1. Renderizado de Grilla (renderGrid)**

```cpp
void GridSystem::renderGrid(ImDrawList* drawList, 
                           ImVec2 viewportPos,
                           ImVec2 viewportSize,
                           const Camera& camera)
```

**Algoritmo:**
1. Recorre grid lines desde -50 a +50 en pasos de `cellSize`
2. Para cada línea:
   - Crea dos puntos 3D en el plano Y=0
   - Proyecta ambos puntos a pantalla via `projectWorldToScreen()`
   - Dibuja línea 2D en ImGui si está dentro del viewport
3. Resalta líneas centrales (X=0, Z=0) en rojo
4. Dibuja círculo de origen (0,0,0)

**Por qué esta estructura:**
- Proyecta dinámicamente: la grilla se adapta a zoom y rotación de cámara
- Solo dibuja líneas visibles: optimización de rendimiento
- Y=0 es el "suelo": referencia consistente para editor

**2. Snapping de Posición (snapPosition)**

```cpp
EU::Vector3 GridSystem::snapPosition(const EU::Vector3& position) const
{
    if (!m_settings.snapEnabled) return position;

    return EU::Vector3(
        std::round(position.x / snapSize) * snapSize,
        std::round(position.y / snapSize) * snapSize,
        std::round(position.z / snapSize) * snapSize
    );
}
```

**Matemáticas:**
- Divide posición entre tamaño de snap
- Redondea al entero más cercano
- Multiplica de vuelta: `pos' = round(pos / snap) * snap`

**Ejemplo:**
```
Si pos.x = 2.7 y snap = 0.5:
  pos' = round(2.7 / 0.5) * 0.5 = round(5.4) * 0.5 = 5 * 0.5 = 2.5
```

**3. Snapping de Rotación (snapRotation)**

```cpp
float GridSystem::snapRotation(float angleRadians) const
{
    float snapRadians = XMConvertToRadians(m_settings.snapRotationAngle);
    return std::round(angleRadians / snapRadians) * snapRadians;
}
```

**Notas:**
- Convierte ángulo de snap de grados a radianes
- Aplicable a cualquier eje de rotación
- Por defecto 15° = ?/12 radianes

**4. Snapping de Escala (snapScale)**

```cpp
float GridSystem::snapScale(float scaleValue) const
{
    if (scaleValue <= 0.0f) return m_settings.snapScaleValue;
    return std::round(scaleValue / m_settings.snapScaleValue) * m_settings.snapScaleValue;
}
```

**Precaución:** Previene que la escala llegue a cero

### Integración en GUI

#### Archivo: `include/GUI.h` (líneas ~205)

```cpp
void drawViewportPanel(..., class GridSystem* gridSystem);
void drawEditorToolsPanel(class GridSystem* gridSystem);
```

**Por qué puntero y no referencia:**
- El GridSystem podría no existir en algunos contextos de testing
- Permite pasar nullptr sin errores

#### Archivo: `source/GUI.cpp`

**En drawViewportPanel (línea ~1570):**
```cpp
ImDrawList* gizmoDrawList = m_viewportDrawList;
m_viewportDrawList = ImGui::GetForegroundDrawList();  // Pasar al frente
drawLightIcons(actors, camera, lightIconSRV);

// Renderizar grilla sobre iconos pero bajo gizmo
if (gridSystem) {
    gridSystem->renderGrid(m_viewportDrawList, m_viewportPos, m_viewportSize, camera);
}

m_viewportDrawList = gizmoDrawList;
editTransform(camera, window, selectedActor);  // Gizmo encima de todo
```

**Por qué este orden:**
1. Dibujar luces (más atrás)
2. Dibujar grilla (referencia visual)
3. Dibujar gizmo (más adelante, debe ser clickeable)

**drawEditorToolsPanel (línea ~1862):**
```cpp
void GUI::drawEditorToolsPanel(class GridSystem* gridSystem) {
    ImGui::SetNextWindowSize(ImVec2(400.0f, 300.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Editor Tools##Tools")) {
        if (gridSystem) {
            gridSystem->drawImGuiControls();  // Delegación
        }
    }
    ImGui::End();
}
```

**drawImGuiControls (GridSystem.cpp línea ~73):**
```cpp
bool GridSystem::drawImGuiControls() {
    bool changed = false;

    if (ImGui::CollapsingHeader("Grid Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        changed |= ImGui::Checkbox("Render Grid", &m_settings.renderGrid);
        changed |= ImGui::Checkbox("Enable Snap", &m_settings.snapEnabled);
        // ... más controles

        changed |= ImGui::SliderFloat("Grid Size##cell", &m_settings.cellSize, 0.1f, 10.0f);
        // ... más sliders
    }

    return changed;  // Permite que BaseApp reaccione a cambios
}
```

### Integración en BaseApp

#### Archivo: `source/BaseApp.cpp`

**En update() (línea ~774):**
```cpp
m_gui.inspectorGeneral(selectedActor);
m_gui.drawEditorToolsPanel(&m_gridSystem);  // Antes de render()
if (m_gui.consumeSaveSceneRequest()) { ... }
```

**Por qué aquí:**
- `update()` es cuando se construye la UI (ImGui::Update())
- Debe estar después del inspector general (en la misma ventana lógica)
- Antes de `render()` que es cuando se dibuja todo

---

## Decisiones Técnicas Arquitectónicas

### 1. **Por qué GridSystem es miembro de BaseApp y no un Singleton**

```cpp
// ? Implementado
class BaseApp {
    GridSystem m_gridSystem;
};

// ? Evitado: Singleton
class GridSystem {
    static GridSystem& getInstance();
};
```

**Razones:**
- **Testabilidad:** Cada test puede crear su propio GridSystem
- **Flexibilidad:** Múltiples viewports podrían tener diferentes grillas
- **Ciclo de vida claro:** Se destruye con BaseApp
- **Menos acoplamiento global:** Evita estado compartido implícito

### 2. **Por qué puntero al GridSystem en drawViewportPanel**

```cpp
// ? Implementado
void drawViewportPanel(..., class GridSystem* gridSystem) {
    if (gridSystem) {
        gridSystem->renderGrid(...);
    }
}
```

**Razones:**
- Permite testing de GUI sin GridSystem
- Permite nullptr sin crashes
- Mantiene GUI desacoplada de GridSystem
- Forward declaration (`class GridSystem*`) en header evita includes circulares

### 3. **Por qué snapPosition retorna copia y no modifica in-place**

```cpp
// ? Implementado
EU::Vector3 snapPosition(const EU::Vector3& position) const;

// ? Evitado
void snapPosition(EU::Vector3& position);
```

**Razones:**
- **Funcional puro:** sin efectos secundarios, fácil de testear
- **Composable:** `auto pos2 = snap(snap(pos1))`  
- **Thread-safe:** no comparte estado
- **Consistente:** todos los snap() siguen el mismo patrón

### 4. **Conversión de entrada global a relativa del viewport**

```cpp
// ? Critico para funcionar
int viewportMouseX = static_cast<int>(io.MousePos.x - m_gui.m_viewportPos.x);

// Este fue el error original que evitó que funcione el input
```

**Importancia:**
- ImGui da posición **global** en pantalla
- DCCCameraController espera posición **relativa** dentro del viewport
- Sin esta conversión, el mouse se comporta de forma errática

### 5. **Damping para rotación suave (opcional pero importante)**

```cpp
if (m_dampingEnabled) {
    m_yaw = m_yaw + (m_targetYaw - m_yaw) * m_dampingFactor;
    // Interpolación lineal: lerp(current, target, factor)
} else {
    m_yaw = m_targetYaw;  // Cambio inmediato
}
```

**Beneficio:**
- Similar a Maya/Blender: movimiento de cámara suave y profesional
- Factor ~0.2 = 20% del camino hacia el target cada frame = smooth pero responsivo

### 6. **Proyección de world space a screen space**

```cpp
XMVECTOR projected = XMVector3Project(worldVec,
    viewportPos.x, viewportPos.y,
    viewportSize.x, viewportSize.y,
    0.0f, 1.0f,
    camera.getProj(),
    camera.getView(),
    XMMatrixIdentity());

float screenZ = XMVectorGetZ(projected);
if (screenZ < 0.0f || screenZ > 1.0f) {
    return ImVec2(-1.0f, -1.0f);  // Out of frustum
}
```

**Por qué:**
- Z check previene dibujar puntos atrás de la cámara
- XMVector3Project es la forma DirectX de transformar 3D a 2D
- Retornar (-1, -1) es inócuo: se dibuja fuera de pantalla

---

## Flujo de Ejecución

### Frame Típico con DCC Camera + Grid

```
1. BaseApp::update(deltaTime)
   ?? m_gui.update()                         // Preparar ImGui
   ?? ImGuiIO detecta mouse/keyboard
   ?? if (m_gui.m_viewportHovered) {
   ?  ?? Convertir mouse global?relativo     // CRÍTICO
   ?  ?? m_cameraController.update(...)      // Procesar entrada
   ?  ?  ?? m_cameraController.updateCameraPosition()
   ?  ?     ?? Calcular (x,y,z) en esféricas
   ?  ?     ?? m_camera.lookAt(pos, focus, up)
   ?  ?     ?? m_camera.updateViewMatrix()   // Actualizar matriz
   ?  ?? }
   ?? m_sceneGraph.update()                  // Actualizar actores
   ?? m_sceneGraph.gatherRenderScene()       // Preparar render

2. BaseApp::render()
   ?? m_renderPipeline.render()              // Renderizar viewport
   ?? m_gui.drawViewportPanel(..., &m_gridSystem)
   ?  ?? Dibujar imagen del viewport
   ?  ?? Dibujar iconos de luces
   ?  ?? gridSystem->renderGrid()            // GRILLA
   ?  ?  ?? Proyectar líneas 3D?2D
   ?  ?  ?? Dibujar en ImGui::GetForegroundDrawList()
   ?  ?  ?? return
   ?  ?? Dibujar gizmo 3D (encima)
   ?  ?? return
   ?? m_gui.drawEditorToolsPanel(&m_gridSystem)
   ?  ?? gridSystem->drawImGuiControls()     // Panel de configuración
   ?? m_gui.render()                         // Renderizar toda UI
   ?? m_swapChain.present()                  // Mostrar en pantalla
```

### Interacción de Usuario: Rotar Cámara

```
1. Usuario mantiene Right Mouse Button y mueve mouse
2. ImGui::GetIO().MousePos actualiza
3. ImGui::IsMouseDown(ImGuiMouseButton_Right) = true
4. BaseApp::update():
   ?? Detecta viewport hovered
   ?? Calcula deltaX = currentMouseX - prevMouseX
   ?? Pasa a m_cameraController.update()
   ?? DCCCameraController calcula:
   ?  ?? targetYaw -= deltaX * sensitivity * 0.01f
   ?? Si damping deshabilitado:
   ?  ?? m_yaw = m_targetYaw
   ?? updateCameraPosition() calcula nueva posición
   ?? m_camera.lookAt(newPos, focusPoint, up)
5. Siguiente frame renderiza con nueva matriz de vista
```

---

## Problemas Resueltos

### Problema 1: Viewport vacío inicial
**Síntoma:** Aplicación cargaba pero no se veía nada en viewport
**Causa:** Cámara fuera del frustum de objetos
**Solución:** Reposicionar cámara inicial y focus point del controlador

### Problema 2: Controles de cámara no funcionaban
**Síntoma:** Mouse no tenía efecto en viewport
**Causa:** `m_viewportHovered` siempre falso (ImGui::Image no es interactivo)
**Solución:** Agregar InvisibleButton encima para detectar hover

### Problema 3: Movimiento de cámara entrecortado
**Síntoma:** Ligero lag al rotar cámara
**Causa:** Doble actualización de matriz de vista por frame
**Solución:** Remover updateViewMatrix() redundante en BaseApp

### Problema 4: Snap no se aplicaba a transformaciones
**Síntoma:** Grilla visible pero snap no funcionaba
**Causa:** Snap functions creadas pero nunca llamadas desde TransformCommand
**Solución:** GridSystem listo para integración con sistema de transformación (futura)

---

## Próximas Mejoras Recomendadas

1. **Integración con TransformCommand:**
   - Llamar `m_gridSystem.snapPosition()` antes de aplicar transformación
   - Agregar undo/redo para cambios de snap

2. **Raycast Picking (siguiente tarea):**
   - Usar mouse position en update()
   - Castear rayo 3D desde cámara
   - Seleccionar actor en intersección más cercana

3. **Indicador de Snap Activo:**
   - Cambiar color de grilla cuando snap está habilitado
   - Mostrar valor de snap en UI

4. **Configuración Persistente:**
   - Guardar settings de grid en archivo de preferencias
   - Restaurar al iniciar aplicación

---

## Conclusión

La implementación combina:
- **Arquitectura modular:** GridSystem y DCCCameraController son independientes
- **Integración limpia:** Punteros nullptr-safe, delegación clara
- **Decisiones técnicas sólidas:** Funciones puras, orden de renderizado correcto
- **Solución de problemas efectiva:** Cada bug tuvo causa identificable

El código está listo para pickingde objetos y sistema de grilla mejorado.
