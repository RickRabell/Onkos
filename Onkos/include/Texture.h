#pragma once
#include "Prerequisites.h"

// Forward declarations
class Device;
class DeviceContext;

/**
 * @class Texture
 * @brief A wrapper for DirectX 2D texture resources (ID3D11Texture2D).
 * @author Ricardo Rabell
 * @date 2025-09-18
 *
 * This class handles the creation, management, and binding of 2D textures.
 * It supports loading textures from image files and creating procedural textures
 * for use as render targets or depth-stencil buffers.
 */
class
Texture {
public:
	/**
	 * @brief Default constructor.
	 */
	Texture() = default;
	/**
	 * @brief Default destructor.
	 */
	~Texture() = default;

	/**
	 * @brief Initializes a texture by loading it from a file.
	 * @param device The graphics device used to create the texture resource.
	 * @param textureName The file path of the image to load.
	 * @param extensionType The file extension type (e.g., PNG, JPG) to guide the loading process.
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(	Device& deivce, const std::string& textureName, ExtensionType extensionType	);

	/**
	 * @brief Initializes a procedural texture (e.g., for a render target or depth buffer).
	 * @param device The graphics device used to create the texture resource.
	 * @param width The width of the texture in pixels.
	 * @param height The height of the texture in pixels.
	 * @param format The texture data format (e.g., DXGI_FORMAT_R8G8B8A8_UNORM).
	 * @param bindFlags Flags indicating how the texture will be bound to the pipeline (e.g., D3D11_BIND_RENDER_TARGET).
	 * @param sampleCount The number of multisamples per pixel (for MSAA). Defaults to 1.
	 * @param qualityLevels The quality level for multisampling. Defaults to 0.
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(	Device& device, 
				unsigned int width, 
				unsigned int height, 
				DXGI_FORMAT format, 
				unsigned int bindFlags, 
				unsigned int sampleCount = 1, 
				unsigned int qualityLevels = 0	);

	/**
	 * @brief Initializes a texture based on an existing texture resource.
	 * @note This is useful for creating a specific view (e.g. a depth stencil view)
	 * from an existing typeless texture resource.
	 * @param device The graphics device used for creation.
	 * @param textureRef A reference to an existing Texture object whose resource will be used.
	 * @param format The specific format for the view being created.
	 * @return HRESULT Returns S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(Device& device, Texture& textureRef, DXGI_FORMAT format);

	/**
	 * @brief Per-frame update logic for the texture.
	 * @note Can be used for animated textures or other dynamic effects.
	 * @todo Implementation details should be added.
	 */
	void
	update();

	/**
	 * @brief Binds the texture's shader resource view to a shader stage.
	 * @note This method makes the texture available for sampling in a shader (e.g., a pixel shader).
	 * @param deviceContext The device context used to issue the binding command.
	 * @param startSlot The starting shader register slot to bind the texture to.
	 * @param numViews The number of views to bind (typically 1).
	 */
	void
	render(DeviceContext& deviceContext, unsigned int startSlot, unsigned int numViews);

	/**
	 * @brief Releases the underlying D3D11 texture and shader resource view.
	 */
	void
	destroy();

public:
	/** 
		* @brief The underlying DirectX 2D texture resource. 
		*/
	ID3D11Texture2D* m_texture = nullptr;

	/** 
		* @brief The shader resource view (SRV) for this texture. The SRV is what allows shaders to read from the texture. 
		*/
	ID3D11ShaderResourceView* m_textureFromImg = nullptr;

	/** 
		* @brief The name or file path of the texture, used for identification. 
		*/
	std::string m_textureName;
};
