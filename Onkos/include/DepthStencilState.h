#pragma once
#include "Prerequisites.h"

// Forward Declarations
class Device;
class DeviceContext;

/**
 * @class DepthStencilState
 * @brief Manages the Depth (Z-Buffer) and Stencil testing stages of the pipeline.
 * @author Ricardo Rabell
 * @date 2026-03-02
 *
 * @details
 * **Context:** When rendering 3D, we must ensure objects closer to the camera obscure
 * objects further away. The GPU does this by writing the distance (depth) of every
 * pixel to a Z-Buffer. Before drawing a new pixel, it compares its depth against the
 * existing one.
 * * **Why it exists:** Sometimes we need to break the default rules. For a Skybox, we want
 * to draw it at maximum depth, so we change the comparison function to `LESS_EQUAL`.
 * For UI elements or outlines, we might disable depth testing entirely, or use the
 * Stencil buffer to "mask" certain pixels. This class wraps that configuration.
 * * **Analogy:** The Depth Stencil state is like a bouncer at a club for pixels. The Depth test
 * checks the pixel's ID (its distance). If a closer pixel is already inside the club
 * (the screen), the bouncer rejects the new pixel. The Stencil test is like a VIP list;
 * it can mask out specific areas of the screen where pixels are or aren't allowed to render.
 */
class 
DepthStencilState {
public:
	/** @brief Default constructor. */
	DepthStencilState() = default;

	/** @brief Default destructor. */
	~DepthStencilState() = default;

	/**
	* @brief Creates the depth-stencil state object.
	* @param device The D3D11 device.
	* @param depthEnable True to enable Z-Buffering, false to disable it (e.g., for UI).
	* @param writeMask Controls whether depth data is written to the buffer (D3D11_DEPTH_WRITE_MASK_ALL or ZERO).
	* @param depthFunc The comparison function to pass the depth test (usually D3D11_COMPARISON_LESS).
	* @return HRESULT S_OK on success.
	*/
	HRESULT
	init(Device& device,
			 bool depthEnable,
			 D3D11_DEPTH_WRITE_MASK writeMask,
		 	 D3D11_COMPARISON_FUNC depthFunc);

	/** @brief Placeholder for potential dynamic updates. */
	void
	update();

	/**
	* @brief Binds this state to the Output Merger (OM) stage of the pipeline.
	* @param deviceContext The D3D11 device context.
	* @param stencilRef The reference value to use when performing stencil tests.
	* @param reset Currently unused parameter.
	*/
	void
	render(DeviceContext& deviceContext, unsigned int stencilRef = 0, bool reset = false);

	/** @brief Releases the underlying COM object. */
	void
	destroy();

private:
	/** @brief The underlying D3D11 Depth Stencil State interface. */
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
};