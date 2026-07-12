#pragma once
#include "Prerequisites.h"

// Forward Declarations
class Device;
class DeviceContext;
class Camera;
class RenderScene;
class EditorViewportPass;

/**
 * @enum RenderType
 * @brief Enumerates high-level rendering strategies supported by the renderer.
 *
 * This enum selects the overall rendering path an implementation of
 * ISceneRenderer will use to produce the final image.
 */
enum class
RendererType {
	/** Forward rendering path (simple, single-pass rasterization). */
	Forward = 0,
	/** Deferred rendering path (uses multiple G-buffers and a lighting pass). */
	Deferred
};

/**
 * @class ISceneRenderer
 * @brief Abstract interface for scene renderers.
 * @author Ricardo Rabell
 * @date 2026-05-13
 *
 * ISceneRenderer defines the lifecycle and capabilities required from any
 * renderer implementation used by the engine's editor and runtime viewports.
 * Implementations are responsible for creating and releasing GPU resources,
 * handling resize events, executing the rendering for a frame, and exposing
 * shader resource views for debugging or composition.
 *
 * Typical usage:
 * - Call init(...) once to allocate resources.
 * - Call resize(...) when the output surface changes size.
 * - Call render(...) each frame to draw the scene.
 * - Call destroy() to free all resources when the renderer is no longer needed.
 *
 * @see RenderType
 */
class
ISceneRenderer {
public:
	virtual ~ISceneRenderer() = default;
	
	/**
	 * @brief Initialize renderer resources.
	 * @param device Reference to the device used to create GPU resources.
	 * @return HRESULT indicating success or failure of initialization.
	 *
	 * Implementations should create all required GPU resources and shaders.
	 * This method may be called once during the renderer's lifetime.
	 */
	virtual HRESULT
	init(Device& device) = 0;
	
	/**
	 * @brief Notify the renderer about a change in output size.
	 * @param device Reference to the device for resource (re)allocation.
	 * @param width New width of the output in pixels.
	 * @param height New height of the output in pixels.
	 *
	 * Implementations should resize render targets, viewports and any
	 * dependent resources to match the provided dimensions.
	 */
	virtual void
	resize(Device& device, unsigned int width, unsigned int height) = 0;

	/**
	 * @brief Render the scene for the current frame.
	 * @param deviceContext Device context used to record draw calls and set state.
	 * @param camera Camera describing the view and projection for the frame.
	 * @param scene Scene containing renderable objects and lighting information.
	 * @param viewportPass Editor viewport pass that may provide UI overlays or
	 *                     editor-specific compositing passes.
	 *
	 * This method performs all rendering required to produce the final image
	 * for the given camera and scene. Implementations must not present the
	 * swap chain unless explicitly required by the caller.
	 */
	virtual void
	render(DeviceContext& deviceContext,
				 const Camera& camera,
				 RenderScene& scene,
				 EditorViewportPass& viewportPass) = 0;

	/**
	 * @brief Destroy and release all renderer resources.
	 *
	 * Implementations should release any GPU resources, shader objects and
	 * internal allocations. After calling destroy(), the renderer may be
	 * re-initialized via init(...) if required.
	 */
	virtual void
	destroy() = 0;

	/**
	 * @brief Get the shader resource view for the shadow map.
	 * @return Pointer to an ID3D11ShaderResourceView containing the shadow map
	 *         or nullptr if the renderer does not provide one.
	 *
	 * @note Default implementation returns nullptr.
	 */
	virtual ID3D11ShaderResourceView*
	getShadowMapSRV() const { return nullptr; }

	/**
	 * @brief Get the shader resource view for the pre-shadow (temporary) map.
	 * @return Pointer to an ID3D11ShaderResourceView or nullptr if unsupported.
	 *
	 * @note Default implementation returns nullptr.
	 */
	virtual ID3D11ShaderResourceView*
	getPreShadowSRV() const { return nullptr; }

	/**
	 * @brief Get the G-buffer shader resource view containing albedo and metallic.
	 * @return Pointer to an ID3D11ShaderResourceView or nullptr if unsupported.
	 *
	 * @note Used by deferred renderers to expose intermediate buffers for
	 *       debugging or post-processing.
	 */
	virtual ID3D11ShaderResourceView*
	getGBufferAlbedoMetallicSRV() const { return nullptr; }

	/**
	 * @brief Get the G-buffer shader resource view containing normal and roughness.
	 * @return Pointer to an ID3D11ShaderResourceView or nullptr if unsupported.
	 */
	virtual ID3D11ShaderResourceView*
	getGBufferNormalRoughnessSRV() const { return nullptr; }

	/**
	 * @brief Get the G-buffer shader resource view containing world-space data and AO.
	 * @return Pointer to an ID3D11ShaderResourceView or nullptr if unsupported.
	 */
	virtual ID3D11ShaderResourceView*
	getGBufferWorldAoSRV() const { return nullptr; }

	/**
	 * @brief Get the G-buffer shader resource view containing emissive and alpha.
	 * @return Pointer to an ID3D11ShaderResourceView or nullptr if unsupported.
	 */
	virtual ID3D11ShaderResourceView*
	getGBufferEmissiveAlphaSRV() const { return nullptr; }

	/**
	 * @brief Enable or disable debug visualization for shadow factors.
	 * @param enabled True to enable debug visualization, false to disable.
	 *
	 * @note Default implementation is a no-op.
	 */
	virtual void
	setShadowFactorDebugEnabled(bool enabled) { (void)enabled; }

	/**
	 * @brief Set the debug view mode for deferred rendering debug outputs.
	 * @param mode Integer identifying the debug visualization mode.
	 *
	 * @note Interpretation of @p mode is renderer-specific. Default is no-op.
	 */
	virtual void 
	setDeferredDebugViewMode(int mode) { (void)mode; }

	/**
	 * @brief Get a short, null-terminated string identifying the renderer.
	 * @return C-string with the renderer's debug name.
	 *
	 * Implementations should return a static string literal identifying the
	 * concrete renderer (for logging/UI purposes). Must not return nullptr.
	 */
	virtual const char*
	getDebugName() const = 0;
};