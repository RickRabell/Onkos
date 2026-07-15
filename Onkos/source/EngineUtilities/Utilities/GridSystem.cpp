/**
 * @file GridSystem.cpp
 * @brief Implements grid visualization and object snapping.
 * @ingroup utilities
 */
#include "EngineUtilities\Utilities\GridSystem.h"
#include "EngineUtilities\Utilities\Camera.h"
#include <cmath>
#include <algorithm>

GridSystem::GridSystem()
  : m_settings() {
}

void GridSystem::renderGrid(ImDrawList* drawList,
  ImVec2 viewportPos,
  ImVec2 viewportSize,
  const Camera& camera) {

  if (!drawList || !m_settings.renderGrid || viewportSize.x < 1.0f || viewportSize.y < 1.0f) {
    return;
  }

  ImU32 gridColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, m_settings.opacity));
  ImU32 centerColor = ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, m_settings.opacity * 1.5f));

  if (m_settings.showGridPlane) {
    // Draw grid on the Y=0 plane (world ground)
    const float gridRange = 50.0f;  // Total range to display grid
    const float step = m_settings.cellSize;

    // Draw grid lines
    for (float x = -gridRange; x <= gridRange; x += step) {
      // Line along Z axis at position X
      EU::Vector3 start(x, 0.0f, -gridRange);
      EU::Vector3 end(x, 0.0f, gridRange);

      bool validStart = false, validEnd = false;
      ImVec2 screenStart = projectWorldToScreen(start, camera, viewportPos, viewportSize, validStart);
      ImVec2 screenEnd = projectWorldToScreen(end, camera, viewportPos, viewportSize, validEnd);

      if (validStart && validEnd &&
          (isInViewport(screenStart, viewportPos, viewportSize) || isInViewport(screenEnd, viewportPos, viewportSize))) {
        // Highlight center line (X=0)
        ImU32 color = (fabs(x) < 0.01f) ? centerColor : gridColor;
        drawList->AddLine(screenStart, screenEnd, color, 1.0f);
      }
    }

    for (float z = -gridRange; z <= gridRange; z += step) {
      // Line along X axis at position Z
      EU::Vector3 start(-gridRange, 0.0f, z);
      EU::Vector3 end(gridRange, 0.0f, z);

      bool validStart = false, validEnd = false;
      ImVec2 screenStart = projectWorldToScreen(start, camera, viewportPos, viewportSize, validStart);
      ImVec2 screenEnd = projectWorldToScreen(end, camera, viewportPos, viewportSize, validEnd);

      if (validStart && validEnd &&
          (isInViewport(screenStart, viewportPos, viewportSize) || isInViewport(screenEnd, viewportPos, viewportSize))) {
        // Highlight center line (Z=0)
        ImU32 color = (fabs(z) < 0.01f) ? centerColor : gridColor;
        drawList->AddLine(screenStart, screenEnd, color, 1.0f);
      }
    }

    // Draw origin marker (0, 0, 0)
    EU::Vector3 origin(0.0f, 0.0f, 0.0f);
    bool validOrigin = false;
    ImVec2 screenOrigin = projectWorldToScreen(origin, camera, viewportPos, viewportSize, validOrigin);
    if (validOrigin && isInViewport(screenOrigin, viewportPos, viewportSize)) {
      drawList->AddCircle(screenOrigin, 4.0f, centerColor, 16, 2.0f);
    }
  }
}

EU::Vector3 GridSystem::snapPosition(const EU::Vector3& position) const {
  if (!m_settings.snapEnabled || m_settings.snapPositionSize <= 0.0f) {
    return position;
  }

  float snapSize = m_settings.snapPositionSize;
  return EU::Vector3(
    std::round(position.x / snapSize) * snapSize,
    std::round(position.y / snapSize) * snapSize,
    std::round(position.z / snapSize) * snapSize
  );
}

float GridSystem::snapRotation(float angleRadians) const {
  if (!m_settings.snapEnabled || m_settings.snapRotationAngle <= 0.0f) {
    return angleRadians;
  }

  // Convert snap angle from degrees to radians
  float snapRadians = XMConvertToRadians(m_settings.snapRotationAngle);

  // Snap to nearest angle
  return std::round(angleRadians / snapRadians) * snapRadians;
}

float GridSystem::snapScale(float scaleValue) const {
  if (!m_settings.snapEnabled || m_settings.snapScaleValue <= 0.0f) {
    return scaleValue;
  }

  // Prevent scale from going to zero
  if (scaleValue <= 0.0f) {
    return m_settings.snapScaleValue;
  }

  return std::round(scaleValue / m_settings.snapScaleValue) * m_settings.snapScaleValue;
}

namespace {
  /**
   * @brief Draws a slider that can also be edited by typing an exact value.
   * @details Double-clicking the slider opens a small popup with an input box,
   * in addition to normal slider dragging and Ctrl+Click text entry (default ImGui behavior).
   */
  bool sliderFloatWithInput(const char* label, float* v, float min, float max, const char* format = "%.2f") {
    bool changed = ImGui::SliderFloat(label, v, min, max, format);

    const bool doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
    if (doubleClicked) {
      ImGui::OpenPopup(label);
    }

    if (ImGui::BeginPopup(label)) {
      ImGui::SetNextItemWidth(120.0f);
      float temp = *v;
      if (ImGui::InputFloat("##input", &temp, 0.0f, 0.0f, format, ImGuiInputTextFlags_EnterReturnsTrue)) {
        *v = std::clamp(temp, min, max);
        changed = true;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    return changed;
  }
}

bool GridSystem::drawImGuiControls() {
  bool changed = false;

  if (ImGui::CollapsingHeader("Grid Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= ImGui::Checkbox("Render Grid", &m_settings.renderGrid);
    changed |= ImGui::Checkbox("Enable Snap", &m_settings.snapEnabled);
    changed |= ImGui::Checkbox("Show Grid Lines", &m_settings.showGridLines);
    changed |= ImGui::Checkbox("Show Grid Plane", &m_settings.showGridPlane);

    ImGui::Separator();

    changed |= sliderFloatWithInput("Grid Size##cell", &m_settings.cellSize, 0.1f, 10.0f);
    changed |= sliderFloatWithInput("Grid Opacity", &m_settings.opacity, 0.1f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Snap Settings");

    changed |= sliderFloatWithInput("Snap Position Size", &m_settings.snapPositionSize, 0.1f, 5.0f);
    changed |= sliderFloatWithInput("Snap Rotation (degrees)", &m_settings.snapRotationAngle, 1.0f, 90.0f);
    changed |= sliderFloatWithInput("Snap Scale", &m_settings.snapScaleValue, 0.05f, 1.0f);
  }

  return changed;
}

ImVec2 GridSystem::projectWorldToScreen(const EU::Vector3& worldPos,
  const Camera& camera,
  ImVec2 viewportPos,
  ImVec2 viewportSize,
  bool& outValid) const {

  // Project world position to screen space
  XMVECTOR worldVec = XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 1.0f);
  XMVECTOR projected = XMVector3Project(worldVec,
    viewportPos.x,
    viewportPos.y,
    viewportSize.x,
    viewportSize.y,
    0.0f,
    1.0f,
    camera.getProj(),
    camera.getView(),
    XMMatrixIdentity());

  float screenX = XMVectorGetX(projected);
  float screenY = XMVectorGetY(projected);
  float screenZ = XMVectorGetZ(projected);

  // Check if point is in front of camera (valid NDC depth range)
  if (screenZ < 0.0f || screenZ > 1.0f) {
    outValid = false;
    return ImVec2(-1.0f, -1.0f);
  }

  outValid = true;
  return ImVec2(screenX, screenY);
}

bool GridSystem::isInViewport(ImVec2 screenPos, ImVec2 viewportPos, ImVec2 viewportSize) const {
  return screenPos.x >= viewportPos.x &&
    screenPos.x <= viewportPos.x + viewportSize.x &&
    screenPos.y >= viewportPos.y &&
    screenPos.y <= viewportPos.y + viewportSize.y;
}
