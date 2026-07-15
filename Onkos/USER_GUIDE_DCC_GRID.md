# GUÍA DE USO: DCC Camera Controller + Grid System

## Control de Cámara (DCC Style)

### Órbita (Rotación alrededor de objeto)
```
INPUT:  Right Mouse Button + Drag
EFECTO: Rota la cámara horizontalmente (Yaw) y verticalmente (Pitch)
        alrededor del punto focal
EJEMPLO: Selecciona un objeto, sostén clic derecho y mueve el mouse
         para ver el objeto desde diferentes ángulos
```

### Panorámica (Desplazamiento)
```
INPUT:  Middle Mouse Button (rueda) + Drag
EFECTO: Desplaza el punto focal manteniendo la distancia
        similar a "arrastrar" la vista
EJEMPLO: Clic central y arrastra para mover el punto de interés
```

### Zoom (Alejar/Acercar)
```
INPUT:  Mouse Wheel (scroll arriba/abajo)
        O Shift + Right Mouse + Drag (arriba para alejar, abajo para acercar)
EFECTO: Cambia la distancia de la cámara al punto focal
LÍMITES: 0.1 a 1000 unidades
```

### Enfoque en Selección
```
INPUT:  F Key (cuando un actor está seleccionado)
EFECTO: Auto-zoom y reposicionamiento para ver completamente el objeto
        seleccionado
EJEMPLO: Click en "Sci-Fi Toad" en el Outliner ? Presiona F ? Cámara
         se enfoca en la rana
```

---

## Sistema de Grilla

### Visualización de Grilla

#### Activar/Desactivar Grilla
1. Busca el panel "Editor Tools##Tools" en la UI
2. En "Grid Settings", marca/desmarca "Render Grid"

#### Controlar Tamaño de Celdas
- Ajusta el slider "Grid Size##cell" de 0.1 a 10.0 unidades
- Rango recomendado: 0.5 a 2.0 para la mayoría de proyectos

#### Controlar Opacidad
- Usa el slider "Grid Opacity" de 0.1 a 1.0
- Valores bajos (0.2-0.3) para menos distracción
- Valores altos para referencia clara

---

### Sistema de Snap (Alineación a Grilla)

#### Activar Snap Global
1. En "Editor Tools", marca "Enable Snap"
2. Esto activa snap para posiciones, rotaciones y escalas

#### Snap de Posición
```
FUNCIÓN:    Alinea objetos a puntos de grilla
CONTROL:    Slider "Snap Position Size" (0.1 a 5.0)
EJEMPLO:    Si snap = 0.5:
            - pos.x = 2.7 ? se snapea a 2.5
            - pos.x = 2.3 ? se snapea a 2.5
            - pos.x = 2.6 ? se snapea a 2.5 (round to nearest)
NOTA:       Aplica a X, Y, Z independientemente
```

#### Snap de Rotación
```
FUNCIÓN:    Alinea rotaciones a ángulos específicos
CONTROL:    Slider "Snap Rotation (degrees)" (1 a 90)
EJEMPLO:    Si snap = 45°:
            - Rotación 47° ? se snapea a 45°
            - Rotación 22° ? se snapea a 0°
CASOS:
- 15°: Rotaciones muy finas (24 rotaciones posibles)
- 45°: Rotaciones comunes (8 rotaciones en 360°)
- 90°: Alineación ortogonal (4 rotaciones)
```

#### Snap de Escala
```
FUNCIÓN:    Alinea tamaños a valores discretos
CONTROL:    Slider "Snap Scale" (0.05 a 1.0)
EJEMPLO:    Si snap = 0.1:
            - escala 1.25 ? se snapea a 1.2
            - escala 1.29 ? se snapea a 1.3
```

---

## Flujo de Trabajo Recomendado

### Sesión Típica de Edición

```
1. INICIO
   ?? Cámara enfocada en escena (focus point ~centro de objetos)

2. NAVEGACIÓN
   ?? Right Click + Drag: Rotar para inspeccionar objeto
   ?? Middle Click + Drag: Desplazar si necesitas ver parte específica
   ?? Scroll: Zoom in/out según necesites detalles

3. SELECCIÓN DE OBJETO
   ?? Click en objeto en el viewport
   ?? O selecciona desde el Outliner

4. ENFOQUE EN OBJETO
   ?? Objeto seleccionado ? Presiona F
   ?? Cámara se posiciona automáticamente
   ?? Puedes ver completo el objeto

5. TRANSFORMACIÓN CON SNAP
   ?? Abre "Editor Tools" panel
   ?? Habilita "Enable Snap"
   ?? Usa Gizmo (Move/Rotate/Scale)
   ?? Posición/rotación/escala se snapea automáticamente
   ?? Objeto se alinea a grilla

6. AJUSTE DE REFERENCIA
   ?? Si la grilla es muy pequeña: aumenta "Grid Size"
   ?? Si es muy visible: reduce "Grid Opacity"
   ?? Si necesitas snap más fino: reduce "Snap Position Size"
   ?? Repite según necesites
```

---

## Consejos Profesionales

### Navegación Eficiente
```
PROBLEMA: La cámara gira pero quiero panorámica rápida
SOLUCIÓN: Middle Click + Drag es más directo

PROBLEMA: Zoom muy lento/rápido
SOLUCIÓN: Acerca el mouse wheel (scroll arriba = zoom in, abajo = zoom out)

PROBLEMA: Perdí de vista el objeto
SOLUCIÓN: Presiona F (focus en selección) o desselecciona todos (click en espacio)
```

### Trabajo con Snap
```
MEJOR PRÁCTICA: Habilita Snap DESPUÉS de posicionar globalmente
?? Primero: coloca objeto "aproximadamente" sin snap
?? Segundo: habilita snap y hace ajustes finos
?? Razón: evita "saltos" durante colocación inicial

MULTI-OBJETO: Alinear varios objetos
?? Usa snap = 1.0 para alineación coarse
?? Todos los objetos se colocan en posiciones claras
?? Facilita diseño modular y procedural
```

### Configuración de Viewport
```
LIMPIO Y MINIMALISTA:
?? Grid Opacity: 0.2
?? Grid Size: 1.0
?? Snap habilitado por defecto

MÁXIMO DETALLE:
?? Grid Opacity: 0.8
?? Grid Size: 0.5
?? Snap Position: 0.1 (muy fino)
```

---

## Teclas de Acceso Rápido (Roadmap)

| Tecla | Función | Estado |
|-------|---------|--------|
| F | Focus en selección | ? Implementado |
| Right Click | Órbita | ? Implementado |
| Middle Click | Pan | ? Implementado |
| Scroll | Zoom | ? Implementado |
| G | Toggle Grilla (futuro) | ? Pendiente |
| Shift+G | Toggle Snap (futuro) | ? Pendiente |
| [ | Reduce Grid Size (futuro) | ? Pendiente |
| ] | Aumenta Grid Size (futuro) | ? Pendiente |

---

## Troubleshooting

### La grilla no se ve
```
CAUSA 1: "Render Grid" está desmarcado
SOLUCIÓN: Marca la casilla en Editor Tools

CAUSA 2: Opacidad en 0
SOLUCIÓN: Aumenta "Grid Opacity" a 0.3+

CAUSA 3: Cámara debajo del plano Y=0
SOLUCIÓN: Acerca zoom (scroll) o presiona F para enfocar en objeto
```

### Snap no funciona al mover objeto
```
NOTA: Snap está implementado pero NO integrado con TransformCommand
ESTADO: GridSystem.snapPosition() existe pero no se llama automáticamente
PRÓXIMO: Sistema de picking (raycast) integrará snap automáticamente

WORKAROUND: Puedes ver el snap calculándolo manualmente si lo necesitas
```

### Cámara se mueve muy rápido/lento
```
MUY RÁPIDO:
?? Reduce "Sensitivity" en DCCCameraController (ajuste futuro)

MUY LENTO:
?? Aumenta "Sensitivity" en DCCCameraController (ajuste futuro)

TEMPORAL:
?? Usa wheel zoom que es más controlable que drag
```

### Mouse se comporta erráticamente
```
CAUSA: Viewport no detecta hover correctamente
SOLUCIÓN: 
1. Asegúrate que el mouse está DENTRO del viewport (no en edge)
2. Verifica que no hay otro panel superpuesto

DEBUG: Abre Output window (View > Output) y busca "Hovered: YES"
cuando el mouse está sobre viewport
```

---

## Próximas Funcionalidades

### 1. Picking (Raycast) - Selección con Clic
```
DESCRIPCIÓN: Click en objeto en viewport lo selecciona
PRIORIDAD: Alta
STATUS: En desarrollo
```

### 2. Grid Snap Automático
```
DESCRIPCIÓN: Transformaciones se snapean automáticamente si snap activo
INTEGRACIÓN: DCCCameraController + TransformCommand
STATUS: Listo para integrar (GridSystem.snapPosition existe)
```

### 3. Atajos de Teclado
```
DESCRIPCIÓN: G = toggle grid, [ / ] = cambiar tamaño
STATUS: Pendiente de implementación
```

### 4. Referencia de Snap en Gizmo
```
DESCRIPCIÓN: Mostrar líneas de snap mientras se arrastra
STATUS: Futuro mejoramientos
```

---

## Resumen Rápido

| Acción | Control |
|--------|---------|
| Rotar cámara | Right Click + Drag |
| Mover vista | Middle Click + Drag |
| Acercar/Alejar | Mouse Wheel |
| Enfocar objeto | F key |
| Ver grilla | Editor Tools > Render Grid |
| Activar snap | Editor Tools > Enable Snap |
| Cambiar tamaño | Editor Tools > Grid Size |

---

¡Listo para usar el editor DCC profesional! ??
