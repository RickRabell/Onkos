#pragma once
#include "Prerequisites.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <imgui_internal.h>
#include "ImGuizmo.h"

class Viewport;
class Device;
class DeviceContext;
class Window;
class Actor;
class Camera;

/**
 * @class GUI
 * @brief Manages the graphical user interface for the engine's editor.
 * @author Ricardo Rabell
 * @date 2026-02-15
 *
 * This class is the visual bridge between the engine's data and the developer.
 * It uses Dear ImGui for standard UI elements (panels, buttons, properties)
 * and ImGuizmo for 3D viewport manipulation.
 * * Context: Without a GUI, a developer would have to hardcode coordinates to move
 * an object. This class reads the ECS data (like an Actor's Transform) and
 * presents it visually, allowing real-time edits that are fed back into the engine.
 */
class 
GUI {
public:
	/**
	 * @brief Default constructor.
	 */
	GUI() = default;

	/**
	 * @brief Default destructor.
	 */
	~GUI() = default;

	/**
	 * @brief Early initialization step, typically used for allocating UI structures before DirectX is fully ready.
	 */
	void
	awake();

	/**
	 * @brief Initializes the ImGui context, binding it to the Win32 window and DirectX 11 device.
	 * @param window The main application window (needed for input handling).
	 * @param device The D3D11 device (needed to create UI textures/fonts).
	 * @param deviceContext The D3D11 device context (needed to draw the UI).
	 */
	void
	init(Window& window, Device& device, DeviceContext& deviceContext);

	/**
	 * @brief Starts a new ImGui frame and prepares the UI layout.
	 * @param viewport The engine's viewport, used to calculate where ImGuizmo should draw its 3D controls.
	 * @param window The main window, used to poll inputs and window size.
	 */
	void
	update(Viewport& viewport, Window& window);

	/**
	 * @brief Issues the draw commands for the UI.
	 * @note This must be called at the very end of the render loop, right before SwapChain::present(),
	 * so the UI is drawn on top of the 3D scene.
	 */
	void
	render();

	/**
	 * @brief Cleans up ImGui context and resources.
	 */
	void
	destroy();

	/**
	 * @brief Draws the top main menu bar (e.g., File, Edit, View).
	 */
	void
	toolBar();

	/**
	 * @brief Triggers the application shutdown sequence.
	 */
	void
	closeApp();

	/**
	 * @brief Manages hover-over tooltips for UI elements to guide the user.
	 */
	void
	toolTipData();

	/**
	 * @brief Helper function to draw a standardized 3-component vector editor (X, Y, Z).
	 * @param label The property name (e.g., "Position").
	 * @param values Pointer to the float[3] array holding the data.
	 * @param resetValues The value to apply if the user clicks the reset button.
	 * @param columnWidth Layout formatting width.
	 */
	void
	vec3Control(const std::string& label,
							float* values,
							float resetValues = 0.0f,
							float columnWidth = 100.0f);

	/**
	 * @brief Draws the main Inspector panel for an Actor.
	 * @param actor The currently selected actor whose details will be displayed.
	 */
	void
	inspectorGeneral(EU::TSharedPointer<Actor> actor);

	/**
	 * @brief A modular container that dynamically lists all components attached to the selected Actor.
	 * @param actor The selected actor.
	 */
	void
	inspectorContainer(EU::TSharedPointer<Actor> actor);

	/**
	 * @brief Draws the Outliner (Scene Hierarchy) panel.
	 * Context: This is the tree-view list of all Actors currently in the scene,
	 * allowing the user to select them.
	 * @param actors The list of all active actors in the scene.
	 */
	void
	outliner(const std::vector<EU::TSharedPointer<Actor>>& actors);

	/**
	 * @brief Draws the 3D manipulation gizmo over the selected actor in the viewport.
	 * Context: Calculates the screen-space projection of the 3D object to draw interactive
	 * arrows (translation), rings (rotation), or boxes (scale) using ImGuizmo.
	 * @param view The active camera's view matrix.
	 * @param projection The active camera's projection matrix.
	 * @param actor The actor currently being manipulated.
	 */
	void
	editTransform(Camera& cam, Window& window, EU::TSharedPointer<Actor> actor);

	/**
	 * @brief Draws the UI toolbar to switch between Translate, Rotate, and Scale gizmo modes.
	 */
	void
	drawGizmoToolbar();

	/**
	 * @brief Adapter function to convert DirectX 11 math to ImGuizmo math.
	 * Context: ImGuizmo expects matrices as flat 16-float arrays in memory, while
	 * DirectX uses the SIMD-optimized XMMATRIX. This function bridges that gap.
	 * @param mat The DirectX matrix to convert.
	 * @param dest The pre-allocated float array (size 16) to receive the data.
	 */
	void
	toFloatArray(const XMMATRIX& mat, float* dest) {
		XMFLOAT4X4 temp;
		XMStoreFloat4x4(&temp, mat);
		memcpy(dest, &temp, sizeof(float) * 16);
	}

private:
  /**
  * @brief Example checkbox value for UI state.
  * Used to store the state of a sample checkbox in the GUI.
  */
  bool checkboxValue = true;

  /**
  * @brief Example secondary checkbox value for UI state.
  * Used to store the state of a second checkbox in the GUI.
  */
  bool checkboxValue2 = false;

  /**
  * @brief List of object names displayed in the UI.
  * Holds C-style string pointers to the names of objects shown in panels such as the outliner.
  */
  std::vector<const char*> m_objectsNames;

  /**
  * @brief List of tooltips for UI elements.
  * Holds C-style string pointers to tooltip texts that provide contextual help in the GUI.
  */
  std::vector<const char*> m_tooltips;

  /**
  * @brief Flag to control the display of the exit confirmation popup.
  * True if the exit popup should be shown, false otherwise.
  */
  bool show_exit_popup = false;

public:
	/** @brief The index of the currently selected actor in the outliner. -1 means no selection. */
	int selectedActorIndex = -1;
};