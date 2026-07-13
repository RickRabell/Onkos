#pragma once
#include "Prerequisites.h"
#include "Camera.h"
#include "EngineUtilities\Vectors\Vector3.h"

/**
 * @class DCCCameraController
 * @brief Manages DCC (Digital Content Creation) style camera navigation.
 * @author Ricardo Rabell
 * @date 2026-07-12
 *
 * This class handles camera interactions typical in professional 3D software
 * (like Maya, Blender, 3ds Max):
 * - **Orbit**: Right mouse button + drag to rotate around a focus point
 * - **Pan**: Middle mouse button + drag to move the camera within the view plane
 * - **Zoom**: Mouse wheel scroll or Shift + Right drag to zoom in/out
 *
 * The camera orbits around a focus point rather than rotating in place,
 * providing intuitive navigation for examining objects from all angles.
 */
class 
DCCCameraController {
public:
  /**
   * @brief Default constructor.
   * Initializes the controller with default parameters.
   */
  DCCCameraController();

  /**
   * @brief Default destructor.
   */
  ~DCCCameraController() = default;

  /**
   * @brief Initialize the controller with a camera and focus point.
   * @param camera Pointer to the camera to control.
   * @param initialFocusPoint The initial point the camera orbits around.
   */
  void 
  init(Camera* camera, const EU::Vector3& initialFocusPoint);

  /**
   * @brief Updates camera based on current input and interaction state.
   * Should be called once per frame.
   * @param currentMouseX Current mouse X position in screen space.
   * @param currentMouseY Current mouse Y position in screen space.
   * @param rightMouseDown True if right mouse button is pressed.
   * @param middleMouseDown True if middle mouse button is pressed.
   * @param scrollDelta Mouse wheel scroll amount (positive = scroll up/forward).
   * @param leftShiftHeld True if left Shift key is held.
   */
  void 
  update(int currentMouseX, int currentMouseY, bool rightMouseDown,
         bool middleMouseDown, int scrollDelta, bool leftShiftHeld);

  /**
   * @brief Frames all objects in view by computing bounding box and zooming to fit.
   * @param boundingBoxMin Minimum corner of the bounding box.
   * @param boundingBoxMax Maximum corner of the bounding box.
   * @param viewportWidth Width of the viewport in pixels.
   * @param viewportHeight Height of the viewport in pixels.
   */
  void 
  zoomToFit(const EU::Vector3& boundingBoxMin,
            const EU::Vector3& boundingBoxMax,
            unsigned int viewportWidth,
            unsigned int viewportHeight);

  /**
   * @brief Frames a specific actor by computing its world-space bounding box and zooming to fit.
   * @param actor The actor to frame.
   * @param viewportWidth Width of the viewport in pixels.
   * @param viewportHeight Height of the viewport in pixels.
   */
  void 
  focusOnActor(class Actor* actor, unsigned int viewportWidth, unsigned int viewportHeight);

  /**
   * @brief Sets the distance at which the camera orbits from the focus point.
   * @param distance The orbital radius.
   */
  void 
  setOrbitDistance(float distance) { m_orbitDistance = distance; }

  /**
   * @brief Gets the current orbital distance.
   * @return float The distance from the focus point to the camera.
   */
  float 
  getOrbitDistance() const { return m_orbitDistance; }

  /**
   * @brief Sets the focus point around which the camera orbits.
   * @param focusPoint The new focus point in world space.
   */
  void 
  setFocusPoint(const EU::Vector3& focusPoint) { m_focusPoint = focusPoint; }

  /**
   * @brief Gets the current focus point.
   * @return EU::Vector3 The focus point in world space.
   */
  EU::Vector3 
  getFocusPoint() const { return m_focusPoint; }

  /**
   * @brief Sets the camera's horizontal rotation (yaw) in radians.
   * @param yaw The yaw angle.
   */
  void 
  setYaw(float yaw) { m_yaw = yaw; }

  /**
   * @brief Gets the camera's current horizontal rotation (yaw) in radians.
   * @return float The yaw angle.
   */
  float 
  getYaw() const { return m_yaw; }

  /**
   * @brief Sets the camera's vertical rotation (pitch) in radians.
   * @param pitch The pitch angle.
   */
  void 
  setPitch(float pitch) { m_pitch = pitch; }

  /**
   * @brief Gets the camera's current vertical rotation (pitch) in radians.
   * @return float The pitch angle.
   */
  float 
  getPitch() const { return m_pitch; }

  /**
   * @brief Sets the sensitivity of orbit/pan/zoom operations.
   * @param sensitivity A multiplier for mouse movement (default ~1.0f).
   */
  void 
  setSensitivity(float sensitivity) { m_sensitivity = sensitivity; }

  /**
   * @brief Gets the current sensitivity setting.
   * @return float The sensitivity multiplier.
   */
  float 
  getSensitivity() const { return m_sensitivity; }

  /**
   * @brief Sets the zoom sensitivity multiplier.
   * @param zoomSensitivity Multiplier for scroll wheel zoom (default ~0.1f).
   */
  void 
  setZoomSensitivity(float zoomSensitivity) { m_zoomSensitivity = zoomSensitivity; }

  /**
   * @brief Gets the zoom sensitivity multiplier.
   * @return float The zoom sensitivity.
   */
  float 
  getZoomSensitivity() const { return m_zoomSensitivity; }

  /**
   * @brief Sets the minimum allowed orbital distance from the focus point.
   * @param minDistance The minimum distance (prevents clipping into objects).
   */
  void 
  setMinDistance(float minDistance) { m_minDistance = minDistance; }

  /**
   * @brief Sets the maximum allowed orbital distance from the focus point.
   * @param maxDistance The maximum distance (prevents pulling too far back).
   */
  void 
  setMaxDistance(float maxDistance) { m_maxDistance = maxDistance; }

  /**
   * @brief Gets the minimum allowed orbital distance.
   * @return float The minimum distance.
   */
  float 
  getMinDistance() const { return m_minDistance; }

  /**
   * @brief Gets the maximum allowed orbital distance.
   * @return float The maximum distance.
   */
  float 
  getMaxDistance() const { return m_maxDistance; }

  /**
   * @brief Constrains pitch to avoid gimbal lock (prevents looking "upside down").
   * @param constrainPitch If true, pitch is clamped to [-PI/2, PI/2].
   */
  void 
  setConstrainPitch(bool constrainPitch) { m_constrainPitch = constrainPitch; }

  /**
   * @brief Enables or disables orbit damping (smoothing) for a more organic feel.
   * @param enable If true, applies damping to orbit movements.
   */
  void 
  setDampingEnabled(bool enable) { m_dampingEnabled = enable; }

  /**
   * @brief Sets the damping factor (0.0f = no smoothing, 1.0f = very smooth).
   * @param damping The damping coefficient.
   */
  void 
  setDampingFactor(float damping) { m_dampingFactor = damping; }

private:
  /**
   * @brief Internal helper to update the camera's position based on orbital parameters.
   * Called after yaw/pitch/distance changes to recompute the world-space camera position.
   */
  void 
  updateCameraPosition();

  /**
   * @brief Computes the bounding box of an actor and all its children recursively.
   * @param actor The actor to compute the bounding box for.
   * @param outMin Output: minimum corner of the bounding box.
   * @param outMax Output: maximum corner of the bounding box.
   * @param initialized If true on input, outMin/outMax are expanded to include the actor's box.
   *                    If false on input, they are set to the actor's box.
   */
  void 
  computeActorBoundingBox(class Actor* actor, EU::Vector3& outMin, EU::Vector3& outMax, bool& initialized);

private:
  /** @brief Pointer to the camera being controlled. */
  Camera* m_camera = nullptr;

  /** @brief The point in world space around which the camera orbits. */
  EU::Vector3 m_focusPoint{ 0.0f, 0.0f, 0.0f };

  /** @brief The distance from the focus point to the camera position. */
  float m_orbitDistance = 10.0f;

  /** @brief Horizontal rotation angle around the Y-axis (in radians). */
  float m_yaw = 0.0f;

  /** @brief Vertical rotation angle around the local right axis (in radians). */
  float m_pitch = XM_PIDIV4; // ~45 degrees

  /** @brief Previous frame's mouse X position for delta calculation. */
  int m_prevMouseX = 0;

  /** @brief Previous frame's mouse Y position for delta calculation. */
  int m_prevMouseY = 0;

  /** @brief Mouse movement sensitivity multiplier. */
  float m_sensitivity = 1.0f;

  /** @brief Zoom sensitivity multiplier for scroll wheel. */
  float m_zoomSensitivity = 0.1f;

  /** @brief Minimum allowed orbital distance. */
  float m_minDistance = 0.1f;

  /** @brief Maximum allowed orbital distance. */
  float m_maxDistance = 1000.0f;

  /** @brief If true, constrains pitch to avoid gimbal lock. */
  bool m_constrainPitch = true;

  /** @brief If true, applies damping to orbit movements for smoothness. */
  bool m_dampingEnabled = false;

  /** @brief Damping factor for smoothing (0.0f to 1.0f). */
  float m_dampingFactor = 0.2f;

  /** @brief Target yaw for damping calculations. */
  float m_targetYaw = 0.0f;

  /** @brief Target pitch for damping calculations. */
  float m_targetPitch = XM_PIDIV4;

  /** @brief Target distance for damping calculations. */
  float m_targetDistance = 10.0f;
};
