#pragma once
#include "Prerequisites.h"

// Forward declarations to prevent circular dependencies
class Device;
class DeviceContext;
class Texture;
class DepthStencilView;

/**
 * @class RenderTargetView
 * @brief Manages a Direct3D 11 Render Target View (RTV).
 * @author Ricardo Rabell
 * @date 2025-09-25
 *
 * This class encapsulates an ID3D11RenderTargetView, which represents a resource
 * (typically a texture) that the GPU can render to. It acts as a drawing canvas
 * for the graphics pipeline.
 */
class 
RenderTargetView {
public:
	/**
	 * @brief Default constructor.
	 */
	RenderTargetView() = default;

	/**
	 * @brief Default destructor.
	 */
	~RenderTargetView() = default;

	/**
	 * @brief Initializes the render target view for a swap chain's back buffer.
	 * @param device The graphics device used to create the view.
	 * @param backBuffer The texture resource from the swap chain to be used as a render target.
	 * @param format The data format for the render target view.
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(Device& device, Texture& backBuffer, DXGI_FORMAT format);

	/**
	 * @brief Initializes a render target view for a generic texture resource.
	 * @param device The graphics device used to create the view.
	 * @param inTex The texture resource to be used as a render target.
	 * @param ViweDimension The dimension of the resource (e.g., D3D11_RTV_DIMENSION_TEXTURE2D).
	 * @param format The data format for the render target view.
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(Device& device,
			 Texture& inTex,
			 D3D11_RTV_DIMENSION ViweDimension,
			 DXGI_FORMAT format);

	/**
	 * @brief Per-frame update logic for the render target view.
	 */
	void
	update();

	/**
	 * @brief Binds the render target view to the pipeline and clears its contents.
	 * @param deviceContext The device context to issue rendering commands.
	 * @param depthStencilView The depth-stencil view to bind alongside this render target.
	 * @param numViews The number of render targets to bind (usually 1).
	 * @param ClearColor The color to clear the render target with.
	 */
	void
	render(DeviceContext& deviceContext,
				 DepthStencilView& depthStencilView,
				 unsigned int numViews,
				 const float ClearColor[4]);

	/**
	 * @brief Binds the render target view to the pipeline without clearing it.
	 * @note This is useful when you want to continue drawing to an existing target.
	 * @param deviceContext The device context to issue the binding command.
	 * @param numViews The number of render targets to bind (usually 1).
	 */
	void
	render(DeviceContext& deviceContext, unsigned int numViews);

	/**
	 * @brief Releases the underlying ID3D11RenderTargetView COM object.
	 */
	void
	destroy();

private:
	/**
	 * @brief Pointer to the underlying DirectX 11 Render Target View interface.
	 */
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
};