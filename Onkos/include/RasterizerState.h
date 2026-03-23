#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

/**
 * @class RasterizerState
 * @brief Configures the rasterization stage of the graphics pipeline.
 * @author Ricardo Rabell
 * @date 2026-03-02
 *
 * @details
 * **Context:** Rasterization is the process of taking 3D mathematical triangles
 * and converting them into 2D pixels on the screen.
 * * **Why it exists:** This state allows us to optimize and alter how triangles are filled.
 * "Culling" tells the GPU to ignore triangles facing away from the camera (saving massive
 * amounts of processing). The "Fill Mode" lets us draw objects as solid shapes or wireframes.
 * * **Analogy:** Imagine baking cookies with a cookie cutter. The Rasterizer State decides
 * if you keep the whole cookie (Solid Fill) or just the crust (Wireframe). Culling is
 * like deciding you're not going to bother decorating the bottom of the cookie because
 * nobody will see it anyway.
 */
class 
RasterizerState {
public:
	/** @brief Default constructor. */
	RasterizerState() = default;

	/** @brief Default destructor. */
	~RasterizerState() = default;

	/**
	 * @brief Initializes the rasterizer state with default engine parameters.
	 * @param device The D3D11 Device.
	 * @return HRESULT S_OK on success.
	 */
	HRESULT
	init(Device device);

	/**
	 * @brief Initializes the rasterizer state with specific configurations.
	 * @param device The D3D11 Device.
	 * @param fill D3D11_FILL_SOLID (normal rendering) or D3D11_FILL_WIREFRAME (debugging).
	 * @param cull Determines which triangles to ignore based on their facing direction (NONE, FRONT, BACK).
	 * @param frontCCW Determines triangle winding order (Counter-Clockwise). True means CCW is considered "Front".
	 * @param depthClip Enable clipping based on distance.
	 * @return HRESULT S_OK on success.
	 */
	HRESULT
	init(Device& device,
			 D3D11_FILL_MODE fill,
			 D3D11_CULL_MODE cull,
			 bool frontCCW,
			 bool depthClip);

	/** @brief Placeholder for dynamic updates. */
	void
	update();

	/**
	 * @brief Binds this state to the Rasterizer (RS) stage of the pipeline.
	 * @param deviceContext The D3D11 device context.
	 */
	void
	render(DeviceContext& deviceContext);

	/** @brief Releases the underlying COM object. */
	void
	destroy();

private:
	/** @brief The underlying D3D11 Rasterizer State interface. */
	ID3D11RasterizerState* m_rasterizerState = nullptr;
};