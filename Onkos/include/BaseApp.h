#pragma once
#include "Prerequisites.h"
#include "Window.h"
#include "Device.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "Texture.h";
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "Viewport.h"
#include "ShaderProgram.h"
#include "MeshComponent.h"
#include "Buffer.h"
#include "SamplerState.h"
#include "Model3D.h"
#include "ECS/Actor.h"
#include "GUI.h"
#include "SceneGraph/SceneGraph.h"
#include "EngineUtilities/Utilities/Camera.h"
#include "EngineUtilities\Utilities\Skybox.h"
#include "EngineUtilities/Utilities/LayoutBuilder.h"

extern IMGUI_IMPL_API
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @class BaseApp
 * @brief The core application class that initializes and runs the engine.
 * @author Ricardo Rabell
 * @date 2025-10-29
 *
 * BaseApp is responsible for owning and managing the lifecycle of all primary
 * graphics objects (Device, DeviceContext, SwapChain, etc.), handling the
 * main message loop, and orchestrating the per-frame update and render calls.
 * It serves as the central hub for the entire application.
 */
class
BaseApp {
public:
	/**
	 * @brief Constructor.
	 * @param hInst Handle to the application instance.
	 * @param nCmdShow Command show parameter for the main window.
	 */
	BaseApp() = default;

	/**
	 * @brief Destructor.
	 * Automatically calls the destroy() method to ensure proper cleanup.
	 */
	~BaseApp() { destroy(); }

	/**
	* @brief Initializes subsystems that must be ready before main initialization.
	* Typically used for early resource allocation or configuration.
	* @return HRESULT S_OK if successful, otherwise an error code.
	*/
	HRESULT
	awake();

	/**
	 * @brief The main entry point and message loop of the application.
	 * This function initializes the application (via init()) and then enters
	 * the Win32 message loop, processing OS messages and triggering
	 * update() and render() calls each frame.
	 * @param hInst Handle to the application instance.
	 * @param nCmdShow Command show parameter for the main window.
	 * @return int The exit code of the application.
	 */
	int
	run(HINSTANCE hInst, int nCmdShow);

	/**
	 * @brief Initializes all application and graphics systems.
	 * This includes creating the window, initializing the D3D11 device and
	 * swap chain, creating render targets, compiling shaders, and setting
	 * up initial scene geometry and buffers.
	 * @return HRESULT S_OK if all initializations are successful.
	 */
	HRESULT
	init();

	/**
	 * @brief Updates the application logic once per frame.
	 * @param deltaTime The time elapsed since the last frame.
	 */
	void
	update(float deltaTime);

	/**
	 * @brief Renders a single frame.
	 * This function binds all necessary pipeline states (shaders, buffers,
	 * render targets) and issues the final draw calls for the scene.
	 */
	void
	render();

	/**
	 * @brief Cleans up and releases all allocated resources.
	 * This ensures all COM objects (Device, SwapChain, Buffers, etc.)
	 * are properly released and the window is destroyed.
	 */
	void
	destroy();

  /**
  * @brief Handles window resize events and updates all dependent resources.
  *
  * This function is called whenever the application window is resized.
  * It is responsible for resizing the swap chain buffers, recreating the render target
  * and depth-stencil views, and updating the viewport to match the new window dimensions.
  * All graphics resources that depend on the window size should be updated here to ensure
  * correct rendering after a resize event.
  *
  * @param newW The new width of the window in pixels.
  * @param newH The new height of the window in pixels.
  */
  void
  onResize(unsigned int newW, unsigned int newH);

private:
	/**
	 * @brief The static window procedure for handling Win32 messages.
	 * @param hWnd The handle to the window receiving the message.
	 * @param message The message identifier.
	 * @param wParam Additional message-specific information.
	 * @param lParam Additional message-specific information.
	 */
	static LRESULT CALLBACK
	wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//--------------------------------------------------------------------------------------
	// Global Variables
	//--------------------------------------------------------------------------------------
	/** @brief The main application window. */
	Window m_window;

	/** @brief The D3D11 device (resource factory). */
	Device m_device;

	/** @brief The D3D11 device context (command issuer). */
	DeviceContext m_deviceContext;

	/** @brief The DXGI swap chain for front/back buffers. */
	SwapChain m_swapChain;

	/** @brief The texture resource for the swap chain's back buffer. */
	Texture m_backBuffer;

	/** @brief The render target view (RTV) for the back buffer. */
	RenderTargetView m_renderTargetView;

	/** @brief The texture resource for the depth-stencil buffer. */
	Texture m_depthStencil;

	/** @brief The depth-stencil view (DSV) for the depth buffer. */
	DepthStencilView m_depthStencilView;

	/** @brief The viewport configuration. */
	Viewport m_viewport;

	/** @brief The vertex and pixel shader program. */
	ShaderProgram m_shaderProgram;

  /** @brief Indicates if the Direct3D device and related resources are fully initialized and ready for use. */
  bool m_d3dReady = false;
	
  /** @brief The GPU constant buffer for per-frame data (view/projection, lighting, etc.). */
  Buffer m_constantBuffer;

  /** @brief CPU-side struct holding the data to be uploaded to the main constant buffer. */
  CBMain m_constantBufferStruct;

	// Textures
  /** @brief The albedo (base color) texture used for PBR rendering. */
  Texture m_AlbedoSRV;
  /** @brief The metallic texture map used for PBR material properties. */
  Texture m_MetallicSRV;
  /** @brief The roughness texture map used for PBR material properties. */
  Texture m_RoughnessSRV;
  /** @brief The ambient occlusion (AO) texture map for PBR shading. */
  Texture m_AOSRV;
  /** @brief The normal map texture used for simulating surface detail in PBR. */
  Texture m_NormalSRV;

	/** @brief The main camera used for rendering the scene. */
	Camera m_camera;

	/** @brief The scene graph responsible for hierarchical transformations and rendering. */
	SceneGraph m_sceneGraph;

	/**
	* @brief The list of actors present in the scene.
	* Each actor is managed via a shared pointer for automatic memory management.
	*/
	std::vector<EU::TSharedPointer<Actor>> m_actors;

	/**
	* @brief Shared pointer to the main Abe Bowser actor.
	* Used for direct access and manipulation of this specific actor.
	*/
	EU::TSharedPointer<Actor> m_spitFire;

	/**
	* @brief Pointer to the loaded 3D model resource.
	* Represents the current model used in the scene.
	*/
	Model3D* m_model;

	/** @brief CPU-side struct for the 'ChangeOnResize' constant buffer. */
	//CBChangeOnResize cbChangesOnResize;

	/** @brief CPU-side struct for the 'NeverChanges' constant buffer. */
	//CBNeverChanges cbNeverChanges;

	/**
	* @brief The user interface manager for the application.
	* Handles UI rendering, input, and interaction logic.
	*/
	GUI m_gui;

  /**
   * @brief The current position of the camera in world space.
   */
  EU::Vector3 m_cameraPos;

  /**
  * @brief The skybox manager responsible for rendering the environment background.
  */
  Skybox m_skybox;

  /**
  * @brief The texture resource used for the skybox environment.
  */
  Texture m_skyboxTex;

  /**
	* @brief The default rasterizer state used for rendering.
  */
  RasterizerState m_defaultRasterizer;

  /**
	* @brief The default depth-stencil state used for depth testing.
  */
  DepthStencilState m_defaultDepthStencil;
};