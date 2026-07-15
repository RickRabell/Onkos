# ?? REFERENCIA RÁPIDA DE ATAJOS

## Undo/Redo en el Editor

### Atajos de Teclado

| Atajo | Acción | Resultado |
|-------|--------|-----------|
| **Ctrl+Z** | Deshacer | Revierte el último cambio |
| **Ctrl+Y** | Rehacer | Reaplica el último cambio deshecho |

### Operaciones Registradas Automáticamente

| Operación | Cómo | Registro |
|-----------|------|----------|
| Mover | Arrastra gizmo (flecha roja/verde/azul) | ? Automático |
| Rotar | Arrastra gizmo (círculos) | ? Automático |
| Escalar | Arrastra gizmo (cuadrados) | ? Automático |
| Crear Actor | Interfaz de creación | ? Automático |
| Eliminar Actor | Eliminar entidad | ? Automático |

---

## Ejemplos de Uso

### Ejemplo 1: Mover Objeto y Deshacer
```
1. Selecciona objeto
2. Arrastra gizmo 5 unidades a la derecha
3. Ctrl+Z
   ? Objeto vuelve a posición original
4. Ctrl+Y
   ? Objeto vuelve 5 unidades a la derecha
```

### Ejemplo 2: Múltiples Operaciones
```
1. Mueve objeto (Cmd 1)
2. Rota objeto (Cmd 2)
3. Escala objeto (Cmd 3)

Presionando Ctrl+Z 3 veces:
- Primer Ctrl+Z ? Escala se revierte
- Segundo Ctrl+Z ? Rotación se revierte
- Tercer Ctrl+Z ? Movimiento se revierte
```

### Ejemplo 3: Experimento con Rehacer
```
1. Move objeto (Cmd 1)
2. Rotate objeto (Cmd 2)
3. Ctrl+Z ? Scale se revierte
4. Ctrl+Z ? Rotate se revierte
5. Ctrl+Y ? Rotate se reaplica
6. Ctrl+Y ? Scale se reaplica
```

---

## Limitaciones y Notas

### Cambios Registrados
? Transformaciones de ImGuizmo (Move, Rotate, Scale)
? Creación de actores
? Eliminación de actores

### Cambios NO Registrados (Aún)
? Cambios de propiedades en inspector (editable después)
? Cambios de materiales
? Cambios de luz
? Cambios de nombres (editable después)

---

## Límites del Sistema

| Aspecto | Valor | Notas |
|---------|-------|-------|
| Máximo historial | 100 comandos | Configurable |
| Cambio mínimo detectable | 0.0001 unidades | Epsilon |
| Tiempo por undo | < 1 ms | Casi instantáneo |
| Memoria por comando | ~1 KB | Aprox. |

---

## Troubleshooting Rápido

### Ctrl+Z no funciona
- ? Verifica que el foco esté en la ventana principal
- ? Asegúrate de haber hecho algún cambio antes
- ? Intenta presionar en la ventana viewport

### Comando no se registra
- ? El cambio debe ser > 0.0001 unidades
- ? Debes soltar el mouse para registrar
- ? No se registran cambios insignificantes

### Historial se llena rápido
- ? Es normal si haces muchas operaciones
- ? El límite por defecto es 100 comandos
- ? Los comandos más antiguos se eliminan automáticamente

---

## Atajos Relacionados (Existentes)

| Atajo | Función | Origen |
|-------|---------|--------|
| **Ctrl+S** | Guardar escena | Existente |
| **W** | Modo Translate | ImGuizmo |
| **E** | Modo Rotate | ImGuizmo |
| **R** | Modo Scale | ImGuizmo |
| **X** | Cambiar Global/Local | ImGuizmo |

---

## Casos de Uso Completos

### Caso 1: Corregir Posición Incorrecta
```
1. Mueves objeto a (10, 0, 0) pero debía ser (5, 0, 0)
2. Ctrl+Z ? Vuelve a (0, 0, 0)
3. Mueves a (5, 0, 0) correctamente
```

### Caso 2: Probar Diferentes Transformaciones
```
1. Prueba Move ? No te gusta
2. Ctrl+Z ? Revierte
3. Prueba Rotate ? Mejor
4. Prueba Scale ? Aún mejor
5. Ctrl+Z (si no quieres algo) o Ctrl+Y (para rehacer)
```

### Caso 3: Revisar Cambios
```
1. Haces 5 cambios
2. Ctrl+Z x5 ? Vuelves al estado original
3. Ctrl+Y x5 ? Vuelves a los cambios hechos
```

---

## Panel de Información (Futuro)

Próximamente podrás ver:
- Último comando deshecho (en tooltip)
- Siguiente comando para rehacer (en tooltip)
- Historial visual de comandos (panel)

---

## Configuración Avanzada

Para cambiar el límite de historial (solo desarrolladores):

```cpp
// En código
BaseApp& app = /* obtener app */;
app.getCommandInvoker().setMaxHistorySize(200);  // 200 en lugar de 100
```

---

## Preguntas Frecuentes

**P: ¿Puedo deshacer/rehacer muchos pasos?**
R: Sí, hasta 100 por defecto (configurable)

**P: ¿Los cambios se guardan automáticamente?**
R: No, solo en historial. Usa Ctrl+S para guardar la escena.

**P: ¿Qué pasa si desactivo el editor?**
R: El historial se limpia (no se persiste)

**P: ¿Funciona con múltiples objetos?**
R: Sí, cada cambio se registra independientemente

**P: ¿Hay límite de complejidad?**
R: No, puedes deshacer/rehacer cualquier tipo de transformación

---

## Atajos de Teclado Completos

### Editor (Sistema Undo/Redo)
```
Ctrl+Z    = Deshacer
Ctrl+Y    = Rehacer
```

### Gizmo 3D (ImGuizmo)
```
W         = Modo Translate
E         = Modo Rotate
R         = Modo Scale
```

### Escena (Existentes)
```
Ctrl+S    = Guardar escena
```

---

**¡Listo para usar! Simplemente presiona Ctrl+Z para deshacer cualquier cambio.**
