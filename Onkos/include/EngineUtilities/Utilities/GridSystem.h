#pragma once
#include "Prerequisites.h"
#include "EngineUtilities\Vectors\Vector3.h"
#include <imgui.h>

// Forward declarations
class Camera;
class DeviceContext;

/**
 * @class GridSystem
 * @brief Manages viewport grid visualization and object snapping to grid.
 * @author Ricardo Rabell
 * @date 2026-07-12
 *
 * @details
 * **Context:** When editing objects in 3D space, precise alignment is critical.
 * A grid provides visual reference points and snap-to-grid functionality ensures
 * objects align perfectly without manual tweaking.
 *
 * **Functionality:**
 * - Visual grid rendering in viewport (customizable size and opacity)
 * - Snap-to-grid for object positions
 * - Snap angles for rotations (15°, 45°, 90°, etc.)
 * - Snap scaling to discrete values
 * - Toggle on/off via ImGui UI
 */
class GridSystem {
public:
  /**
   * @brief Settings for grid behavior and appearance
   */
  struct GridSettings {
    float cellSize = 1.0f;              ///< Distance between grid cells
    float opacity = 0.3f;               ///< Grid line opacity (0.0 - 1.0)
    bool renderGrid = true;             ///< Show/hide grid in viewport
    bool snapEnabled = false;            ///< Enable/disable snapping
    float snapPositionSize = 0.5f;      ///< Grid cell size for position snapping
    float snapRotationAngle = 15.0f;    ///< Rotation snap angle in degrees
    float snapScaleValue = 0.1f;        ///< Scale snap increment
    bool showGridLines = true;          ///< Show grid lines
    bool showGridPlane = true;          ///< Show grid plane (Y = 0)
  };

  /**
   * @brief Default constructor
   */
  GridSystem();

  /**
   * @brief Default destructor
   */
  ~GridSystem() = default;

  /**
   * @brief Renders the grid in the viewport
   * @param drawList ImGui draw list for rendering
   * @param viewportPos Top-left position of viewport in screen space
   * @param viewportSize Dimensions of viewport
   * @param camera Active camera for world-to-screen projection
   */
  void renderGrid(ImDrawList* drawList, 
                  ImVec2 viewportPos, 
                  ImVec2 viewportSize,
                  const Camera& camera);

  /**
   * @brief Snaps a world position to the grid
   * @param position Position to snap
   * @return EU::Vector3 Snapped position
   */
  EU::Vector3 snapPosition(const EU::Vector3& position) const;

  /**
   * @brief Snaps a rotation angle to the nearest grid angle
   * @param angleRadians Angle in radians
   * @return float Snapped angle in radians
   */
  float snapRotation(float angleRadians) const;

  /**
   * @brief Snaps a scale value to the nearest grid increment
   * @param scaleValue Scale value to snap
   * @return float Snapped scale value
   */
  float snapScale(float scaleValue) const;

  /**
   * @brief Draws ImGui controls for grid settings
   * @return bool True if any settings changed
   */
  bool drawImGuiControls();

  // Getters/Setters
  void setGridSize(float size) { m_settings.cellSize = size; }
  float getGridSize() const { return m_settings.cellSize; }

  void setSnapEnabled(bool enabled) { m_settings.snapEnabled = enabled; }
  bool isSnapEnabled() const { return m_settings.snapEnabled; }

  void setRenderGrid(bool render) { m_settings.renderGrid = render; }
  bool isGridVisible() const { return m_settings.renderGrid; }

  void setOpacity(float opacity) { m_settings.opacity = std::clamp(opacity, 0.0f, 1.0f); }
  float getOpacity() const { return m_settings.opacity; }

  void setSnapPositionSize(float size) { m_settings.snapPositionSize = size; }
  float getSnapPositionSize() const { return m_settings.snapPositionSize; }

  void setSnapRotationAngle(float angle) { m_settings.snapRotationAngle = angle; }
  float getSnapRotationAngle() const { return m_settings.snapRotationAngle; }

  void setSnapScaleValue(float scale) { m_settings.snapScaleValue = scale; }
  float getSnapScaleValue() const { return m_settings.snapScaleValue; }

  GridSettings& getSettings() { return m_settings; }
  const GridSettings& getSettings() const { return m_settings; }

private:
  /**
   * @brief Projects a world position to screen space for rendering
   * @param worldPos Position in world space
   * @param camera Camera for projection
   * @param viewportPos Viewport position in screen space
   * @param viewportSize Viewport dimensions
   * @param outValid Set to true if the point is in front of the camera (valid projection)
   * @return ImVec2 Screen space position, or {-1, -1} if outside frustum
   */
  ImVec2 projectWorldToScreen(const EU::Vector3& worldPos,
                              const Camera& camera,
                              ImVec2 viewportPos,
                              ImVec2 viewportSize,
                              bool& outValid) const;

  /**
   * @brief Checks if a screen point is within the viewport bounds
   * @param screenPos Screen space position
   * @param viewportPos Viewport position
   * @param viewportSize Viewport dimensions
   * @return bool True if point is inside viewport
   */
  bool isInViewport(ImVec2 screenPos, ImVec2 viewportPos, ImVec2 viewportSize) const;

private:
  GridSettings m_settings;
};
