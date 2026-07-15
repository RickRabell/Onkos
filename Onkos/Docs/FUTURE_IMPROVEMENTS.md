# Futuras Mejoras - Roadmap

## Mejoras Propuestas para el Motor Onkos

Este documento describe las futuras mejoras que pueden integrarse en el sistema de navegación y edición del viewport, considerando el trabajo ya realizado con el DCCCameraController.

---

## 1. Picking en Viewport (Raycast desde Mouse)

### Descripción
Implementar raycasting desde la posición del mouse hacia el mundo 3D para detectar qué objeto está bajo el cursor.

### Funcionalidad
- **Clic en objeto**: Selecciona el actor haciendo clic en el viewport
- **Visualización en Inspector**: El objeto seleccionado aparece automáticamente en el inspector ImGui
- **Gizmo Respectivo**: Se aplica el gizmo de transformación al objeto seleccionado

### Archivos a Modificar
- `include/EngineUtilities/Utilities/RaycastManager.h` (nuevo archivo)
- `source/EngineUtilities/Utilities/RaycastManager.cpp` (nuevo archivo)
- `source/GUI.cpp` - Integrar raycasting en `drawViewportPanel()`
- `source/BaseApp.cpp` - Manejar selección de objetos por raycast

### Algoritmo General
```cpp
1. Obtener posición del mouse en pantalla
2. Convertir coordenadas de pantalla a espacio NDC
3. Crear rayo desde cámara a través del pixel del mouse
4. Iterar todos los actores en la escena
5. Verificar intersección rayo-AABB o rayo-triángulo
6. Retornar el actor más cercano (menor distancia al origen del rayo)
7. Seleccionar automáticamente ese actor
```

### Pseudocódigo
```cpp
class RaycastManager {
public:
    struct RaycastResult {
        Actor* hitActor;
        float distance;
        EU::Vector3 hitPoint;
        bool isHit;
    };

    RaycastResult raycast(const EU::Vector3& rayOrigin, 
                         const EU::Vector3& rayDirection,
                         const std::vector<EU::TSharedPointer<Actor>>& actors);

    RaycastResult raycastFromMouse(int mouseX, int mouseY,
                                  Camera& camera,
                                  ImVec2 viewportPos,
                                  ImVec2 viewportSize,
                                  const std::vector<EU::TSharedPointer<Actor>>& actors);
};
```

### Integración en BaseApp
```cpp
// En update()
if (m_gui.m_viewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    if (!m_gui.m_isUsingGizmo) {  // No seleccionar mientras se usa gizmo
        ImGuiIO& io = ImGui::GetIO();
        RaycastManager::RaycastResult result = m_raycastManager.raycastFromMouse(
            static_cast<int>(io.MousePos.x),
            static_cast<int>(io.MousePos.y),
            m_camera,
            m_gui.m_viewportPos,
            m_gui.m_viewportSize,
            m_actors
        );

        if (result.isHit) {
            // Encontrar índice del actor
            for (int i = 0; i < m_actors.size(); ++i) {
                if (m_actors[i].get() == result.hitActor) {
                    m_gui.selectedActorIndex = i;
                    break;
                }
            }
        } else {
            m_gui.selectedActorIndex = -1;  // Deseleccionar
        }
    }
}
```

### Consideraciones
- Usar AABB (Axis-Aligned Bounding Box) para intersecciones rápidas
- Opcionalmente, usar raycasting de triángulo para precisión en meshes complejos
- Implementar frustum culling para optimizar búsqueda
- Permitir selección múltiple con Ctrl+Clic (futura mejora)

---

## 2. Grid con Snap (Opcionales)

### Descripción
Visualizar una grilla en el viewport y permitir snap de transformaciones a ella.

### Funcionalidad
- **Visualización**: Mostrar grilla de referencia en el viewport (modificable en tamaño y opacidad)
- **Snap Position**: Encajar la posición del objeto a puntos de la grilla
- **Snap Rotation**: Encajar rotación a ángulos específicos (15°, 45°, 90°, etc.)
- **Snap Scale**: Encajar escala a valores discretos (0.5, 1.0, 1.5, 2.0, etc.)

### Archivos a Crear/Modificar
- `include/EngineUtilities/Utilities/GridSystem.h` (nuevo archivo)
- `source/EngineUtilities/Utilities/GridSystem.cpp` (nuevo archivo)
- `source/GUI.cpp` - Renderizar grilla en `drawViewportPanel()`
- `source/BaseApp.cpp` - Aplicar snap durante transformación

### Estructura de GridSystem
```cpp
class GridSystem {
public:
    struct GridSettings {
        float cellSize = 1.0f;
        float opacity = 0.5f;
        bool renderGrid = true;
        bool snapEnabled = false;
        float snapSize = 1.0f;
        float rotationSnapAngle = 15.0f;  // grados
        float scaleSnapValue = 0.25f;
    };

    GridSettings m_settings;

    // Funciones de rendering
    void renderGrid(ImDrawList* drawList, 
                   ImVec2 viewportPos, 
                   ImVec2 viewportSize,
                   Camera& camera);

    // Funciones de snap
    EU::Vector3 snapPosition(const EU::Vector3& position);
    float snapRotation(float angle);
    float snapScale(float scale);

    // Configuración
    void setGridSize(float size);
    void setSnapEnabled(bool enabled);
    void setOpacity(float opacity);
};
```

### Renderizado de Grilla
```cpp
void GridSystem::renderGrid(ImDrawList* drawList, 
                           ImVec2 viewportPos, 
                           ImVec2 viewportSize,
                           Camera& camera) {
    if (!m_settings.renderGrid) return;

    // Calcular posiciones de grilla en espacio de pantalla
    // Proyectar puntos de grilla desde mundo a pantalla
    // Dibujar líneas de grilla usando drawList->AddLine()

    ImU32 gridColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, m_settings.opacity));

    // Dibujar líneas verticales y horizontales
    for (float x = -10.0f; x <= 10.0f; x += m_settings.cellSize) {
        // Proyecto x a pantalla
        // Dibujo línea vertical
    }
    for (float z = -10.0f; z <= 10.0f; z += m_settings.cellSize) {
        // Proyecto z a pantalla
        // Dibujo línea horizontal
    }
}
```

### Integración en Transformaciones
```cpp
// En el inspector cuando se editan propiedades
if (BeginInspectorPropertyTable("##TransformProperties")) {
    DrawPropertyLabel("Position");
    if (ImGui::SliderFloat3("##Position", &newPos.x)) {
        if (m_gridSystem.m_settings.snapEnabled) {
            newPos = m_gridSystem.snapPosition(newPos);
        }
        transform->setPosition(newPos);
    }
    // Similar para rotación y escala
}
```

### Interfaz de Usuario
Agregar panel en la GUI para controlar la grilla:
```
Grid Settings:
  ? Show Grid
  ? Enable Snap
  Grid Size: [slider] 0.5 to 5.0
  Snap Size: [slider] 0.1 to 2.0
  Grid Opacity: [slider] 0.1 to 1.0
```

---

## 3. Mejora: Focus on Selection (YA IMPLEMENTADO ?)

La funcionalidad de "Focus on Selection" ya está completamente implementada:
- Presionar **F** enfoca el objeto seleccionado
- Se calcula automáticamente la distancia óptima
- Se centra el viewport en el bounding box del objeto

---

## 4. Picking Multi-objeto (Futura Mejora)

### Descripción
Extender el raycasting para permitir selección de múltiples objetos.

### Funcionalidad
- **Ctrl+Clic**: Agregar objeto a la selección
- **Shift+Clic**: Remover objeto de la selección
- **Selección por Área**: Dibujar rectángulo en viewport para seleccionar múltiples objetos
- **Outliner Selección**: Ctrl+Clic en outliner para multi-selección

### Consideraciones
- Mantener lista de objetos seleccionados
- Mostrar gizmo conjunto o individual según preferencia
- Agregar indicador visual en viewport (outline/highlight)

---

## 5. Mejora: Viewport Camera Presets

### Descripción
Guardar y restaurar posiciones de cámara preestablecidas.

### Funcionalidad
- **Teclas Numéricas**: 0-9 para guardar/cargar vistas
- **Vistas Estándar**: Front, Back, Left, Right, Top, Bottom, Isometric
- **Transición Suave**: Animación entre vistas preestablecidas

### Implementación
```cpp
class CameraPresets {
public:
    struct CameraPreset {
        EU::Vector3 position;
        float yaw, pitch;
        float distance;
    };

    std::map<int, CameraPreset> m_presets;

    void savePreset(int slot, const DCCCameraController& controller);
    void loadPreset(int slot, DCCCameraController& controller);
    void setStandardViews(DCCCameraController& controller);
};
```

---

## 6. Mejora: Viewport Gizmo Enhancements

### Descripción
Mejorar la interacción con el gizmo de ImGuizmo.

### Funcionalidad
- **Constrain a Eje**: Teclas X, Y, Z para limitar movimiento a eje específico
- **Duplicar+Mover**: Shift+Arrastrar gizmo duplica el objeto
- **Snap a Eje**: Shift+Arrastrar aplica snap de 1 unidad
- **Incrementos Precisos**: Alt+Arrastrar para ajustes finos (0.1 unidades)

---

## 7. Mejora: Viewport Shortcuts

### Tabla de Atajos Propuestos

| Atajo | Acción |
|-------|--------|
| **F** | Focus on Selection |
| **G** | Grab/Move (Blender-style) |
| **R** | Rotate |
| **S** | Scale |
| **X, Y, Z** | Constrainar a eje |
| **Shift+D** | Duplicate |
| **Ctrl+D** | Duplicate and Move |
| **Del** | Delete Selected |
| **H** | Hide Selected |
| **Shift+H** | Show All |
| **/** | Frame All Objects |
| **Ctrl+0** | Save View 0 |
| **0** | Load View 0 |
| **Numpad 7** | Top View |
| **Numpad 1** | Front View |
| **Numpad 3** | Side View |

---

## 8. Plan de Implementación Sugerido

### Fase 1: Picking (Próxima - Recomendado)
- **Tiempo Estimado**: 2-3 horas
- **Complejidad**: Media
- **Impacto**: Alto (mejora significativa de UX)
- **Dependencias**: Ninguna (usa infraestructura existente)

### Fase 2: Grid System
- **Tiempo Estimado**: 3-4 horas
- **Complejidad**: Media
- **Impacto**: Medio (útil para diseño de nivel)
- **Dependencias**: GridSystem (nueva clase)

### Fase 3: Camera Presets
- **Tiempo Estimado**: 1.5 horas
- **Complejidad**: Baja
- **Impacto**: Bajo-Medio (mejora de productividad)
- **Dependencias**: DCCCameraController (ya existe)

### Fase 4: Multi-selección y Polish
- **Tiempo Estimado**: 4-5 horas
- **Complejidad**: Alta
- **Impacto**: Alto (mejora colaboración)
- **Dependencias**: Picking (Fase 1)

---

## Referencias y Estándares

### Comportamiento Esperado (Basado en Industria)
- **Maya**: Orbit con botón central, Pan con Alt+botón central, Zoom con rueda
- **Blender**: Orbit con botón central, Pan con Shift+botón central, Zoom con rueda
- **Unreal Engine**: Orbit con botón derecho, Pan con botón central, Zoom con rueda
- **Onkos**: Implementa estilo Unreal (Orbit derecha, Pan centro, Zoom rueda)

### Recursos Externos
- [Dear ImGui Documentation](https://github.com/ocornut/imgui)
- [ImGuizmo Documentation](https://github.com/CedricGuillemet/ImGuizmo)
- [Raycasting Techniques](https://www.khronos.org/opengl/wiki/Raycasting)

---

## Conclusión

El sistema de navegación DCC ya está implementado y funcional. Las mejoras futuras propuestas se pueden agregar incrementalmente sin afectar el código existente del `DCCCameraController`. La recomendación es implementar **Picking** primero, ya que proporciona el mayor retorno en términos de experiencia de usuario y mejora la productividad del editor.
