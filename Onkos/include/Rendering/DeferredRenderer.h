#pragma once
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "ISceneRenderer.h"
#include "RenderScene.h"
#include "RenderTypes.h"
#include "SamplerState.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

// Forward Declarations
class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @class DeferredRenderer
 * @brief Implements a Deferred Shading rendering pipeline.
 * @author Ricardo Rabell
 * @date 2026-06-03
 *
 * @details
 * **Context:** In a traditional Forward Renderer, every time a triangle is drawn on the screen,
 * the GPU calculates the lighting for every pixel of that triangle. If there are $G$ geometry pixels
 * and $L$ lights, the computational complexity is $O(G \times L)$. This leads to massive performance
 * waste due to "overdraw" (calculating lighting for a wall that ends up hidden behind a closer object).
 * * **Why it exists:** A Deferred Renderer splits the process into two major steps to change the
 * complexity to $O(G + L)$:
 * 1. **Geometry Pass:** It renders all opaque objects without calculating ANY lighting. Instead,
 * it saves the raw "ingredients" of the surface (Color, Normals, Metallic, Roughness, World Position)
 * into a series of textures called the **G-Buffer** (Geometry Buffer).
 * 2. **Lighting Pass:** It draws a single flat rectangle (a Full-Screen Quad) over the screen. For
 * every pixel on that screen, it reads the ingredients from the G-Buffer and calculates the final
 * lighting math exactly once.
 * * **Analogy:** Forward Rendering is like cooking a full meal (geometry + lighting) for every guest
 * that walks in, only to throw half the meals in the trash because guests shared tables (overdraw).
 * Deferred Rendering is like prepping all the raw ingredients in different bowls (the G-Buffer),
 * waiting until everyone sits down, and then cooking exactly the right amount of food in one go
 * (the Lighting Pass).
 * * **The Transparency Catch:** Deferred rendering cannot handle transparent objects like glass, because
 * a single pixel in the G-Buffer can only hold one value (you can't store the glass AND the wall behind it).
 * Therefore, transparent objects must be drawn using traditional Forward Rendering *after* the lighting pass.
 */
class 
DeferredRenderer : public ISceneRenderer {
public:
	/**
	 * @brief Initializes the deferred pipeline, allocating G-Buffer textures, shaders, and the full-screen quad.
	 * @param device The D3D11 graphics device.
	 * @return HRESULT S_OK on success.
	 */
	HRESULT 
	init(Device& device) override;

	/**
	 * @brief Re-allocates the G-Buffer textures when the viewport changes size.
	 * @note G-Buffer textures must exactly match the pixel dimensions of the screen.
	 */
	void 
	resize(Device& device, unsigned int width, unsigned int height) override;

	/**
	 * @brief Executes the full Deferred Rendering pipeline for a single frame.
	 * @note Order of operations: buildQueues -> renderShadowPass -> renderGeometryPass ->
	 * renderLightingPass -> renderSkyboxPass -> renderTransparentPass.
	 */
	void 
	render(DeviceContext& deviceContext,
		const Camera& camera,
		RenderScene& scene,
		EditorViewportPass& viewportPass) override;

	/** @brief Cleans up all renderer resources and G-Buffer targets. */
	void 
	destroy() override;

	//--------------------------------------------------------------------------------------
	// Debug & Resource Getters
	// Context: Exposing these SRVs allows the engine editor to display the raw G-Buffer 
	// contents to the developer. This is critical for reverse-engineering visual bugs 
	// (e.g., checking if normals are facing the wrong way).
	//--------------------------------------------------------------------------------------

	ID3D11ShaderResourceView* 
	getShadowMapSRV() const override { 
		return m_shadowDepthSRV.m_textureFromImg; 
	}
	
	ID3D11ShaderResourceView* 
	getPreShadowSRV() const override { 
		return m_preShadowDebugPass.getSRV(); 
	}

	ID3D11ShaderResourceView* 
	getGBufferAlbedoMetallicSRV() const override { 
		return m_gBufferAlbedoMetallicSRV.m_textureFromImg; 
	}

	ID3D11ShaderResourceView* 
	getGBufferNormalRoughnessSRV() const override { 
		return m_gBufferNormalRoughnessSRV.m_textureFromImg; 
	}
	
	ID3D11ShaderResourceView* 
	getGBufferWorldAoSRV() const override { 
		return m_gBufferWorldAoSRV.m_textureFromImg; 
	}
	
	ID3D11ShaderResourceView* 
	getGBufferEmissiveAlphaSRV() const override { 
		return m_gBufferEmissiveAlphaSRV.m_textureFromImg; 
	}

	/** @brief Toggles rendering of the shadow map directly to the screen for debugging. */
	void 
	setShadowFactorDebugEnabled(bool enabled) override {
		m_shadowFactorDebugEnabled = enabled; 
	}

	/**
	 * @brief Switches the output of the Lighting Pass to show raw G-Buffer data.
	 * @param mode An integer mapping to different G-Buffer outputs (e.g., 0=Final, 1=Normals, 2=Albedo).
	 */
	void 
	setDeferredDebugViewMode(int mode) override { m_deferredDebugViewMode = mode; }

	const char* 
	getDebugName() const override { return "DeferredRenderer"; }

private:
	/** @brief Sorts scene objects into opaque (Deferred) and transparent (Forward) queues. */
	void 
	buildQueues(RenderScene& scene, const Camera& camera);

	/** @brief Updates the Constant Buffers with per-frame data (Camera view, light positions). */
	void 
	updatePerFrame(const Camera& camera, 
								 const RenderScene& scene, 
								 DeviceContext& deviceContext);
	
	/**
	 * @brief Updates matrices or other derived data related to lights used for shadow mapping and culling.
	 * @param camera The main camera used for view-dependent calculations.
	 * @param scene The scene containing lights and their properties.
	 */
	void 
	updateLightMatrices(const Camera& camera, const RenderScene& scene);

	/** @brief Main execution wrapper that orchestrates the sub-passes. */
	void 
	renderSceneToTarget(DeviceContext& deviceContext, 
											RenderScene& scene, 
											EditorViewportPass& targetPass, 
											bool applyShadows);

	/**
	 * @brief Binds multiple Render Targets (MRTs) simultaneously.
	 * @note This is the magic of the Geometry Pass. A single Pixel Shader outputs to 4 different
	 * textures at once (Albedo, Normals, Position, Emissive).
	 */
	void 
	bindGBufferTargets(DeviceContext& deviceContext, 
										 ID3D11DepthStencilView* depthStencilView);

	/** @brief Unbinds the G-Buffer and binds the final screen texture for the Lighting Pass. */
	void 
	bindFinalTarget(DeviceContext& deviceContext, 
									ID3D11RenderTargetView* renderTargetView, 
									ID3D11DepthStencilView* depthStencilView);

	/** @brief Unbinds Shader Resource Views to prevent D3D11 read/write hazard warnings. */
	void 
	clearDeferredSRVs(DeviceContext& deviceContext);

	/**
	 * @brief PASS 1: Renders all opaque objects into the G-Buffer textures.
	 * @note No lighting is calculated here.
	 */
	void 
	renderGeometryPass(DeviceContext& deviceContext);
	
	/**
	 * @brief Renders a single object into the G-Buffer.
	 * @param deviceContext The DeviceContext used to set states and issue draw calls.
	 * @param object The RenderObject instance containing mesh, material and transform.
	 */
	void 
	renderGeometryObject(DeviceContext& deviceContext, const RenderObject& object);

	/**
	 * @brief PASS 2: Renders a full-screen quad, reading the G-Buffer and computing PBR lighting.
	 */
	void 
	renderLightingPass(DeviceContext& deviceContext);

	/** @brief PASS 3: Renders the background environment. Must happen after lighting to respect depth. */
	void 
	renderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);

	/** @brief PASS 4: Forward rendering for glass, water, and particles. */
	void 
	renderTransparentPass(DeviceContext& deviceContext);
	
	/**
	 * @brief Renders a single object using the forward (transparent) pipeline.
	 * @param deviceContext The DeviceContext used to set states and issue draw calls.
	 * @param object The RenderObject to render.
	 * @param passType The specific forward pass type (e.g., translucent, additive).
	 */
	void 
	renderForwardObject(DeviceContext& deviceContext, 
											const RenderObject& object, 
											RenderPassType passType);

	/** @brief Generates the depth map used for shadowing. */
	void 
	renderShadowPass(DeviceContext& deviceContext);
	
	/**
	 * @brief Renders a single object into the shadow map depth buffer.
	 * @param deviceContext The DeviceContext used to set depth-only states and draw.
	 * @param object The RenderObject to render into the shadow map.
	 */
	void 
	renderShadowObject(DeviceContext& deviceContext, const RenderObject& object);

	//--------------------------------------------------------------------------------------
	// Initialization Helpers
	//--------------------------------------------------------------------------------------
	/**
	 * @brief Creates textures, SRVs, and depth-stencil views required for shadow mapping.
	 * @param device Reference to the D3D11 device used to create resources.
	 * @return HRESULT S_OK on success, otherwise a failing HRESULT.
	 */
	HRESULT 
	createShadowResources(Device& device);
	
	/**
	 * @brief Allocates all G-Buffer textures and their views for the given viewport size.
	 * @param device Reference to the D3D11 device used to create resources.
	 * @param width Desired texture width in pixels.
	 * @param height Desired texture height in pixels.
	 * @return HRESULT S_OK on success, otherwise a failing HRESULT.
	 */
	HRESULT 
	createGBufferResources(Device& device, unsigned int width, unsigned int height);
	
	/**
	 * @brief Helper to create a single G-Buffer render target and its associated SRV.
	 * @param device Reference to the D3D11 device used to create resources.
	 * @param width Texture width in pixels.
	 * @param height Texture height in pixels.
	 * @param format DXGI_FORMAT describing the texture format.
	 * @param texture Output wrapper that will contain the created texture.
	 * @param srv Output wrapper for the created shader resource view.
	 * @param rtv Output wrapper for the created render target view.
	 * @return HRESULT S_OK on success, otherwise a failing HRESULT.
	 */
	HRESULT 
	createGBufferTarget(Device& device, 
											unsigned int width, 
											unsigned int height, 
											DXGI_FORMAT format, 
											Texture& texture, 
											Texture& srv, 
											RenderTargetView& rtv);
	
	/**
	 * @brief Creates shaders, samplers, rasterizer and any states required by the lighting pass.
	 * @param device Reference to the D3D11 device used to create resources.
	 * @return HRESULT S_OK on success, otherwise a failing HRESULT.
	 */
	HRESULT 
	createLightingResources(Device& device);

	/**
	 * @brief Generates the geometry for the Lighting Pass.
	 * @note It creates a single rectangle (two triangles) that perfectly covers the screen.
	 * This ensures the Lighting Pixel Shader runs exactly once for every pixel on the monitor.
	 */
	HRESULT 
	createFullScreenQuad(Device& device);

	/**
	 * @brief Creates commonly used blend states (alpha, opaque, additive, premultiplied).
	 * @param device Reference to the D3D11 device used to create blend states.
	 * @return HRESULT S_OK on success, otherwise a failing HRESULT.
	 */
	HRESULT 
	createBlendStates(Device& device);
	
	/**
	 * @brief Selects the appropriate D3D11 blend state based on the given material properties.
	 * @param material Pointer to the material whose blending rules should be respected. May be null.
	 * @return ID3D11BlendState* Pointer to the blend state to apply when rendering the material.
	 */
	ID3D11BlendState* 
	resolveBlendState(const Material* material) const;

private:
	// Constant Buffers
	Buffer m_perFrameBuffer;
	Buffer m_perObjectBuffer;
	Buffer m_perMaterialBuffer;
	Buffer m_lightingDebugBuffer;

	// Full Screen Quad Geometry
	Buffer m_fullScreenVertexBuffer;
	Buffer m_fullScreenIndexBuffer;

	// Depth Stencil States
	DepthStencilState m_transparentDepthStencil;
	DepthStencilState m_disabledDepthStencil; // Used during the Lighting Pass (quad doesn't need depth)
	DepthStencilState m_shadowDepthStencil;

	// Blend States
	ID3D11BlendState* m_alphaBlendState = nullptr;
	ID3D11BlendState* m_opaqueBlendState = nullptr;
	ID3D11BlendState* m_additiveBlendState = nullptr;
	ID3D11BlendState* m_premultipliedBlendState = nullptr;
	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Shadow Mapping
	Texture m_shadowDepthTexture;
	Texture m_shadowDepthSRV;
	DepthStencilView m_shadowDSV;
	ShaderProgram m_shadowShader;
	RasterizerState m_shadowRasterizer;
	unsigned int m_shadowMapSize = 2048;

	// Deferred Shaders and States
	ShaderProgram m_gBufferShader;           // Shader used during the Geometry Pass
	ShaderProgram m_deferredLightingShader;  // Shader used during the Lighting Pass
	SamplerState m_lightingSampler;
	RasterizerState m_fullScreenRasterizer;

	//--------------------------------------------------------------------------------------
	// The G-Buffer Textures
	// Context: By packing related data together (e.g., Albedo RGB + Metallic A), we 
	// minimize memory bandwidth.
	//--------------------------------------------------------------------------------------
	Texture m_gBufferAlbedoMetallicTexture;
	Texture m_gBufferAlbedoMetallicSRV;
	RenderTargetView m_gBufferAlbedoMetallicRTV;

	Texture m_gBufferNormalRoughnessTexture;
	Texture m_gBufferNormalRoughnessSRV;
	RenderTargetView m_gBufferNormalRoughnessRTV;

	Texture m_gBufferWorldAoTexture;
	Texture m_gBufferWorldAoSRV;
	RenderTargetView m_gBufferWorldAoRTV;

	Texture m_gBufferEmissiveAlphaTexture;
	Texture m_gBufferEmissiveAlphaSRV;
	RenderTargetView m_gBufferEmissiveAlphaRTV;

	// Render Config
	EditorViewportPass m_preShadowDebugPass;
	bool m_applyShadows = true;
	unsigned int m_renderWidth = 1280;
	unsigned int m_renderHeight = 720;

	// CPU Data structs mapping to GPU Constant Buffers
	CBPerFrame m_cbPerFrame{};
	CBPerObject m_cbPerObject{};
	CBPerMaterial m_cbPerMaterial{};

	/** @brief Maps to a Constant Buffer to toggle debug views inside the HLSL lighting shader. */
	struct DeferredLightingDebugData {
		int debugViewMode = 0;
		float shadowStrength = 1.0f;
		float pad0 = 0.0f;
		float pad1 = 0.0f;
	} m_lightingDebugData{};

	bool m_shadowFactorDebugEnabled = false;
	int m_deferredDebugViewMode = 0;

	// Render Queues
	std::vector<const RenderObject*> m_opaqueQueue;
	std::vector<const RenderObject*> m_transparentQueue;
};