#pragma once
#include "Prerequisites.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <imgui_internal.h>
//#include "ECS/Actor.h"
#include "ImGuizmo.h"

class Viewport;
class Device;
class DeviceContext;
class Window;
class Actor;

/**
 * @class UserInterface
 * @brief Manages the Immediate Mode GUI (ImGui) system for the engine.
 * @author Ricardo Rabell
 * @date 2025-12-05
 *
 * This class handles the initialization, updating, and rendering of the editor's
 * user interface tools using the Dear ImGui library. It acts as a bridge between
 * the engine's data (like Actors and Components) and the visual editor controls.
 * It is responsible for drawing widgets, property inspectors, and handling user input
 * for the UI layer.
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

	void
	awake();

	/**
	 * @brief Initializes the ImGui context and backends.
	 * Sets up the Win32 and DirectX 11 bindings for ImGui.
	 * @param window A handle to the OS window (HWND).
	 * @param device Pointer to the D3D11 Device.
	 * @param deviceContext Pointer to the D3D11 Device Context.
	 */
	void
	init(Window& window, Device& device, DeviceContext& deviceContext);

	/**
	 * @brief Starts a new ImGui frame.
	 * Must be called once per frame before defining any UI windows or widgets.
	 * Usually handles inputs and time delta updates for the UI.
	 */
	void
	update(Viewport& viewport, Window& window);

	/**
	 * @brief Renders the generated UI draw data.
	 * Should be called after the main scene rendering but before the SwapChain presentation.
	 */
	void
	render();

	/**
	 * @brief Shuts down the ImGui context and releases associated resources.
	 */
	void
	destroy();

	void
	toolBar();

	void
	closeApp();

	void
	toolTipData();

	/**
	 * @brief Renders a custom control widget for a 3-component vector (X, Y, Z).
	 *
	 * This helper function draws a standardized UI element for editing vectors,
	 * commonly used for Transform components (Position, Rotation, Scale).
	 *
	 * @param label The display name for the vector property.
	 * @param values A pointer to the float array (of size 3) to be modified.
	 * @param resetValues The value to set the vector to if reset (default is 0.0f).
	 * @param columnWidth The width reserved for the label text.
	 * @return bool Returns true if any value was modified in this frame.
	 */
	void
	vec3Control(const std::string& label,
							float* values,
							float resetValues = 0.0f,
							float columnWidth = 100.0f);

	/**
	 * @brief Sets the actor currently selected by the user.
	 * The UI will update to display the properties (components) of this actor.
	 * @param actor Pointer to the actor to select.
	 */
	//void
	//setSelectedActor(Actor* actor) { m_selectedActor = actor; }

	void
	inspectorGeneral(EU::TSharedPointer<Actor> actor);

	void
	inspectorContainer(EU::TSharedPointer<Actor> actor);

	void
	outliner(const std::vector<EU::TSharedPointer<Actor>>& actors);

	void
	editTransform(const XMMATRIX& view, 
								const XMMATRIX& projection, 
								EU::TSharedPointer<Actor> actor);

	void
	drawGizmoToolbar();

	// Auxiliar function to convert a XMMATRIX to what ImGuizmo wants
	void
	toFloatArray(const XMMATRIX& mat, float* dest) {
		XMFLOAT4X4 temp;
		XMStoreFloat4x4(&temp, mat);
		memcpy(dest, &temp, sizeof(float) * 16);
	}

private:
	/** @brief Pointer to the actor currently selected for inspection in the editor UI. */
	//Actor* m_selectedActor = nullptr;

	bool checkboxValue = true;
	bool checkboxValue2 = false;
	std::vector<const char*> m_objectsNames;
	std::vector<const char*> m_tooltips;

	bool show_exit_popup = false;

public:
	int selectedActorIndex = -1;
};