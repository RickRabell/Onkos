#include "Rendering/PostProcessSystem.h"
#include "Device.h"
#include "DeviceContext.h"
#include "EngineUtilities\Utilities\EditorViewportPass.h"

HRESULT
PostProcessSystem::init(Device& device) {
  HRESULT hr = S_OK;

  // The full-screen triangle is generated procedurally in the vertex shader using
  // SV_VertexID, so no vertex buffer or input layout is required.
  hr = m_shaderProgram.CreateShader(device, ShaderType::VERTEX_SHADER, "GrayScalePostProcess.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile PostProcess vertex shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_shaderProgram.CreateShader(device, ShaderType::PIXEL_SHADER, "GrayScalePostProcess.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile PostProcess pixel shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_constantBuffer.init(device, sizeof(CBPostProcess));
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess constant buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_sampler.init(device);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess sampler state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_rasterizerState.init(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess rasterizer state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_depthStencilState.init(device, false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess depth-stencil state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  return S_OK;
}

void
PostProcessSystem::render(DeviceContext& deviceContext, EditorViewportPass& source, EditorViewportPass& destination) {
  if (!deviceContext.m_deviceContext) {
    ERROR("PostProcessSystem", "render", "DeviceContext is nullptr.");
    return;
  }

  CBPostProcess cb{};
  cb.enableGrayscale = m_settings.enableGrayscale ? 1 : 0;
  m_constantBuffer.update(deviceContext, m_constantBuffer.m_buffer, 0, nullptr, &cb, 0, 0);

  const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  destination.begin(deviceContext, clearColor);
  destination.setViewport(deviceContext);

  m_rasterizerState.render(deviceContext);
  m_depthStencilState.render(deviceContext);

  deviceContext.m_deviceContext->VSSetShader(m_shaderProgram.m_VertexShader, nullptr, 0);
  deviceContext.m_deviceContext->PSSetShader(m_shaderProgram.m_PixelShader, nullptr, 0);
  deviceContext.m_deviceContext->IASetInputLayout(nullptr);

  m_constantBuffer.render(deviceContext, 0, 1, true);

  ID3D11ShaderResourceView* sourceSRV = source.getSRV();
  deviceContext.PSSetShaderResources(0, 1, &sourceSRV);
  m_sampler.render(deviceContext, 0, 1);

  deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  deviceContext.m_deviceContext->Draw(3, 0);

  // Unbind the source SRV to avoid hazards (it may be used as a render target next frame).
  ID3D11ShaderResourceView* nullSRV = nullptr;
  deviceContext.PSSetShaderResources(0, 1, &nullSRV);
}

void
PostProcessSystem::destroy() {
  m_shaderProgram.destroy();
  m_constantBuffer.destroy();
  m_sampler.destroy();
  m_rasterizerState.destroy();
  m_depthStencilState.destroy();
}
