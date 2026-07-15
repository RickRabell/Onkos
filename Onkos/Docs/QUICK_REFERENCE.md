# Quick Reference - DCC Camera Controller

## ?? Keyboard & Mouse Shortcuts

```
???????????????????????????????????????????????????????
?          VIEWPORT CAMERA CONTROLS                   ?
???????????????????????????????????????????????????????
?                                                     ?
?  ???  RIGHT MOUSE + DRAG     ? Orbit around object  ?
?  ???  MIDDLE MOUSE + DRAG    ? Pan (move focus)    ?
?  ???  MOUSE WHEEL UP/DOWN    ? Zoom in/out         ?
?  ?? SHIFT + RIGHT DRAG      ? Zoom alternative     ?
?  ?? F KEY                   ? Focus on Selection   ?
?                                                     ?
???????????????????????????????????????????????????????
```

## ?? Usage Examples

### Example 1: Orbiting Around an Object
```
1. Position mouse over viewport
2. Press and hold RIGHT mouse button
3. Move mouse in circular motion
4. Camera rotates around the object
5. Release button to stop
```

### Example 2: Pan the View
```
1. Position mouse over viewport
2. Press and hold MIDDLE mouse button (wheel)
3. Move mouse up/down/left/right
4. The focus point moves with your mouse
5. Release to stop panning
```

### Example 3: Zoom In/Out
```
Method A (Scroll Wheel):
  - Scroll wheel UP   ? Zoom in
  - Scroll wheel DOWN ? Zoom out

Method B (Shift + Right Drag):
  - Hold SHIFT
  - Press RIGHT mouse button
  - Drag UP to zoom in / DOWN to zoom out
```

### Example 4: Focus on Selected Object
```
1. Click on an object in the Hierarchy panel to select it
2. Position mouse over viewport
3. Press F key
4. Camera automatically frames the selected object
5. Optimal distance calculated automatically
```

## ?? Configuration (In Code)

### Initialization Parameters
```cpp
// Default values (in BaseApp::init())
m_cameraController.setSensitivity(1.0f);        // 0.1 - 5.0
m_cameraController.setZoomSensitivity(0.15f);   // 0.05 - 0.5
m_cameraController.setConstrainPitch(true);     // Prevent gimbal lock
m_cameraController.setMinDistance(0.1f);        // Minimum zoom distance
m_cameraController.setMaxDistance(1000.0f);     // Maximum zoom distance
```

### Advanced Settings
```cpp
// Enable smooth camera movement
m_cameraController.setDampingEnabled(true);
m_cameraController.setDampingFactor(0.15f);     // 0.0 (snappy) - 1.0 (smooth)

// Manually set focus point
m_cameraController.setFocusPoint(EU::Vector3(0.0f, 1.0f, 10.0f));

// Get current parameters
float distance = m_cameraController.getOrbitDistance();
EU::Vector3 focus = m_cameraController.getFocusPoint();
```

## ?? Performance Tips

### For Smooth Navigation
```cpp
// Enable damping for cinematic feel
m_cameraController.setDampingEnabled(true);
m_cameraController.setDampingFactor(0.2f);  // Adjust 0.1-0.3
```

### For Precise Control
```cpp
// Disable damping for immediate response
m_cameraController.setDampingEnabled(false);

// Increase sensitivity for faster rotations
m_cameraController.setSensitivity(2.0f);
```

### For Large Scenes
```cpp
// Increase maximum distance
m_cameraController.setMaxDistance(5000.0f);

// Decrease minimum distance to prevent clipping
m_cameraController.setMinDistance(0.01f);
```

## ?? Visual Feedback

The camera controller provides visual feedback through:
- Smooth interpolation of camera movement
- Consistent pivot point around selected objects
- Gimbal lock prevention (smooth pitch limits)
- Constraint indicators (pitch won't exceed ±90°)

## ? Troubleshooting

### "Camera isn't responding to mouse"
- ? Check if mouse is hovering over viewport
- ? Ensure Gizmo is not being used (would block input)
- ? Verify ImGui is capturing mouse correctly

### "Camera moved too fast/slow"
- ? Adjust `setSensitivity()` - increase for faster, decrease for slower
- ? Adjust `setZoomSensitivity()` - separate control for zoom speed

### "Camera jumps when I press F"
- ? This is expected behavior - camera rapidly frames selected object
- ? Use damping to smooth the transition:
  ```cpp
  m_cameraController.setDampingEnabled(true);
  m_cameraController.setDampingFactor(0.3f);  // More damping = smoother
  ```

### "I can see through the back of objects"
- ? This is because there's no collision detection
- ? Increase minimum distance if needed:
  ```cpp
  m_cameraController.setMinDistance(0.5f);
  ```

### "Pan doesn't feel right"
- ? Ensure you're using MIDDLE mouse button (not middle + other buttons)
- ? Pan distance is proportional to orbital distance
- ? If object is far away, panning moves more in world units

## ?? API Quick Reference

```cpp
// Core Methods
void init(Camera* camera, const EU::Vector3& initialFocusPoint);
void update(int mouseX, int mouseY, bool rightMouseDown, 
            bool middleMouseDown, int scrollDelta, bool shiftHeld);
void zoomToFit(const EU::Vector3& min, const EU::Vector3& max,
               unsigned int viewportWidth, unsigned int viewportHeight);
void focusOnActor(Actor* actor, unsigned int viewportWidth, 
                  unsigned int viewportHeight);

// Setters
void setSensitivity(float sensitivity);
void setZoomSensitivity(float zoomSensitivity);
void setMinDistance(float minDistance);
void setMaxDistance(float maxDistance);
void setOrbitDistance(float distance);
void setFocusPoint(const EU::Vector3& focusPoint);
void setYaw(float yaw);
void setPitch(float pitch);
void setConstrainPitch(bool constrainPitch);
void setDampingEnabled(bool enable);
void setDampingFactor(float damping);

// Getters
float getOrbitDistance() const;
EU::Vector3 getFocusPoint() const;
float getYaw() const;
float getPitch() const;
float getSensitivity() const;
float getZoomSensitivity() const;
float getMinDistance() const;
float getMaxDistance() const;
```

## ?? Related Features

- **Gizmo**: Right-click in viewport to use transform gizmo (translate/rotate/scale)
- **Selection**: Click objects in Hierarchy or use viewport picking (future)
- **Inspector**: Selected object properties appear in Inspector panel
- **Outliner**: View and select objects from scene hierarchy

## ?? Further Reading

- See `DCC_CAMERA_CONTROLLER_GUIDE.md` for detailed documentation
- See `FUTURE_IMPROVEMENTS.md` for planned enhancements
- See `IMPLEMENTATION_SUMMARY.md` for technical details

---

**Last Updated**: 2026-07-12  
**Status**: ? Ready for Production  
**Version**: 1.0
