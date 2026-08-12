#include "Rendering/PostProcessSystem.h"
#include "Device.h"
#include "DeviceContext.h"
#include "EngineUtilities\Utilities\EditorViewportPass.h"
#include "EngineUtilities\Utilities\Camera.h"

HRESULT
PostProcessSystem::init(Device& device) {
  HRESULT hr = S_OK;

  // The full-screen triangle is generated procedurally in the vertex shader using
  // SV_VertexID, so no vertex buffer or input layout is required.
  hr = m_shaderProgram.CreateShader(device, ShaderType::VERTEX_SHADER, "PostProcess.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile PostProcess vertex shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_shaderProgram.CreateShader(device, ShaderType::PIXEL_SHADER, "PostProcess.hlsl");
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

  // Cull NONE: the full-screen triangle's winding should already match D3D11's default
  // front face, but a post-process quad should never depend on winding at all -- this
  // guarantees it draws regardless of whatever CullMode the previous geometry pass left
  // configured, since we never explicitly reset it otherwise.
  hr = m_rasterizerState.init(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess rasterizer state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Depth test fully disabled + zero depth writes: post-processing is a 2D image
  // operation. This also means it never matters whether `destination`'s own depth
  // buffer was cleared before this pass -- there's no test to fail against it.
  hr = m_depthStencilState.init(device, false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess depth-stencil state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  return S_OK;
}

void
PostProcessSystem::render(DeviceContext& deviceContext,
  EditorViewportPass& source,
  EditorViewportPass& destination,
  const Camera& camera,
  ID3D11ShaderResourceView* worldPositionAOSRV) {
  if (!deviceContext.m_deviceContext) {
    ERROR("PostProcessSystem", "render", "DeviceContext is nullptr.");
    return;
  }
  if (!m_settings.hasActiveEffects()) {
    // Nothing enabled -- the caller should already be reading source.getSRV() directly
    // instead of calling render() at all, but bail out safely either way.
    return;
  }

  // Depth Fog needs the deferred G-Buffer's world-position/AO target. If the active
  // renderer is Forward (or the G-Buffer simply isn't ready), RenderPipeline::
  // getGBufferWorldAoSRV() returns nullptr -- silently drop fog for this frame instead
  // of sampling garbage data. The checkbox stays checked; it resumes working on its own
  // once Deferred is active again.
  const bool canApplyFog = m_settings.enableFog && (worldPositionAOSRV != nullptr);

  float clampedSmoothness = m_settings.vignetteSmoothness;
  if (clampedSmoothness < 0.001f) {
    clampedSmoothness = 0.001f;
  }

  CBPostProcess cb{};
  cb.FogColor = m_settings.fogColor;
  cb.FogStart = m_settings.fogStart;
  cb.CameraPos = camera.getPosition();
  cb.FogRange = m_settings.fogRange;
  cb.VignetteIntensity = m_settings.vignetteIntensity;
  cb.VignetteSmoothness = clampedSmoothness;
  cb.EnableFog = canApplyFog ? 1.0f : 0.0f;
  cb.EnableVignette = m_settings.enableVignette ? 1.0f : 0.0f;
  cb.EnableGrayscale = m_settings.enableGrayscale ? 1.0f : 0.0f;
  cb._pad0 = 0.0f;
  cb._pad1 = 0.0f;
  cb._pad2 = 0.0f;
  m_constantBuffer.update(deviceContext, m_constantBuffer.m_buffer, 0, nullptr, &cb, 0, 0);

  const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  destination.begin(deviceContext, clearColor);
  destination.setViewport(deviceContext);

  // This is the pair of calls the previous Fog/Vignette attempt was missing entirely --
  // without them, whatever RasterizerState/DepthStencilState the geometry pass left bound
  // stays active, which can silently discard the full-screen triangle before it ever
  // reaches the pixel shader (leaving only `destination`'s clear color on screen).
  m_rasterizerState.render(deviceContext);
  m_depthStencilState.render(deviceContext);

  deviceContext.m_deviceContext->VSSetShader(m_shaderProgram.m_VertexShader, nullptr, 0);
  deviceContext.m_deviceContext->PSSetShader(m_shaderProgram.m_PixelShader, nullptr, 0);
  deviceContext.m_deviceContext->IASetInputLayout(nullptr);

  m_constantBuffer.render(deviceContext, 0, 1, true);

  // t0 = scene color from `source`, t1 = world-position/AO G-Buffer (may be nullptr).
  ID3D11ShaderResourceView* srvs[2] = { source.getSRV(), worldPositionAOSRV };
  deviceContext.PSSetShaderResources(0, 2, srvs);
  m_sampler.render(deviceContext, 0, 1);

  deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  deviceContext.m_deviceContext->Draw(3, 0);

  // Unbind both SRVs to avoid hazards (either texture may be used as a render target
  // again next frame).
  ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
  deviceContext.PSSetShaderResources(0, 2, nullSRVs);
}

void
PostProcessSystem::destroy() {
  m_shaderProgram.destroy();
  m_constantBuffer.destroy();
  m_sampler.destroy();
  m_rasterizerState.destroy();
  m_depthStencilState.destroy();
}