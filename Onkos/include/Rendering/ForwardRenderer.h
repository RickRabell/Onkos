#pragma once
#include "Prerequisites.h"
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

// Forward Declarations
class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @class ForwardRenderer
 * @brief The core engine responsible for executing the rendering pipeline.
 * @author Ricardo Rabell
 * @date 2026-04-25
 *
 * @details
 * **Context:** A "Forward" renderer processes each object one by one, calculating its
 * geometry and lighting simultaneously (as opposed to Deferred rendering, which separates them).
 * * **Why it exists:** If every object tried to draw itself whenever it updated, the GPU
 * would thrash changing states constantly, and transparency would break. The Renderer
 * solves this by collecting all objects into "Queues", sorting them, and drawing them
 * in highly optimized "Passes".
 * * **Analogy:** Think of the Renderer as a master painter. First, it paints all the
 * shadows (Shadow Pass). Then, it paints all solid objects (Opaque Pass) from front-to-back
 * so it doesn't waste paint on things hidden behind walls. Finally, it paints glass and
 * water (Transparent Pass) from back-to-front so the colors blend correctly.
 */
class 
ForwardRenderer {
public:
	/**
	 * @brief Initializes the renderer, allocating constant buffers and default states.
	 * @param device The D3D11 graphics device.
	 * @return HRESULT S_OK on success.
	 */
	HRESULT
	init(Device& device);

	/**
	 * @brief Resizes render targets when the viewport/window changes.
	 * @param device The D3D11 device.
	 * @param width New width.
	 * @param height New height.
	 */
	void
	resize(Device& device, unsigned int width, unsigned int height);

	/**
	 * @brief Prepares data for the current frame before any drawing happens.
	 * @note Updates the "PerFrame" constant buffers (like View/Projection matrices and Light data).
	 * @param camera The active camera.
	 * @param scene The container of all renderable objects.
	 * @param deviceContext The D3D11 device context.
	 */
	void
	updatePerFrame(const Camera& camera, 
								 const RenderScene& scene, 
								 DeviceContext& deviceContext);

	/**
	 * @brief Executes the full rendering pipeline for a single frame.
	 * @note This is the master function that calls buildQueues, renderShadowPass,
	 * renderOpaquePass, etc.
	 * @param deviceContext The D3D11 device context.
	 * @param camera The active camera.
	 * @param scene The scene containing objects to render.
	 * @param viewportPass Utility for editor rendering.
	 */
	void
	render(DeviceContext& deviceContext,
				 const Camera& camera,
				 RenderScene& scene,
				 EditorViewportPass& viewportPass);

	/** @brief Cleans up all renderer resources. */
	void
	destroy();

	/** @brief Gets the generated shadow map texture to bind it to materials. */
	ID3D11ShaderResourceView* 
	getShadowMapSRV() const { return m_shadowDepthSRV.m_textureFromImg; }

	/** @brief Gets a debug view of the shadow pass for the editor. */
	ID3D11ShaderResourceView* 
	getPreShadowSRV() const { return m_preShadowDebugPass.getSRV(); }

private:
	/**
	 * @brief Sorts all scene objects into Opaque and Transparent queues.
	 * @note Opaque objects are usually sorted front-to-back (to optimize early-Z rejection).
	 * Transparent objects MUST be sorted back-to-front for alpha blending to work correctly.
	 */
	void 
	buildQueues(RenderScene& scene, const Camera& camera);

	/** @brief Renders a debug pass to visualize the shadow map in the editor. */
	void 
	renderPreShadowDebugPass(DeviceContext& deviceContext, RenderScene& scene);

	/**
	 * @brief Renders the scene from the perspective of the Light to generate the Shadow Map.
	 * @note Only outputs depth, no colors.
	 */
	void 
	renderShadowPass(DeviceContext& deviceContext);

	/** @brief Renders all solid objects. */
	void 
	renderOpaquePass(DeviceContext& deviceContext);

	/** @brief Renders all objects with alpha blending enabled. */
	void 
	renderTransparentPass(DeviceContext& deviceContext);

	/** @brief Renders the background environment cube. */
	void 
	renderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);

	/** @brief Binds material parameters and issues the draw call for a single object. */
	void 
	renderObject(DeviceContext& deviceContext, 
							 const RenderObject& object, 
							 RenderPassType passType);

	/** @brief A stripped-down render call for shadows (no textures/colors needed). */
	void 
	renderShadowObject(DeviceContext& deviceContext, const RenderObject& object);

	/** @brief Allocates the high-resolution depth buffer for shadow mapping. */
	HRESULT 
	createShadowResources(Device& device);

	/** @brief Calculates the View and Projection matrices for the directional light. */
	void 
	updateLightMatrices(const Camera& camera, const RenderScene& scene);

	/** @brief Initializes the standard D3D11 Blend States (Opaque, Alpha, Additive). */
	HRESULT 
	createBlendStates(Device& device);

	/** @brief Returns the correct D3D11BlendState based on the Material's settings. */
	ID3D11BlendState* 
	resolveBlendState(const Material* material) const;

private:
	// --- Constant Buffers to feed the Shaders ---

	/** @brief Constant buffer for per-frame shader data (e.g., camera, lights). */
	Buffer m_perFrameBuffer;
	/** @brief Constant buffer for per-object shader data (e.g., world matrix). */
	Buffer m_perObjectBuffer;
	/** @brief Constant buffer for per-material shader data (e.g., material properties). */
	Buffer m_perMaterialBuffer;

	// --- Pipeline States ---

	/** @brief Depth stencil state used for transparent rendering. */
	DepthStencilState m_transparentDepthStencil;
	/** @brief Blend state for alpha blending. */
	ID3D11BlendState* m_alphaBlendState = nullptr;
	/** @brief Blend state for opaque rendering. */
	ID3D11BlendState* m_opaqueBlendState = nullptr;
	/** @brief Blend state for additive blending. */
	ID3D11BlendState* m_additiveBlendState = nullptr;
	/** @brief Blend state for premultiplied alpha blending. */
	ID3D11BlendState* m_premultipliedBlendState = nullptr;
	/** @brief Blend factors for D3D11 blend state. */
	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// --- Shadow Mapping Specific Resources ---

	/** @brief Depth texture used for shadow mapping. */
	Texture m_shadowDepthTexture;
	/** @brief Shader resource view for the shadow depth texture. */
	Texture m_shadowDepthSRV;
	/** @brief Depth stencil view for shadow rendering. */
	DepthStencilView m_shadowDSV;
	/** @brief Shader program used for shadow pass. */
	ShaderProgram m_shadowShader;
	/** @brief Rasterizer state for shadow rendering. */
	RasterizerState m_shadowRasterizer;
	/** @brief Resolution of the shadow map texture. */
	unsigned int m_shadowMapSize = 2048;
	/** @brief Debug pass for visualizing the shadow map in the editor. */
	EditorViewportPass m_preShadowDebugPass;
	/** @brief Enables or disables shadow application. */
	bool m_applyShadows = true;


	// --- CPU-side data structures for the Constant Buffers updates ---
	/** @brief CPU-side structure for per-frame constant buffer data. */
	CBPerFrame m_cbPerFrame{};
	/** @brief CPU-side structure for per-object constant buffer data. */
	CBPerObject m_cbPerObject{};
	/** @brief CPU-side structure for per-material constant buffer data. */
	CBPerMaterial m_cbPerMaterial{};

	// --- Render Queues ---

	/** @brief Queue of opaque objects to render (sorted front-to-back). */
	std::vector<const RenderObject*> m_opaqueQueue;
	/** @brief Queue of transparent objects to render (sorted back-to-front). */
	std::vector<const RenderObject*> m_transparentQueue;
};