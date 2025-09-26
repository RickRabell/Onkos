#pragma once
#include "Prerequisites.h"

// Forward declarations
class Device;
class DeviceContext;
class Texture;

/**
 * @class DepthStencilView
 * @brief Manages a Direct3D 11 Depth Stencil View (DSV).
 * @author Ricardo Rabell
 * @date 2025-09-25
 *
 * This class encapsulates an ID3D11DepthStencilView. The depth-stencil buffer is
 * used for depth testing (to ensure correct object occlusion) and stencil testing
 * (for more advanced effects like masking or outlining).
 */
class 
DepthStencilView {
public:
	/**
	 * @brief Default constructor.
	 */
	DepthStencilView() = default;

	/**
	 * @brief Default destructor.
	 */
	~DepthStencilView() = default;

	/**
	 * @brief Initializes the depth-stencil view from a texture resource.
	 * @param device The graphics device used to create the view.
	 * @param depthStencil The texture resource that will be used as the depth-stencil buffer.
	 * @param format The data format for the depth-stencil view (e.g., DXGI_FORMAT_D24_UNORM_S8_UINT).
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(Device& device, Texture& depthStencil, DXGI_FORMAT format);

	/**
	 * @brief Per-frame update logic for the depth-stencil view.
	 */
	void
	update() {};

	/**
	 * @brief Clears the depth and stencil buffers.
	 * @note This should be called at the beginning of each frame to reset the depth/stencil state.
	 * @param deviceContext The device context used to issue the clear command.
	 */
	void
	render(DeviceContext& deviceContext);

	/**
	 * @brief Releases the underlying ID3D11DepthStencilView COM object.
	 */
	void
	destroy();

public:
	/**
	 * @brief Pointer to the underlying DirectX 11 Depth Stencil View interface.
	 */
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
};