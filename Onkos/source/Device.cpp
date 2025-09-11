#include "Device.h"

/*
HRESULT
CreateRenderTargetView(ID3D11Resource* pResource,
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

	if (SUCCEDED(hr)) {
		MESSAGE("Device", "CreatedRenderTargetView", "Render Target View created successfully!");
	}
	else {
		ERROR("Device", "CreateRenderTargetView", "Failed to create Render Target View. HRESULT: " + std::to_string(hr)).c_str())
	}

	return hr;
}

//hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);

//hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);

// Vertex shader, input layout y pixel shader, buffer y sampler state
*/

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