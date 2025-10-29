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
	BaseApp(HINSTANCE hInst, int nCmdShow);

	/**
	 * @brief Destructor.
	 * Automatically calls the destroy() method to ensure proper cleanup.
	 */
	~BaseApp() { destroy(); }

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

private:
	/**
	 * @brief The static window procedure for handling Win32 messages.
	 * @param hWnd The handle to the window receiving the message.
	 * @param message The message identifier.
	 * @param wParam Additional message-specific information.
	 * @param lParam Additional message-specific information.
	 * @return LRESULT The result of the message processing.
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
	/** @brief The CPU-side mesh data (vertices/indices). */
	MeshComponent m_mesh;
	/** @brief The GPU-side vertex buffer. */
	Buffer m_vertexBuffer;
	/** @brief The GPU-side index buffer. */
	Buffer m_indexBuffer;
	/** @brief GPU constant buffer for data updated once (e.g., View matrix). */
	Buffer m_cbNeverChanges;
	/** @brief GPU constant buffer for data updated on resize (e.g., Projection matrix). */
	Buffer m_cbChangeOnResize;
	/** @brief GPU constant buffer for data updated every frame (e.g., World matrix). */
	Buffer m_cbChangesEveryFrame;
	/** @brief A sample texture for the mesh. */
	Texture m_textureCube;
	/** @brief The sampler state for texture sampling. */
	SamplerState m_samplerState;

	/** @brief The world transformation matrix. */
	XMMATRIX m_World;
	/** @brief The view (camera) transformation matrix. */
	XMMATRIX m_View;
	/** @brief The projection (perspective) transformation matrix. */
	XMMATRIX m_Projection;
	/** @brief A color tint for the mesh. */
	XMFLOAT4 m_vMeshColor;

	/** @brief CPU-side struct for the 'ChangeOnResize' constant buffer. */
	CBChangeOnResize cbChangesOnResize;
	/** @brief CPU-side struct for the 'NeverChanges' constant buffer. */
	CBNeverChanges cbNeverChanges;
	/** @brief CPU-side struct for the 'ChangesEveryFrame' constant buffer. */
	CBChangesEveryFrame cb;
};