#pragma once
#include "Prerequisites.h"

/**
 * @class DeviceContext
 * @brief A wrapper class for the DirectX 11 ID3D11DeviceContext interface.
 * @author Ricardo Rabell
 * @date 2025-09-12
 *
 * This class encapsulates the ID3D11DeviceContext object, which is responsible
 * for generating rendering commands. It sets pipeline state and issues drawing
 * calls to the GPU.
 */
class DeviceContext {
public:
  /**
   * @brief Default constructor.
   */
	DeviceContext() = default;
  /**
   * @brief Default destructor.
   */
	~DeviceContext() = default;
	
  /**
   * @brief Initializes the underlying D3D11 device context.
   * @todo Implementation details should be added.
   */
	void
	init();

  /**
   * @brief Handles per-frame updates that require the device context, like updating buffers.
   * @todo Implementation details should be added.
   */
  void
  update();

  /**
   * @brief Main render loop entry point. Executes rendering commands for a frame.
   * @todo Implementation details should be added.
   */
	void
	render();

  /**
   * @brief Releases the D3D11 device context.
   */
	void
	destroy();

  /**
   * @brief Binds one or more render targets and the depth-stencil buffer to the output-merger stage.
   * @param NumViews Number of render targets to bind.
   * @param ppRenderTargetViews Pointer to an array of render-target views.
   * @param pDepthStencilView Pointer to the depth-stencil view.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omsetrendertargets
   */
	void
	OMSetRenderTargets( unsigned int NumViews, 
											ID3D11RenderTargetView* const* ppRenderTargetViews,
											ID3D11DepthStencilView* pDepthStencilView);
	
  /**
   * @brief Bind an array of viewports to the rasterizer stage of the pipeline.
   * @param NumViewports Number of viewports to bind.
   * @param pViewports An array of D3D11_VIEWPORT structures to bind to the device.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-rssetviewports
   */
	void
	RSSetViewports( unsigned int NumViewports,
									const D3D11_VIEWPORT* pViewports);

  /**
   * @brief Binds an input-layout object to the input-assembler stage.
   * @param pInputLayout A pointer to the input-layout object.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetinputlayout
   */
	void
	IASetInputLayout( ID3D11InputLayout* pInputLayout);

  /**
   * @brief Binds an array of vertex buffers to the input-assembler stage.
   * @param StartSlot The first input slot for binding.
   * @param NumBuffers The number of vertex buffers in the array.
   * @param ppVertexBuffers A pointer to an array of vertex buffers.
   * @param pStrides A pointer to an array of stride values; one stride for each buffer.
   * @param pOffsets A pointer to an array of offset values; one offset for each buffer.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetvertexbuffers
   */
	void
	IASetVertexBuffers(unsigned int StartSlot,
											unsigned int NumBuffers,
											ID3D11Buffer* const* ppVertexBuffers,
											const unsigned int* pStrides,
											const unsigned int* pOffsets);

  /**
   * @brief Binds an index buffer to the input-assembler stage.
   * @param pIndexBuffer A pointer to an ID3D11Buffer object for the index buffer.
   * @param Format A DXGI_FORMAT that specifies the format of the data in the index buffer.
   * @param Offset Offset (in bytes) from the start of the index buffer to the first index to use.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetindexbuffer
   */
	void 
	IASetIndexBuffer(	ID3D11Buffer* pIndexBuffer,
										DXGI_FORMAT Format,
										unsigned int Offset);

  /**
   * @brief The CPU copies data from memory to a subresource created in non-mappable memory.
   * @param pDstResource A pointer to the destination resource.
   * @param DstSubresource A zero-based index, that identifies the destination subresource.
   * @param pDstBox A pointer to a box that defines the portion of the destination subresource to copy the resource data into.
   * @param pSrcData A pointer to the source data in memory.
   * @param SrcRowPitch The size of one row of the source data.
   * @param SrcDepthPitch The size of one depth slice of the source data.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
   */
  void 
	UpdateSubresource(  ID3D11Resource* pDstResource,
											unsigned int DstSubresource,
											const D3D11_BOX* pDstBox,
											const void* pSrcData,
											unsigned int SrcRowPitch,
											unsigned int SrcDepthPitch);

  /**
   * @brief Binds information about the primitive type, and data order that describes input data for the input assembler stage.
   * @param Topology The type of primitive to be rendered.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetprimitivetopology
   */
	void
	IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY Topology );

  /**
   * @brief The CPU copies data from memory to a subresource created in non-mappable memory.
   * @param pDstResource A pointer to the destination resource.
   * @param DstSubresource A zero-based index, that identifies the destination subresource.
   * @param pDstBox A pointer to a box that defines the portion of the destination subresource to copy the resource data into.
   * @param pSrcData A pointer to the source data in memory.
   * @param SrcRowPitch The size of one row of the source data.
   * @param SrcDepthPitch The size of one depth slice of the source data.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
   */
  void
  UpdateSubresource(  ID3D11Resource* pDstResource,
                      unsigned int DstSubresource,
                      const D3D11_BOX* pDstBox,
                      const void* pSrcData,
                      unsigned int SrcRowPitch,
                      unsigned int SrcDepthPitch);

  /**
   * @brief Sets all the elements in a render target to one value.
   * @param pRenderTargetView Pointer to the render target.
   * @param ColorRGBA A 4-component array that represents the color to fill the render target with.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-clearrendertargetview
   */
  void
  ClearRenderTargetView(  ID3D11RenderTargetView* pRenderTargetView,
                          const FLOAT ColorRGBA[4]);

  /**
   * @brief Clears the depth-stencil resource.
   * @param pDepthStencilView Pointer to the depth-stencil to be cleared.
   * @param ClearFlags Specify the parts of the buffer to clear (D3D11_CLEAR_FLAG).
   * @param Depth Clear the depth buffer with this value. Ranges from 0 to 1.
   * @param Stencil Clear the stencil buffer with this value. Ranges from 0 to 255.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-cleardepthstencilview
   */
  void
  ClearDepthStencilView(  ID3D11DepthStencilView* pDepthStencilView,
                          unsigned int ClearFlags,
                          FLOAT Depth,
                          UINT8 Stencil);

  /**
   * @brief Sets a vertex shader to the device.
   * @param pVertexShader Pointer to a vertex shader.
   * @param ppClassInstances An array of class-instance interfaces (can be nullptr).
   * @param NumClassInstances The number of class-instance interfaces in the array.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-vssetshader
   */
  void
  VSSetShader(  ID3D11VertexShader* pVertexShader,
                ID3D11ClassInstance* const* ppClassInstances,
                unsigned int NumClassInstances);

  /**
   * @brief Sets the constant buffers used by the vertex shader pipeline stage.
   * @param StartSlot Index into the device's zero-based array to begin setting constant buffers to.
   * @param NumBuffers Number of buffers to set.
   * @param ppConstantBuffers Array of constant buffers.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-vssetconstantbuffers
   */
  void
  VSSetConstantBuffers( unsigned int StartSlot,
                        unsigned int NumBuffers,
                        ID3D11Buffer* const* ppConstantBuffers);

  /**
   * @brief Sets a pixel shader to the device.
   * @param pPixelShader Pointer to a pixel shader.
   * @param ppClassInstances An array of class-instance interfaces (can be nullptr).
   * @param NumClassInstances The number of class-instance interfaces in the array.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetshader
   */
  void
  PSSetShader(  ID3D11PixelShader* pPixelShader,
                  ID3D11ClassInstance* const* ppClassInstances,
                  unsigned int NumClassInstances);

  /**
   * @brief Sets the constant buffers used by the pixel shader pipeline stage.
   * @param StartSlot Index into the device's zero-based array to begin setting constant buffers to.
   * @param NumBuffers Number of buffers to set.
   * @param ppConstantBuffers Array of constant buffers.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetconstantbuffers
   */
  void
  PSSetConstantBuffers( unsigned int StartSlot,
                          unsigned int NumBuffers,
                          ID3D11Buffer* const* ppConstantBuffers);

  /**
   * @brief Binds an array of shader resources to the pixel shader stage.
   * @param StartSlot Index into the device's zero-based array to begin setting shader resources to.
   * @param NumViews Number of shader resources to set.
   * @param ppShaderResourceViews Array of shader resource view interfaces to set to the device.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetshaderresources
   */
  void
  PSSetShaderResources( unsigned int StartSlot,
                        unsigned int NumViews,
                        ID3D11ShaderResourceView* const* ppShaderResourceViews);

  /**
   * @brief Sets an array of sampler states to the pixel shader pipeline stage.
   * @param StartSlot Index into the device's zero-based array to begin setting samplers to.
   * @param NumSamplers Number of samplers in the array.
   * @param ppSamplers Pointer to an array of sampler-state interfaces.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetsamplers
   */
  void
  PSSetSamplers(  unsigned int StartSlot,
                  unsigned int NumSamplers,
                  ID3D11SamplerState* const* ppSamplers);

  /**
   * @brief Draws indexed, non-instanced primitives.
   * @param IndexCount Number of indices to draw.
   * @param StartIndexLocation The location of the first index read by the GPU from the index buffer.
   * @param BaseVertexLocation A value added to each index before reading a vertex from the vertex buffer.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-drawindexed
   */
  void
  DrawIndexed(  unsigned int IndexCount,
                unsigned int StartIndexLocation,
                INT BaseVertexLocation);

  /**
   * @brief Sets the rasterizer state for the rasterizer stage of the pipeline.
   * @param pRasterizerState Pointer to a rasterizer-state interface to bind to the pipeline.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-rssetstate
   */
  void
  RSSetState( ID3D11RasterizerState* pRasterizerState );

  /**
   * @brief Sets the blend state of the output-merger stage.
   * @param pBlendState Pointer to a blend-state interface.
   * @param BlendFactor 4-component array of blend factors, one for each RGBA component.
   * @param SampleMask A 32-bit sample coverage.
   * @see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omsetblendstate
   */
  void
  OMSetBlendState(  ID3D11BlendState* pBlendState,
                    const float BlendFactor[4],
                    unsigned int SampleMask);

public:
  /**
   * @brief Pointer to the underlying DirectX 11 device context interface.
   */
	ID3D11DeviceContext* m_deviceContext = nullptr;
};