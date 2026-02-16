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
	init(Device& device, 
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

	/**
	* @brief Initializes a cubemap texture from six image files.
	* @param device The graphics device used to create the cubemap resource.
	* @param deviceContext The device context used for uploading texture data.
	* @param facePaths An array of file paths for the six cubemap faces (order: +X, -X, +Y, -Y, +Z, -Z).
	* @param generateMips Whether to generate mipmaps for the cubemap texture.
	* @return HRESULT Returns S_OK if successful, otherwise an error code.
	*/
	HRESULT
	CreateCubemap(Device& device,
								DeviceContext& deviceContext,
								const std::array<std::string, 6>& facePaths,
								bool generateMips /*= false*/);

	/**
	* @brief Creates a shader resource view (SRV) for a specific face of a cubemap texture.
	* @param device Pointer to the DirectX device.
	* @param cubemapTex Pointer to the cubemap texture resource.
	* @param format The format of the SRV to create.
	* @param faceIndex The index of the cubemap face (0-5).
	* @param mipLevels Number of mipmap levels for the SRV. Defaults to 1.
	* @return ID3D11ShaderResourceView* Returns a pointer to the created SRV, or nullptr on failure.
	*/
	ID3D11ShaderResourceView* 
	CreateCubemapFaceSRV( ID3D11Device* device,
												ID3D11Texture2D* cubemapTex,
												DXGI_FORMAT format,
												UINT faceIndex,
												UINT mipLevels = 1) {
	
		D3D11_SHADER_RESOURCE_VIEW_DESC d{};
		d.Format = format;
		d.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		d.Texture2DArray.MostDetailedMip = 0;
		d.Texture2DArray.MipLevels = mipLevels;       // 1 for simple view
		d.Texture2DArray.FirstArraySlice = faceIndex; // Face
		d.Texture2DArray.ArraySize = 1;               // Only this face

		ID3D11ShaderResourceView* srv = nullptr;
		if (FAILED(device->CreateShaderResourceView(cubemapTex, &d, &srv))) {
			return nullptr;
		}

		return srv;
	}

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