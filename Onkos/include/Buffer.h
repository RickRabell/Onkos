#pragma once
#include "Prerequisites.h"
#include "MeshComponent.h"

// Forward declarations
class Device;
class DeviceContext;

/**
 * @class Buffer
 * @brief A generic wrapper for a Direct3D 11 buffer (ID3D11Buffer).
 * @author Ricardo Rabell
 * @date 2025-10-16
 *
 * This class encapsulates an ID3D11Buffer. It is designed to be versatile and
 * can manage Vertex Buffers, Index Buffers, and Constant Buffers. The behavior
 * of the buffer (how it's created and bound) is determined by the `bindFlag`
 * provided during initialization.
 */
class 
Buffer {
public:
	/**
	 * @brief Default constructor.
	 */
	Buffer() = default;

	/**
	 * @brief Default destructor.
	 */
	~Buffer() = default;

	/**
	 * @brief Initializes the buffer as a static resource from mesh data.
	 * This is typically used for creating Vertex Buffers or Index Buffers.
	 * @param device The graphics device used to create the buffer.
	 * @param mesh The mesh component containing the vertex or index data.
	 * @param bindFlag The bind flag (e.g., D3D11_BIND_VERTEX_BUFFER, D3D11_BIND_INDEX_BUFFER).
	 * @return HRESULT S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(Device& device, const MeshComponent& mesh, unsigned int bindFlag);

	/**
	 * @brief Initializes the buffer for dynamic use, typically a constant buffer.
	 * This creates a buffer of a specific size with D3D11_USAGE_DYNAMIC and
	 * D3D11_CPU_ACCESS_WRITE flags.
	 * @param device The graphics device used to create the buffer.
	 * @param byteWidth The size of the buffer in bytes.
	 * @return HRESULT S_OK if successful.
	 */
	HRESULT
	init(Device& device, unsigned int byteWidth);

	/**
	 * @brief Updates the buffer's contents. Wrapper for ID3D11DeviceContext::UpdateSubresource.
	 * @note This is typically used for default-usage buffers (D3D11_USAGE_DEFAULT).
	 * For dynamic buffers, Map/Unmap is preferred.
	 * @param deviceContext The device context to issue the update command.
	 * @param pDstResource The destination buffer resource (should be m_buffer).
	 * @param DstSubresource The destination subresource index.
	 * @param pDstBox A box defining the region to update. nullptr for the whole resource.
	 * @param pSrcData Pointer to the source data in system memory.
	 * @param SrcRowPitch The row pitch of the source data.
	 * @param SrcDepthPitch The depth pitch of the source data.
	 */
	void
	update(DeviceContext& deviceContext,
				 ID3D11Resource* pDstResource,
				 unsigned int DstSubresource,
				 const D3D11_BOX* pDstBox,
				 const void* pSrcData,
				 unsigned int SrcRowPitch,
				 unsigned int SrcDepthPitch);

	/**
	 * @brief Binds the buffer to the graphics pipeline.
	 * This method's behavior changes based on the m_bindFlag set during init.
	 * - If D3D11_BIND_VERTEX_BUFFER: Calls IASetVertexBuffers.
	 * - If D3D11_BIND_INDEX_BUFFER: Calls IASetIndexBuffer.
	 * - If D3D11_BIND_CONSTANT_BUFFER: Calls VSSetConstantBuffers or PSSetConstantBuffers.
	 * @param deviceContext The device context to issue the binding command.
	 * @param StartSlot The starting slot to bind the buffer to.
	 * @param NumBuffers The number of buffers to bind (typically 1).
	 * @param setPixelShader If true and this is a constant buffer, binds to the Pixel Shader stage. Otherwise, binds to Vertex Shader.
	 * @param format The DXGI_FORMAT for an index buffer (e.g., DXGI_FORMAT_R32_UINT).
	 */
	void
	render(DeviceContext& deviceContext,
				 unsigned int StartSlot,
				 unsigned int NumBuffers,
				 bool setPixelShader = false,
				 DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

	/**
	 * @brief Releases the underlying ID3D11Buffer COM object.
	 */
	void
	destroy();

	/**
	 * @brief A private helper function to create the actual D3D11 buffer.
	 * @param device The graphics device.
	 * @param desc The D3D11_BUFFER_DESC describing the buffer.
	 * @param initData Optional initial data for the buffer.
	 * @return HRESULT S_OK if successful.
	 */
	HRESULT
	createBuffer(Device& device,
 							 D3D11_BUFFER_DESC& desc,
	 						 D3D11_SUBRESOURCE_DATA* initData);

private:
	/** @brief Pointer to the underlying DirectX 11 buffer interface. */
	ID3D11Buffer* m_buffer = nullptr;

	/** @brief The size in bytes of a single element (e.g., sizeof(SimpleVertex)). Used for vertex buffers. */
	unsigned int m_stride = 0;

	/** @brief The offset in bytes from the start of the buffer. Almost always 0. Used for vertex buffers. */
	unsigned int m_offset = 0;

	/** @brief Stores the bind flag (e.g., D3D11_BIND_VERTEX_BUFFER) to determine behavior in render(). */
	unsigned int m_bindFlag = 0;
};