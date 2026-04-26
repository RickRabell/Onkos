#pragma once
#include "Prerequisites.h"
#include "Texture.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"

// Forward declarations
class Device;
class DeviceContext;

/**
 * @class EditorViewportPass
 * @brief Encapsulates a Render-to-Texture (RTT) setup for displaying the game within an editor UI.
 * @author Ricardo Rabell
 * @date 2026-04-25
 *
 * @details
 * **Context:** When building an engine Editor (like Unity or Unreal), you don't want the
 * 3D game to take up the entire Windows window. You want panels, hierarchies, and inspectors
 * wrapping the game.
 * * **Why it exists:** Instead of rendering to the main screen's `SwapChain` backbuffer,
 * this class acts as an off-screen "canvas" (an FBO/Render Target). The `ForwardRenderer`
 * draws the entire 3D world onto this class's `m_colorTexture`. Later, the ImGui system
 * takes that texture and draws it as an image inside an ImGui Window widget.
 * * **Analogy:** Instead of broadcasting a live play directly to the television (the SwapChain),
 * the camera is recording the play onto a videotape (the Texture). Later, the Editor
 * plays that videotape on a small monitor on the director's desk.
 */
class 
EditorViewportPass {
public:
	/** @brief Default constructor. */
	EditorViewportPass() = default;

	/** @brief Default destructor. */
	~EditorViewportPass() = default;

	/**
	 * @brief Creates the off-screen Color and Depth textures.
	 * @param device The D3D11 device.
	 * @param width Initial width of the viewport window.
	 * @param height Initial height of the viewport window.
	 * @return HRESULT S_OK on success.
	 */
	HRESULT
	init(Device& device, unsigned int width, unsigned int height);

	/**
	 * @brief Destroys and recreates the textures to match a new window size.
	 * @note Required when the user resizes the ImGui viewport panel to prevent stretching.
	 */
	HRESULT
	resize(Device& device, unsigned int width, unsigned int height);

	/**
	 * @brief Binds this pass as the active Render Target, directing all subsequent D3D11
	 * draw calls to this texture instead of the screen.
	 * @param deviceContext The D3D11 device context.
	 * @param clearColor A 4-float array representing the RGBA background color.
	 */
	void
	begin(DeviceContext& deviceContext, const float clearColor[4]);

	/**
	 * @brief Exchanges the internal resources of two EditorViewportPasses.
	 * @note Useful for post-processing effects (Ping-Ponging between two render targets).
	 */
	void
	swap(EditorViewportPass& other);

	/**
	 * @brief Clears only the Depth Buffer, leaving the Color Buffer intact.
	 */
	void
	clearDepth(DeviceContext& deviceContext);

	/**
	 * @brief Updates the D3D11 Rasterizer viewport to match this texture's dimensions.
	 */
	void
	setViewport(DeviceContext& deviceContext);

	/** @brief Releases all allocated GPU resources. */
	void
	destroy();

	/**
	 * @brief Retrieves the Shader Resource View so ImGui can display the rendered frame.
	 * @return ID3D11ShaderResourceView* The compiled image.
	 */
	ID3D11ShaderResourceView* 
	getSRV() const { return m_colorSRV.m_textureFromImg; }

	/**
	 * @brief Gets the current width of the viewport texture.
	 * @return unsigned int The width in pixels.
	 */
	unsigned int 
	getWidth() const { return m_width; }

	/**
	 * @brief Gets the current height of the viewport texture.
	 * @return unsigned int The height in pixels.
	 */
	unsigned int 
	getHeight() const { return m_height; }

	/**
	 * @brief Verifies that all required textures and views were allocated successfully.
	 * @return bool True if the pass is ready to receive draw calls.
	 */
	bool
	isValid() const {
		return m_colorTexture.m_texture != nullptr &&
					 m_colorSRV.m_textureFromImg != nullptr &&
					 m_depthTexture.m_texture != nullptr;
	}

private:
	/** @brief Internal helper to allocate the specific D3D11 texture formats. */
	HRESULT
	createResources(Device& device, unsigned int width, unsigned int height);

private:
	/**
	 * @brief The 2D surface where pixels are drawn.
	 * @details This texture acts as the render target for the editor viewport, capturing the rendered frame.
	 */
	Texture m_colorTexture;

	/**
	 * @brief The read-only view used to pass the surface to ImGui.
	 * @details This texture provides a Shader Resource View (SRV) for displaying the rendered image inside ImGui panels.
	 */
	Texture m_colorSRV;

	/**
	 * @brief The write-only view used by the Output Merger to draw pixels.
	 * @details This Render Target View (RTV) binds the color texture for rendering operations.
	 */
	RenderTargetView m_rtv;

	/**
	 * @brief The 2D surface storing Z-buffer depth values.
	 * @details This texture is used for depth testing during rendering to the editor viewport.
	 */
	Texture m_depthTexture;

	/**
	 * @brief The write/read view used for depth testing.
	 * @details This Depth Stencil View (DSV) binds the depth texture for depth and stencil operations.
	 */
	DepthStencilView m_dsv;

	/**
	 * @brief Current width in pixels.
	 * @details Represents the width of the viewport texture, updated on resize.
	 */
	unsigned int m_width = 1;

	/**
	 * @brief Current height in pixels.
	 * @details Represents the height of the viewport texture, updated on resize.
	 */
	unsigned int m_height = 1;
};