#pragma once
#include "Rendering/ForwardRenderer.h"
#include "Rendering/DeferredRenderer.h"

/**
 * @class RenderPipeline
 * @brief High-level orchestrator that manages and switches between different rendering paths.
 * @author Ricardo Rabell
 * @date 2026-07-13
 *
 * @details
 * **Context:** Instead of forcing the main engine (`BaseApp`) to talk directly to a specific
 * renderer, the engine talks only to the `RenderPipeline`. This class implements a variation
 * of the **Strategy and Facade patterns**, encapsulating both `ForwardRenderer` and `DeferredRenderer`
 * behind a unified, simple interface.
 *
 * * **Why it exists:** Different game scenarios require different rendering techniques. A dark,
 * indoor scene with hundreds of dynamic lights thrives under a **Deferred Renderer**. However, a
 * bright, outdoor landscape with lots of transparent water and foliage might run better or look
 * crisper under a **Forward Renderer**. This pipeline allows the engine to swap the entire rendering
 * architecture instantly at runtime (e.g., via an options menu or editor checkbox) without breaking
 * the game loop.
 *
 * * **Lazy Initialization Pattern:** To prevent massive loading times and GPU memory waste at startup,
 * this pipeline uses lazy initialization. It only builds the complex resources (like G-Buffers or
 * massive depth targets) for a renderer *the first time* that specific path is requested.
 *
 * * **Analogy:** The `RenderPipeline` is like a universal video adapter unit. Your console (the engine)
 * sends the gameplay data down the cable. Inside the adapter, you can flip a switch to process the
 * data using a vintage filter (Forward) or a high-tech cinematic matrix (Deferred). The console doesn't
 * care *how* the pixels are cooked; it just hands over the ingredients and expects an image back.
 */
class 
RenderPipeline {
public:
	/** @brief Default constructor. */
	RenderPipeline() = default;

	/** @brief Default destructor. Calls destroy() internally to clean up GPU states. */
	~RenderPipeline() = default;

	/**
	 * @brief Initializes the pipeline wrapper and boots up the default renderer type.
	 * @param device The D3D11 graphics device.
	 * @param initialRenderer The rendering path to start with (defaults to Deferred).
	 * @return HRESULT S_OK if the default renderer initialized successfully.
	 */
	HRESULT 
	init(Device& device, RendererType initialRenderer = RendererType::Deferred);

	/**
	 * @brief Switches the active rendering path at runtime.
	 * @note If the requested renderer has never been used before, this method will automatically
	 * trigger its full resource allocation (`ensureRendererInitialized`).
	 * @param rendererType The new desired backend (Forward or Deferred).
	 * @param device The D3D11 graphics device needed for potential allocation.
	 * @return HRESULT S_OK if the swap was safe and successful.
	 */
	HRESULT 
	setRendererType(RendererType rendererType, Device& device);

	/**
	 * @brief Propagates window resize events down to both renderers.
	 * @details Ensures that if the screen changes resolution, G-Buffers and shadow maps
	 * scale accordingly to prevent sampling artifacts or sampling out-of-bounds memory.
	 * @param device The D3D11 graphics device.
	 * @param width New width in pixels.
	 * @param height New height in pixels.
	 */
	void 
	resize(Device& device, unsigned int width, unsigned int height);

	/**
	 * @brief Redirects the draw call request to the currently active renderer strategy.
	 * @param deviceContext The D3D11 graphics device context.
	 * @param camera The active viewing camera containing view/projection matrices.
	 * @param scene The package of all visual objects gathered for this frame.
	 * @param viewportPass The off-screen render target representing the editor viewport canvas.
	 */
	void 
	render(DeviceContext& deviceContext,
				 const Camera& camera,
				 RenderScene& scene,
				 EditorViewportPass& viewportPass);

	/**
	 * @brief Releases GPU memory allocations for both Forward and Deferred rendering paths.
	 */
	void 
	destroy();

	/** @brief Retrieves the enum identifier of the active renderer. */
	RendererType 
	getRendererType() const { return m_activeRendererType; }

	/** @brief Returns a human-readable string name of the active backend (e.g., "DeferredRenderer"). */
	const char* 
	getActiveRendererName() const;

	//--------------------------------------------------------------------------------------
	// Unified Resource Getters (Facade Interface)
	// Context: The editor UI calls these methods to draw debug textures. If the active 
	// renderer is Forward, G-Buffer getters will safely return nullptr since Forward 
	// doesn't utilize a Geometry Buffer.
	//--------------------------------------------------------------------------------------

	ID3D11ShaderResourceView* getShadowMapSRV() const;
	ID3D11ShaderResourceView* getPreShadowSRV() const;
	ID3D11ShaderResourceView* getGBufferAlbedoMetallicSRV() const;
	ID3D11ShaderResourceView* getGBufferNormalRoughnessSRV() const;
	ID3D11ShaderResourceView* getGBufferWorldAoSRV() const;
	ID3D11ShaderResourceView* getGBufferEmissiveAlphaSRV() const;

	/** @brief Directs the active renderer to overlay shadow debug weights onto the screen. */
	void 
	setShadowFactorDebugEnabled(bool enabled);

	/** @brief Directs the active renderer to display isolated G-Buffer channels (Normals, Albedo, etc.). */
	void 
	setDeferredDebugViewMode(int mode);

private:
	/**
	 * @brief Internal helper managing the Lazy Initialization check.
	 * * **Reverse Engineering Key:** If `rendererType` is Forward and `m_forwardInitialized` is false,
	 * this function runs `m_forwardRenderer.init(device)` and marks the flag as true. This keeps
	 * the application's memory footprint light until the features are actually requested.
	 * * @return HRESULT S_OK if the renderer was already ready or successfully set up right now.
	 */
	HRESULT 
	ensureRendererInitialized(RendererType rendererType, Device& device);

	/**
	 * @brief Helper to map the `RendererType` enum to the internal concrete object pointer.
	 * @param rendererType Target enum.
	 * @return ISceneRenderer* Base interface pointer pointing to either m_forwardRenderer or m_deferredRenderer.
	 */
	ISceneRenderer* 
	resolveRenderer(RendererType rendererType);

	/** @brief Constant version of the pointer resolver. */
	const ISceneRenderer* 
	resolveRenderer(RendererType rendererType) const;

private:
	/** @brief The dedicated Forward Pass allocation. */
	ForwardRenderer m_forwardRenderer;

	/** @brief The dedicated Deferred Pass allocation containing G-Buffer memory. */
	DeferredRenderer m_deferredRenderer;

	/** @brief Polymorphic pointer acting as the active strategy handle. */
	ISceneRenderer* m_activeRenderer = nullptr;

	/** @brief Enum tracking which strategy is active. */
	RendererType m_activeRendererType = RendererType::Deferred;

	/** @brief Lazy initialization tracking flag for the Forward path. */
	bool m_forwardInitialized = false;

	/** @brief Lazy initialization tracking flag for the Deferred path. */
	bool m_deferredInitialized = false;
};