#pragma once
#include "Prerequisites.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Buffer.h"
#include "SamplerState.h"
#include "Model3D.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "EngineUtilities\Utilities\Camera.h"
#include "ECS\Actor.h"

// Forward Declarations
class Device;
class DeviceContext;

/**
 * @class Skybox
 * @brief Handles the rendering of the distant background environment.
 * @author Ricardo Rabell
 * @date 2026-03-02
 *
 * @details
 * **Context:** A Skybox simulates a vast, distant world by placing the camera inside
 * a giant cube. The inside faces of this cube are mapped with a special texture
 * (a TextureCube) representing the horizon, sky, and ground.
 * * **Why it exists:** Rendering an actual planet-sized environment is impossible.
 * By rendering a box that follows the camera's position (but not its rotation),
 * and forcing it to render "behind" everything else, we create a perfect optical
 * illusion of infinite distance.
 * * **Analogy:** It's exactly like the painted backdrop walls on a movie set. The actors
 * move around in the foreground, but the sky painted on the wall behind them never
 * gets any closer, no matter how far the actors walk.
 */
class 
Skybox {
public:
	/** @brief Default constructor. */
	Skybox() = default;

	/** @brief Default destructor. */
	~Skybox() = default;

	/**
	 * @brief Initializes the skybox resources, shaders, and pipeline states.
	 * @param device The D3D11 Device.
	 * @param deviceContext The D3D11 Device Context.
	 * @param cubemap The loaded TextureCube containing the 6 sky images.
	 * @return HRESULT S_OK if all resources are created successfully.
	 */
	HRESULT
	init(Device& device, DeviceContext* deviceContext, Texture& cubemap);

	/**
	 * @brief Updates the skybox's constant buffer with the camera's view-projection matrix.
	 * @note The View matrix used here should have its translation stripped out
	 * (using Camera::GetViewNoTranslation) so the skybox stays infinitely far away.
	 * @param deviceContext The D3D11 Device Context.
	 * @param camera The active scene camera.
	 */
	void
	update(DeviceContext& deviceContext, Camera& camera);

	/**
	 * @brief Binds the specific skybox pipeline states and issues the draw call.
	 * @note The Skybox sets a specific DepthStencilState (to allow drawing at max depth)
	 * and RasterizerState (to disable backface culling, as we are *inside* the cube).
	 * @param deviceContext The D3D11 Device Context.
	 */
	void
	render(DeviceContext& deviceContext);

	/** @brief Cleans up skybox resources. */
	void
	destroy() {}

private:
	/** @brief Specialized shader to handle TextureCube sampling and depth-tricks. */
	ShaderProgram m_shaderProgram;
	/** @brief Buffer holding the View-Projection matrix for the sky. */
	Buffer m_constantBuffer;
	/** @brief Sampler for the cubemap (usually linear filtering). */
	SamplerState m_samplerState;
	/** @brief State to turn off culling so we can see the inside of the cube. */
	RasterizerState m_rasterizerState;
	/** @brief State configured to LESS_EQUAL to allow the skybox depth trick to pass. */
	DepthStencilState m_depthStencilState;
	/** @brief The 6-sided cubemap texture. */
	Texture m_skyboxTexture;
	/** @brief The raw cube geometry. */
	Model3D* m_cubeModel = nullptr;
	/** @brief Actor representation of the skybox in the ECS (optional depending on engine design). */
	EU::TSharedPointer<Actor> m_skybox;
};