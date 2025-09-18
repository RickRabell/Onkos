#pragma once
#include "Prerequisites.h"

/**
 * @class Device
 * @brief A wrapper class for the DirectX 11 ID3D11Device interface.
 * @author Ricardo Rabell
 * @date 2025-09-11
 *
 * This class encapsulates the ID3D11Device object, which is responsible for
 * creating resources such as textures, buffers, and shaders. It acts as a factory
 * for all GPU resources within the engine.
 */
class Device {
public:
	/**
	 * @brief Default constructor.
	 */
	Device() = default;
	/**
	 * @brief Default destructor.
	 */
	~Device() = default;

	/**
	 * @brief Initializes the underlying D3D11 device.
	 * @todo Implementation details should be added.
	 */
	void
	init();

	/**
	 * @brief Handles device-specific updates per frame.
	 * @todo Implementation details should be added.
	 */
	void
	update();

	/**
	 * @brief Handles device-specific rendering tasks.
	 * @note This is likely a placeholder, as most rendering commands are issued
	 * via the DeviceContext.
	 * @todo Implementation details should be added.
	 */
	void
	render();

	/**
	 * @brief Releases the D3D11 device and all associated resources.
	 */
	void
	destroy();

	/**
	 * @brief Creates a render-target view for accessing resource data.
	 * @param pResource Pointer to the resource that will be used as a render target.
	 * @param pDesc Pointer to a render-target view description.
	 * @param ppRTView Address of a pointer to an ID3D11RenderTargetView to be created.
	 * @return HRESULT success or error code.
	 * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createrendertargetview
	 */
	HRESULT
	CreateRenderTargetView(	ID3D11Resource* pResource,
													const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
													ID3D11RenderTargetView** ppRTView);

	/**
	 * @brief Creates a 2D texture.
	 * @param pDesc Pointer to a D3D11_TEXTURE2D_DESC structure that describes a 2D texture resource.
	 * @param pInitialData Pointer to a D3D11_SUBRESOURCE_DATA structure that describes subresources for the 2D texture.
	 * @param ppTexture2D Address of a pointer to an ID3D11Texture2D interface for the created texture.
	 * @return HRESULT success or error code.
	 * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createtexture2d
	 */
	HRESULT
	CreateTexture2D(	const D3D11_TEXTURE2D_DESC* pDesc,
										const D3D11_SUBRESOURCE_DATA* pInitialData,
										ID3D11Texture2D** ppTexture2D);

	/**
	 * @brief Creates a depth-stencil view for accessing resource data.
	 * @param pResource Pointer to the resource that will serve as the depth-stencil surface.
	 * @param pDesc Pointer to a depth-stencil-view description.
	 * @param ppDepthStencilView Address of a pointer to an ID3D11DepthStencilView.
	 * @return HRESULT success or error code.
	 * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createdepthstencilview
	 */
	HRESULT
	CreateDepthStencilView(	ID3D11Resource* pResource,
													const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
													ID3D11DepthStencilView** ppDepthStencilView);

	/**
	 * @brief Creates a vertex shader from a compiled shader.
	 * @param pShaderBytecode Pointer to the compiled shader bytecode.
	 * @param BytecodeLength Size of the compiled shader bytecode.
	 * @param pClassLinkage A pointer to a class linkage interface (can be nullptr).
	 * @param ppVertexShader Address of a pointer to an ID3D11VertexShader interface.
	 * @return HRESULT success or error code.
	 * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createvertexshader
	 */
	HRESULT
	CreateVertexShader(	const void* pShaderBytecode,
											SIZE_T BytecodeLength,
											ID3D11ClassLinkage* pClassLinkage,
											ID3D11VertexShader** ppVertexShader);

	/**
	 * @brief Creates an input-layout object to describe the input-buffer data for the input-assembler stage.
	 * @param pInputElementDescs An array of D3D11_INPUT_ELEMENT_DESC structures.
	 * @param NumElements The number of input-data types in the array of input elements.
	 * @param pShaderBytecodeWithInputSignature Pointer to the compiled shader.
	 * @param BytecodeLength Size of the compiled shader.
	 * @param ppInputLayout Address of a pointer to the created ID3D11InputLayout object.
	 * @return HRESULT success or error code.
	 * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createinputlayout
	 */
	HRESULT
	CreateInputLayout(	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
											UINT NumElements,
											const void* pShaderBytecodeWithInputSignature,
											SIZE_T BytecodeLength,
											ID3D11InputLayout** ppInputLayout);

	/**
	 * @brief Creates a pixel shader from a compiled shader.
	 * @param pShaderBytecode Pointer to the compiled shader bytecode.
	 * @param BytecodeLength Size of the compiled shader bytecode.
	 * @param pClassLinkage A pointer to a class linkage interface (can be nullptr).
	 * @param ppPixelShader Address of a pointer to an ID3D11PixelShader interface.
	 * @return HRESULT success or error code.
	 * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createpixelshader
	 */
	HRESULT
	CreatePixelShader(	const void* pShaderBytecode,
											SIZE_T BytecodeLength,
											ID3D11ClassLinkage* pClassLinkage,
											ID3D11PixelShader** ppPixelShader);

	/**
	 * @brief Creates a buffer (vertex, index, or constant).
	 * @param pDesc A pointer to a D3D11_BUFFER_DESC structure that describes the buffer.
	 * @param pInitialData A pointer to a D3D11_SUBRESOURCE_DATA structure that describes the initialization data.
	 * @param ppBuffer Address of a pointer to an ID3D11Buffer interface for the created buffer.
	 * @return HRESULT success or error code.
	 * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	 */
	HRESULT
	CreateBuffer( const D3D11_BUFFER_DESC* pDesc,
								const D3D11_SUBRESOURCE_DATA* pInitialData,
								ID3D11Buffer** ppBuffer);

	/**
	* @brief Creates a sampler-state object that encapsulates sampling information for a texture.
	* @param pSamplerDesc Pointer to a sampler-state description (see D3D11_SAMPLER_DESC).
	* @param ppSamplerState Address of a pointer to the sampler-state object created (see ID3D11SamplerState).
	* @return HRESULT success or error code.
	* @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3D11-id3d11device-createsamplerstate
	*/
	HRESULT 
	CreateSamplerState( const D3D11_SAMPLER_DESC* pSamplerDesc,
											ID3D11SamplerState** ppSamplerState);

public:
	/**
	 * @brief Pointer to the underlying DirectX 11 device interface.
	 */
	ID3D11Device* m_device = nullptr;
};