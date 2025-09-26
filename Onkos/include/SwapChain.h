#pragma once
#include "Prerequisites.h"

// Forward declarations
class Device;
class DeviceContext;
class Window;
class Texture;

/**
 * @class SwapChain
 * @brief Manages the front and back buffers for rendering and presentation.
 * @author Ricardo Rabell
 * @date 2025-09-19
 *
 * This class encapsulates the IDXGISwapChain, which is a fundamental component
 * of DirectX graphics. It holds a collection of buffers (typically a front buffer
 * displayed on the screen and a back buffer for rendering) and is responsible
 * for presenting the rendered image to the user.
 */
class
SwapChain {
public:
	/**
	 * @brief Default constructor.
	 */
	SwapChain() = default;
	/**
	 * @brief Default destructor.
	 */
	~SwapChain() = default;

	/**
	 * @brief Initializes the swap chain and associated DXGI components.
	 * @param device A reference to the main Device wrapper.
	 * @param deviceContext A reference to the main DeviceContext wrapper.
	 * @param backBuffer A reference to a Texture object that will be used as the back buffer.
	 * @param window The application window where the swap chain will present images.
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(Device& device, 
			 DeviceContext& deviceContext, 
			 Texture& backBuffer, 
			 Window window);

	/**
	 * @brief Per-frame update logic for the swap chain.
	 * @note Can be used to handle events like window resizing.
	 * @todo Implementation details should be added.
	 */
	void
	update();

	/**
	 * @brief Per-frame rendering logic related to the swap chain.
	 * @note This is likely a placeholder, as most rendering commands are issued
	 * via the DeviceContext.
	 * @todo Implementation details should be added.
	 */
	void
	render();
	
	/**
	 * @brief Releases the swap chain and all associated COM objects.
	 */
	void
	destroy();

	/**
	 * @brief Presents the back buffer to the screen.
	 * This function swaps the back buffer and front buffer, making the most recently
	 * rendered frame visible to the user.
	 */
	void
	present();

public:
	/** 
		* @brief The core DirectX Graphics Infrastructure (DXGI) swap chain object. 
		*/
	IDXGISwapChain* m_swapChain = nullptr;

	/** 
		* @brief The driver type used for the device (e.g., Hardware, Reference). 
		*/
	D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_NULL;

private:

	/** 
		* @brief The DirectX feature level supported by the hardware (e.g., 11_0). 
		*/
	D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

	/** 
		* @brief The number of multisamples per pixel for anti-aliasing. 
		*/
	unsigned int m_sampleCount;

	/** 
		* @brief The quality level for multisampling. 
		*/
	unsigned int m_qualityLevels;

	/** 
		* @brief A pointer to the underlying DXGI Device. 
		*/
	IDXGIDevice* m_dxgiDevice = nullptr;

	/** 
		* @brief A pointer to the graphics adapter (video card). 
		*/
	IDXGIAdapter* m_dxgiAdapter = nullptr;

	/** 
		* @brief A pointer to the DXGI Factory used to create the swap chain. 
		*/
	IDXGIFactory* m_dxgiFactory = nullptr;
};