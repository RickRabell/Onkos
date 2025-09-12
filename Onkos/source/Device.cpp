#include "Device.h"

void
Device::destroy() {
  SAFE_RELEASE(m_device);
}

//CreateRenderTargetView
HRESULT Device::CreateRenderTargetView(ID3D11Resource* pResource,
  const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
  ID3D11RenderTargetView** ppRTView) {
  // Validar parametros de entrada
  if (!pResource) {
    ERROR("Device", "CreateRenderTargetView", "pResource is nullptr");
    return E_INVALIDARG;
  }

  if (!ppRTView) {
    ERROR("Device", "CreateRenderTargetView", "ppRTView is nullptr");
    return E_POINTER;
  }

  // Crear el Render Target View
  HRESULT hr = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateRenderTargetView",
      "Render Target View created successfully!");
  }
  else {
    ERROR("Device", "CreateRenderTargetView",
      ("Failed to create Render Target View. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}


//CreateTexture2D
HRESULT Device::CreateTexture2D(
  const D3D11_TEXTURE2D_DESC* pDesc,
  const D3D11_SUBRESOURCE_DATA* pInitialData,
  ID3D11Texture2D** ppTexture2D)
{
  if (!m_device) {
    ERROR("Device", "CreateTexture2D", "m_device is nullptr");
    return E_FAIL;
  }

  if (!pDesc) {
    ERROR("Device", "CreateTexture2D", "pDesc is nullptr");
    return E_INVALIDARG;
  }

  if (!ppTexture2D) {
    ERROR("Device", "CreateTexture2D", "ppTexture2D is nullptr");
    return E_POINTER;
  }

  HRESULT hr = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateTexture2D",
      "Texture2D created successfully!");
  }
  else {
    ERROR("Device", "CreateTexture2D",
      ("Failed to create Texture2D. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}


//CreateDepthStencilView
HRESULT Device::CreateDepthStencilView(
  ID3D11Resource* pResource,
  const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
  ID3D11DepthStencilView** ppDepthStencilView)
{
  if (!m_device) {
    ERROR("Device", "CreateDepthStencilView", "m_device is nullptr");
    return E_FAIL;
  }

  if (!pResource) {
    ERROR("Device", "CreateDepthStencilView", "pResource is nullptr");
    return E_INVALIDARG;
  }

  if (!ppDepthStencilView) {
    ERROR("Device", "CreateDepthStencilView", "ppDepthStencilView is nullptr");
    return E_POINTER;
  }

  HRESULT hr = m_device->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateDepthStencilView",
      "DepthStencilView created successfully!");
  }
  else {
    ERROR("Device", "CreateDepthStencilView",
      ("Failed to create DepthStencilView. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

HRESULT 
Device::CreateVertexShader( const void* pShaderBytecode, 
                            SIZE_T BytecodeLength, 
                            ID3D11ClassLinkage* pClassLinkage, 
                            ID3D11VertexShader** ppVertexShader)
{
  if (!m_device) {
    ERROR("Device", "CreateVertexShader", "m_device is nullptr");
    return E_FAIL;
  }

  if (!pShaderBytecode) {
    ERROR("Device", "CreateVertexShader", "pShaderBytecode is nullptr");
    return E_INVALIDARG;
  }

  if (!pClassLinkage) {
    ERROR("Device", "CreateVertexShader", "pClassLinkage is nullptr");
    return E_POINTER;
  }

  if (!ppVertexShader) {
    ERROR("Device", "CreateVertexShader", "ppVertexShader is nullptr");
    return E_POINTER;
  }

  HRESULT hr = m_device->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateVertexShader",
      "VertexShader created successfully!");
  }
  else {
    ERROR("Device", "CreateVertexShader",
      ("Failed to create VertexShader. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

HRESULT Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout** ppInputLayout)
{
  return E_NOTIMPL;
}

HRESULT Device::CreatePixelShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11PixelShader** ppPixelShader)
{
  return E_NOTIMPL;
}

HRESULT Device::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer)
{
  return E_NOTIMPL;
}

HRESULT Device::CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState)
{
  return E_NOTIMPL;
}
