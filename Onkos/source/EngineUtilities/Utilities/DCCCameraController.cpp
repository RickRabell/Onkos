/**
 * @file DCCCameraController.cpp
 * @brief Implements DCC-style camera navigation (Orbit, Pan, Zoom-to-fit).
 * @ingroup utilities
 */
#include "EngineUtilities\Utilities\DCCCameraController.h"
#include "ECS\Actor.h"
#include "ECS\Transform.h"
#include "ECS\MeshRendererComponent.h"
#include "Rendering\Mesh.h"
#include <algorithm>
#include <cmath>

// Undefine Windows macros that conflict with std functions
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

DCCCameraController::DCCCameraController()
  : m_camera(nullptr),
  m_focusPoint(0.0f, 0.0f, 0.0f),
  m_orbitDistance(10.0f),
  m_yaw(0.0f),
  m_pitch(XM_PIDIV4),
  m_prevMouseX(0),
  m_prevMouseY(0),
  m_sensitivity(1.0f),
  m_zoomSensitivity(0.1f),
  m_minDistance(0.1f),
  m_maxDistance(1000.0f),
  m_constrainPitch(true),
  m_dampingEnabled(false),
  m_dampingFactor(0.2f),
  m_targetYaw(0.0f),
  m_targetPitch(XM_PIDIV4),
  m_targetDistance(10.0f) {
}

void
DCCCameraController::init(Camera* camera, const EU::Vector3& initialFocusPoint) {
  m_camera = camera;
  m_focusPoint = initialFocusPoint;
  m_targetYaw = m_yaw;
  m_targetPitch = m_pitch;
  m_targetDistance = m_orbitDistance;
  updateCameraPosition();
}

void
DCCCameraController::update(int currentMouseX, int currentMouseY, bool rightMouseDown,
  bool middleMouseDown, int scrollDelta, bool leftShiftHeld) {

  if (!m_camera) return;

  // Calculate mouse deltas
  int deltaX = currentMouseX - m_prevMouseX;
  int deltaY = currentMouseY - m_prevMouseY;
  m_prevMouseX = currentMouseX;
  m_prevMouseY = currentMouseY;

  // Orbit: Right Mouse Button
  if (rightMouseDown && !leftShiftHeld) {
    m_targetYaw -= deltaX * m_sensitivity * 0.01f;
    m_targetPitch += deltaY * m_sensitivity * 0.01f;

    // Constrain pitch to avoid gimbal lock
    if (m_constrainPitch) {
      const float maxPitch = XM_PIDIV2 - 0.01f;
      const float minPitch = -XM_PIDIV2 + 0.01f;
      m_targetPitch = std::clamp(m_targetPitch, minPitch, maxPitch);
    }
  }

  // Pan: Middle Mouse Button
  if (middleMouseDown) {
    // Pan moves the camera within the view plane
    // Using camera's right and up vectors
    EU::Vector3 panRight = m_camera->GetRight() * (-deltaX * m_sensitivity * 0.01f * m_orbitDistance * 0.1f);
    EU::Vector3 panUp = m_camera->GetUp() * (deltaY * m_sensitivity * 0.01f * m_orbitDistance * 0.1f);
    m_focusPoint = m_focusPoint + panRight + panUp;
  }

  // Zoom: Scroll Wheel or Shift + Right Mouse
  int zoomDelta = 0;
  if (scrollDelta != 0) {
    zoomDelta = scrollDelta;
  }
  else if (rightMouseDown && leftShiftHeld) {
    zoomDelta = -deltaY; // Shift + right drag zooms
  }

  if (zoomDelta != 0) {
    m_targetDistance *= (1.0f - zoomDelta * m_zoomSensitivity * 0.1f);
    m_targetDistance = std::clamp(m_targetDistance, m_minDistance, m_maxDistance);
  }

  // Apply damping if enabled
  if (m_dampingEnabled) {
    m_yaw = m_yaw + (m_targetYaw - m_yaw) * m_dampingFactor;
    m_pitch = m_pitch + (m_targetPitch - m_pitch) * m_dampingFactor;
    m_orbitDistance = m_orbitDistance + (m_targetDistance - m_orbitDistance) * m_dampingFactor;
  }
  else {
    m_yaw = m_targetYaw;
    m_pitch = m_targetPitch;
    m_orbitDistance = m_targetDistance;
  }

  // Update the camera position based on orbital parameters
  updateCameraPosition();
}

void
DCCCameraController::updateCameraPosition() {
  if (!m_camera) return;

  // Compute the camera position using spherical coordinates
  // Position = FocusPoint + (distance * right_direction * sin(yaw) + distance * up_direction * sin(pitch) - distance * forward_direction * cos(pitch) * cos(yaw))

  float cosYaw = cosf(m_yaw);
  float sinYaw = sinf(m_yaw);
  float cosPitch = cosf(m_pitch);
  float sinPitch = sinf(m_pitch);

  // Position relative to focus point
  EU::Vector3 positionOffset;
  positionOffset.x = m_orbitDistance * sinYaw * cosPitch;
  positionOffset.y = m_orbitDistance * sinPitch;
  positionOffset.z = m_orbitDistance * cosYaw * cosPitch;

  EU::Vector3 newPosition = m_focusPoint + positionOffset;
  m_camera->setPosition(newPosition);

  // Camera looks at the focus point
  m_camera->lookAt(newPosition, m_focusPoint, EU::Vector3(0.0f, 1.0f, 0.0f));
  m_camera->updateViewMatrix();
}

void
DCCCameraController::zoomToFit(const EU::Vector3& boundingBoxMin,
  const EU::Vector3& boundingBoxMax,
  unsigned int viewportWidth,
  unsigned int viewportHeight) {

  if (!m_camera) return;

  // Calculate bounding box center and extents
  EU::Vector3 boxCenter = (boundingBoxMin + boundingBoxMax) * 0.5f;
  EU::Vector3 boxExtents = (boundingBoxMax - boundingBoxMin) * 0.5f;

  // Calculate the maximum extent
  float extX = fabsf(boxExtents.x);
  float extY = fabsf(boxExtents.y);
  float extZ = fabsf(boxExtents.z);
  float maxExtent = std::max(extX, std::max(extY, extZ));
  if (maxExtent < 0.001f) maxExtent = 1.0f; // Prevent division by zero

  // Set focus point to the center of the bounding box
  m_focusPoint = boxCenter;
  m_targetYaw = m_yaw;
  m_targetPitch = m_pitch;

  // Calculate required distance based on FOV
  float fovY = m_camera->getFovY();
  float aspectRatio = m_camera->getAspect();

  // Distance needed to view the object with some padding
  float distance = maxExtent / sinf(fovY * 0.5f);
  distance *= 1.5f; // Add 50% padding

  m_targetDistance = distance;
  if (!m_dampingEnabled) {
      m_orbitDistance = m_targetDistance;
      }

      updateCameraPosition();
    }

    void
    DCCCameraController::focusOnActor(Actor* actor, unsigned int viewportWidth, unsigned int viewportHeight) {
  if (!actor) return;

  EU::Vector3 boxMin = EU::Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
  EU::Vector3 boxMax = EU::Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  bool initialized = false;

  computeActorBoundingBox(actor, boxMin, boxMax, initialized);

  if (initialized) {
    zoomToFit(boxMin, boxMax, viewportWidth, viewportHeight);
  }
}

void
DCCCameraController::computeActorBoundingBox(Actor* actor, EU::Vector3& outMin, EU::Vector3& outMax, bool& initialized) {
  if (!actor) return;

  // Get the actor's transform
  EU::TSharedPointer<Transform> transform = actor->getComponent<Transform>();
  if (!transform) return;

  // Get the mesh renderer component
  EU::TSharedPointer<MeshRendererComponent> meshRenderer = actor->getComponent<MeshRendererComponent>();
  if (meshRenderer) {
    // TODO: Get mesh bounds from the MeshRendererComponent
    // For now, we'll approximate based on a unit cube at the actor's position
    EU::Vector3 actorPos = transform->getPosition();
    float radius = 1.0f; // Default radius

    if (!initialized) {
      outMin = actorPos - EU::Vector3(radius, radius, radius);
      outMax = actorPos + EU::Vector3(radius, radius, radius);
      initialized = true;
    }
    else {
      outMin.x = std::min(outMin.x, actorPos.x - radius);
      outMin.y = std::min(outMin.y, actorPos.y - radius);
      outMin.z = std::min(outMin.z, actorPos.z - radius);

      outMax.x = std::max(outMax.x, actorPos.x + radius);
      outMax.y = std::max(outMax.y, actorPos.y + radius);
      outMax.z = std::max(outMax.z, actorPos.z + radius);
    }
  }

  // Recursively compute bounding boxes for all children
  // TODO: Implement once the scene graph supports hierarchical queries
}
