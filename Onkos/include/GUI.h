#pragma once
#include "Prerequisites.h"
#define	IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <imgui_internal.h>
#include "ImGuizmo.h"

// Forward Declarations
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
	 * @brief Applies a custom Apple-like liquid style to the UI elements.
	 * @param opacity The opacity of the UI elements (0.0 to 1.0).
	 * @param accent The accent color to use for highlights.
	 */
	void
	appleLiquidStyle(float opacity /*0..1f*/, ImVec4 accent /*=#0A84FF*/);

	/**
	 * @brief Applies a custom Unreal Engine 5 style to the UI elements.
	 */
	void
	unrealEngine5Style();

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
							float columnWidth = 100.0f,
							bool displayAsDegrees = false);

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
	 * @param gridSystem The grid system used to determine snap values and toggle state.
	 */
	void
	editTransform(Camera& cam, 
								Window& window, 
								EU::TSharedPointer<Actor> actor, 
								class GridSystem* gridSystem = nullptr);

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

	//---------------------
	/**
	 * @brief Draws the top ribbon of the studio interface.
	 */
	void
	drawStudioTopRibbon();

	/**
	 * @brief Draws the main viewport panel where the 3D scene is rendered.
	 * @param viewportSRV The shader resource view of the viewport texture.
	 * @param actors The list of all active actors in the scene.
	 * @param camera The active camera used for rendering.
	 * @param window The window context for the viewport.
	 * @param selectedActor The currently selected actor.
	 * @param lightIconSRV The shader resource view of the light icon texture.
	 * @param gridSystem The grid system for rendering and snapping.
	 */
	void
	drawViewportPanel(ID3D11ShaderResourceView* viewportSRV,
										const std::vector<EU::TSharedPointer<Actor>>& actors,
										Camera& camera,
										Window& window,
										EU::TSharedPointer<Actor> selectedActor,
										ID3D11ShaderResourceView* lightIconSRV,
										class GridSystem* gridSystem);

	/**
	 * @brief Draws the light icons in the viewport.
	 * @param actors The list of all active actors in the scene.
	 * @param camera The active camera used for rendering.
	 * @param lightIconSRV The shader resource view of the light icon texture.
	 */
	void
	drawLightIcons(const std::vector<EU::TSharedPointer<Actor>>& actors,
								 Camera& camera,
								 ID3D11ShaderResourceView* lightIconSRV);

	/**
	 * @brief Draws the render debug panel.
	 * @param preShadowSRV The shader resource view of the pre-shadow texture.
	 * @param finalViewportSRV The shader resource view of the final viewport texture.
	 * @param shadowMapSRV The shader resource view of the shadow map texture.
	 */
	void
	drawRenderDebugPanel(ID3D11ShaderResourceView* preShadowSRV,
											 ID3D11ShaderResourceView* finalViewportSRV,
											 ID3D11ShaderResourceView* shadowMapSRV);

	/**
	 * @brief Draws the GBuffer debug panel.
	 * @param albedoMetallicSRV The shader resource view of the albedo and metallic texture.
	 * @param normalRoughnessSRV The shader resource view of the normal and roughness texture.
	 * @param worldAoSRV The shader resource view of the world ambient occlusion texture.
	 * @param emissiveAlphaSRV The shader resource view of the emissive and alpha texture.
	 * @param selectedActor The currently selected actor.
	 */
	void
	drawGBufferDebugPanel(ID3D11ShaderResourceView* albedoMetallicSRV,
												ID3D11ShaderResourceView* normalRoughnessSRV,
												ID3D11ShaderResourceView* worldAoSRV,
												ID3D11ShaderResourceView* emissiveAlphaSRV,
												EU::TSharedPointer<Actor> selectedActor);

	/**
	 * @brief Draws the editor tools panel with grid and snap settings.
	 * @param gridSystem The grid system for visualization and snapping.
	 */
	void
	drawEditorToolsPanel(class GridSystem* gridSystem);

	/**
	 * @brief Draws the floating "Post-Processing Effects" panel (Grayscale, Depth Fog, Vignette).
	 * Context: This method owns no state itself -- it edits the PostProcessSystem's settings
	 * struct in place, so BaseApp only has to pass the struct in once per frame and hand the
	 * (possibly updated) same struct to PostProcessSystem::render() afterwards.
	 * @param settings Reference to the active PostProcessSystem::m_settings instance.
	 */
	void
	drawPostProcessPanel(struct PostProcessSettings& settings);

	/**
	 * @brief Draws the editor dockspace.
	 */
	void
	drawEditorDockspace();

	/**
	 * @brief Consume de forma atomica la solicitud de guardado emitida desde la UI.
	 * @return `true` una sola vez por peticion de guardado.
	 */
	bool
	consumeSaveSceneRequest() {
		const bool requested = m_requestSaveScene;
		m_requestSaveScene = false;
		return requested;
	}

	/**
	 * @brief Consume de forma atomica la solicitud de creaci�n de un actor de luz emitida desde la UI.
	 * @return `true` una sola vez por peticion de creaci�n de actor de luz.
	 */
	bool
	consumeCreateLightActorRequest() {
		const bool requested = m_requestCreateLightActor;
		m_requestCreateLightActor = false;
		return requested;
	}

	/**
	 * @brief Sets a reference to the CommandInvoker for undo/redo operations.
	 * @param commandInvoker Reference to the CommandInvoker.
	 */
	void 
	setCommandInvoker(class CommandInvoker* commandInvoker) {
		m_commandInvoker = commandInvoker;
	}

	//---------------------

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

	/**
	 * @brief Flag to indicate if a save scene request has been made.
	 */
	bool m_requestSaveScene = false;

	/**
	 * @brief Flag to indicate if a create light actor request has been made.
	 */
	bool m_requestCreateLightActor = false;

	/**
	 * @brief Pointer to the draw list of the viewport.
	 */
	ImDrawList* m_viewportDrawList = nullptr;

	/**
	 * @brief Pointer to the viewport window.
	 */
	ImGuiWindow* m_viewportWindow = nullptr;

	/**
	 * @brief Flag to indicate if the viewport is visible this frame.
	 */
	bool m_viewportVisibleThisFrame = false;

	/**
	 * @brief Flag to indicate if the viewport is active this frame.
	 */
	bool m_viewportActive = false;

	/**
	 * @brief Shader resource view for pre-shadow rendering debug.
	 */
	ID3D11ShaderResourceView* m_renderDebugPreShadowSRV = nullptr;

	/**
	 * @brief Shader resource view for final rendering debug.
	 */
	ID3D11ShaderResourceView* m_renderDebugFinalSRV = nullptr;

	/**
	 * @brief Shader resource view for shadow map rendering debug.
	 */
	ID3D11ShaderResourceView* m_renderDebugShadowMapSRV = nullptr;

public:
	/** @brief Flag to indicate if the gizmo is currently being used. */
	bool m_isUsingGizmo = false;

	/** @brief Flag to indicate if the deferred shadow factor should be visualized. */
	bool m_visualizeDeferredShadowFactor = false;

	/** @brief The current debug view mode for deferred rendering. */
	int m_deferredDebugViewMode = 0;

	/** @brief The index of the currently selected actor in the outliner. -1 means no selection. */
	int selectedActorIndex = -1;

	/** @brief The position of the viewport in the GUI. */
	ImVec2 m_viewportPos = ImVec2(0.0f, 0.0f);

	/** @brief The size of the viewport in the GUI. */
	ImVec2 m_viewportSize = ImVec2(0.0f, 0.0f);

	/** @brief Flag to indicate if the viewport is hovered. */
	bool m_viewportHovered = false;

	/** @brief Flag to indicate if the viewport is focused. */
	bool m_viewportFocused = false;

	/** @brief Pointer to the CommandInvoker for undo/redo operations. */
	class CommandInvoker* m_commandInvoker = nullptr;

	/** @brief Flag to indicate if the editor dockspace layout should be reset to its default (UE5-style) arrangement. */
	bool m_requestResetLayout = false;

private:
	/** @brief Tracks if gizmo was being used last frame (for detecting release). */
	bool m_wasUsingGizmo = false;

	/** @brief Stores the initial transform before gizmo manipulation starts. */
	EU::Vector3 m_gizmoStartPosition;
	/** @brief Stores the initial rotation before gizmo manipulation starts. */
	EU::Vector3 m_gizmoStartRotation;
	/** @brief Stores the initial scale before gizmo manipulation starts. */
	EU::Vector3 m_gizmoStartScale;

	/** @brief Pointer to the actor currently being edited by the gizmo. */
	EU::TSharedPointer<Actor> m_gizmoEditingActor;
};