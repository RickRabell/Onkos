#include "Rendering/PostProcessSystem.h"
#include "Device.h"
#include "DeviceContext.h"
#include "EngineUtilities\Utilities\EditorViewportPass.h"
#include "EngineUtilities\Utilities\Camera.h"

namespace {
  // Shared helper: bind the pipeline state every full-screen stage needs before its
  // Draw(3, 0) call. Every stage in the stack uses the exact same procedurally-generated
  // full-screen triangle (see any stage's .hlsl VS()), so this is the one place that
  // binds rasterizer/depth-stencil/topology/input-layout for all of them.
  void
    bindFullScreenTriangleState(DeviceContext& deviceContext,
      RasterizerState& rasterizerState,
      DepthStencilState& depthStencilState) {
    rasterizerState.render(deviceContext);
    depthStencilState.render(deviceContext);
    deviceContext.m_deviceContext->IASetInputLayout(nullptr);
    deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  }
}

HRESULT
PostProcessSystem::init(Device& device, unsigned int width, unsigned int height) {
  HRESULT hr = S_OK;

  // All four stages draw the same procedurally-generated full-screen triangle (no vertex/
  // index buffer or input layout needed), so these states are shared across every stage
  // rather than duplicated per shader.
  hr = m_sampler.init(device);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess sampler state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Cull NONE: a post-process quad should never depend on triangle winding, so this
  // guarantees it draws regardless of whatever CullMode the previous pass left configured.
  hr = m_rasterizerState.init(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess rasterizer state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Depth test fully disabled + zero depth writes: post-processing is a 2D image
  // operation, and it never matters whether the destination's own depth buffer was
  // cleared beforehand -- there's no test to fail against it.
  hr = m_depthStencilState.init(device, false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess depth-stencil state. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  //--------------------------------------------------------------------------------------
  // Stage 1: Color Effects (Grayscale + Depth Fog + Vignette)
  //--------------------------------------------------------------------------------------
  hr = m_colorEffectsShader.CreateShader(device, ShaderType::VERTEX_SHADER, "PostProcess.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile PostProcess vertex shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_colorEffectsShader.CreateShader(device, ShaderType::PIXEL_SHADER, "PostProcess.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile PostProcess pixel shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_colorEffectsConstantBuffer.init(device, sizeof(CBPostProcess));
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize PostProcess constant buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  //--------------------------------------------------------------------------------------
  // Stage 2: Bloom
  //--------------------------------------------------------------------------------------
  hr = m_bloomShader.CreateShader(device, ShaderType::VERTEX_SHADER, "Bloom.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile Bloom vertex shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_bloomShader.CreateShader(device, ShaderType::PIXEL_SHADER, "Bloom.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile Bloom pixel shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_bloomConstantBuffer.init(device, sizeof(CBBloom));
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize Bloom constant buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  //--------------------------------------------------------------------------------------
  // Stage 3: Tone Mapping
  //--------------------------------------------------------------------------------------
  hr = m_toneMappingShader.CreateShader(device, ShaderType::VERTEX_SHADER, "ToneMapping.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile ToneMapping vertex shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_toneMappingShader.CreateShader(device, ShaderType::PIXEL_SHADER, "ToneMapping.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile ToneMapping pixel shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_toneMappingConstantBuffer.init(device, sizeof(CBToneMapping));
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize ToneMapping constant buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  //--------------------------------------------------------------------------------------
  // Stage 4: FXAA / SSAO
  //--------------------------------------------------------------------------------------
  hr = m_fxaaSsaoShader.CreateShader(device, ShaderType::VERTEX_SHADER, "FXAA_SSAO.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile FXAA_SSAO vertex shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_fxaaSsaoShader.CreateShader(device, ShaderType::PIXEL_SHADER, "FXAA_SSAO.hlsl");
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to compile FXAA_SSAO pixel shader. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }
  hr = m_fxaaSsaoConstantBuffer.init(device, sizeof(CBFxaaSsao));
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize FXAA_SSAO constant buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Ping-pong scratch buffers: allocated up front at viewport resolution so that chaining
  // 2+ stages (Color Effects -> Bloom -> Tone Mapping -> FXAA/SSAO) never has to allocate
  // mid-frame. Sized/resized in lockstep with the caller's own EditorViewportPass instances
  // via resize().
  hr = m_scratchA.init(device, width, height);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize post-process scratch buffer A. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_scratchB.init(device, width, height);
  if (FAILED(hr)) {
    ERROR("PostProcessSystem", "init",
      ("Failed to initialize post-process scratch buffer B. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  m_scratchWidth = width;
  m_scratchHeight = height;

  return S_OK;
}

void
PostProcessSystem::resize(Device& device, unsigned int width, unsigned int height) {
  ensureScratchSize(device, width, height);
}

void
PostProcessSystem::ensureScratchSize(Device& device, unsigned int width, unsigned int height) {
  if (width == m_scratchWidth && height == m_scratchHeight) {
    return;
  }

  // EditorViewportPass::resize() destroys and recreates its internal textures at the new
  // size, matching the pattern BaseApp already uses for m_editorViewportPass/
  // m_postProcessViewportPass on viewport resize.
  m_scratchA.resize(device, width, height);
  m_scratchB.resize(device, width, height);

  m_scratchWidth = width;
  m_scratchHeight = height;
}

void
PostProcessSystem::render(DeviceContext& deviceContext,
  EditorViewportPass& source,
  EditorViewportPass& destination,
  const Camera& camera,
  ID3D11ShaderResourceView* worldPositionAOSRV,
  ID3D11ShaderResourceView* normalRoughnessSRV) {
  if (!deviceContext.m_deviceContext) {
    ERROR("PostProcessSystem", "render", "DeviceContext is nullptr.");
    return;
  }
  if (!m_settings.hasActiveEffects()) {
    // Nothing enabled -- the caller should already be reading source.getSRV() directly
    // instead of calling render() at all, but bail out safely either way.
    return;
  }

  //--------------------------------------------------------------------------------------
  // The Stack
  // Context: Every stage shares the exact same signature, so it can be stored as a plain
  // member-function pointer here and the loop below doesn't need to know or care how many
  // of them are actually enabled this frame. Order matters -- this is the order effects
  // are composited in: Color Effects -> Bloom -> Tone Mapping -> FXAA/SSAO.
  //--------------------------------------------------------------------------------------
  using StageFn = void (PostProcessSystem::*)(DeviceContext&,
    EditorViewportPass&,
    EditorViewportPass&,
    const Camera&,
    ID3D11ShaderResourceView*,
    ID3D11ShaderResourceView*);
  struct Stage {
    bool active;
    StageFn fn;
  };

  const Stage stages[] = {
    { m_settings.enableGrayscale || m_settings.enableFog || m_settings.enableVignette,
      &PostProcessSystem::renderColorEffectsPass },
    { m_settings.enableBloom,
      &PostProcessSystem::renderBloomPass },
    { m_settings.enableToneMapping,
      &PostProcessSystem::renderToneMappingPass },
    { m_settings.enableFXAA || m_settings.enableSSAO,
      &PostProcessSystem::renderFXAAPass },
  };
  constexpr int kStageCount = sizeof(stages) / sizeof(stages[0]);

  // Only chain through stages that actually have work to do this frame -- a disabled stage
  // costs nothing, not even a pass-through copy.
  StageFn activeStages[kStageCount];
  int activeCount = 0;
  for (int i = 0; i < kStageCount; ++i) {
    if (stages[i].active) {
      activeStages[activeCount++] = stages[i].fn;
    }
  }

  if (activeCount == 0) {
    // hasActiveEffects() said yes but every individual stage predicate said no -- shouldn't
    // happen with the current flag set, but keeps this future-proof if the two ever drift.
    return;
  }

  // Scratch buffer sizing is driven exclusively by resize() (called by BaseApp alongside
  // its own EditorViewportPass::resize() whenever the editor viewport changes size) --
  // render() intentionally never re-sizes anything itself, since DeviceContext alone
  // doesn't expose the Device handle EditorViewportPass::resize() requires.

  // Ping-pong bookkeeping: every stage before the last one writes into a scratch buffer;
  // the last active stage writes directly into `destination` so we never pay for an extra
  // final copy. `readPass` always points at whatever the previous stage just wrote (or
  // `source`, for the very first stage).
  EditorViewportPass* scratchTargets[2] = { &m_scratchA, &m_scratchB };
  EditorViewportPass* readPass = &source;
  int nextScratch = 0;

  for (int n = 0; n < activeCount; ++n) {
    const bool isLastStage = (n == activeCount - 1);
    EditorViewportPass& writePass = isLastStage ? destination : *scratchTargets[nextScratch];

    (this->*activeStages[n])(deviceContext, *readPass, writePass, camera,
      worldPositionAOSRV, normalRoughnessSRV);

    readPass = &writePass;
    nextScratch = 1 - nextScratch;
  }
}

void
PostProcessSystem::renderColorEffectsPass(DeviceContext& deviceContext,
  EditorViewportPass& source,
  EditorViewportPass& destination,
  const Camera& camera,
  ID3D11ShaderResourceView* worldPositionAOSRV,
  ID3D11ShaderResourceView* /*normalRoughnessSRV*/) {
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
  m_colorEffectsConstantBuffer.update(deviceContext, m_colorEffectsConstantBuffer.m_buffer, 0, nullptr, &cb, 0, 0);

  const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  destination.begin(deviceContext, clearColor);
  destination.setViewport(deviceContext);
  bindFullScreenTriangleState(deviceContext, m_rasterizerState, m_depthStencilState);

  deviceContext.m_deviceContext->VSSetShader(m_colorEffectsShader.m_VertexShader, nullptr, 0);
  deviceContext.m_deviceContext->PSSetShader(m_colorEffectsShader.m_PixelShader, nullptr, 0);

  m_colorEffectsConstantBuffer.render(deviceContext, 0, 1, true);

  // t0 = scene color from `source`, t1 = world-position/AO G-Buffer (may be nullptr).
  ID3D11ShaderResourceView* srvs[2] = { source.getSRV(), worldPositionAOSRV };
  deviceContext.PSSetShaderResources(0, 2, srvs);
  m_sampler.render(deviceContext, 0, 1);

  deviceContext.m_deviceContext->Draw(3, 0);

  // Unbind both SRVs to avoid hazards (either texture may be used as a render target
  // again next frame).
  ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
  deviceContext.PSSetShaderResources(0, 2, nullSRVs);
}

void
PostProcessSystem::renderBloomPass(DeviceContext& deviceContext,
  EditorViewportPass& source,
  EditorViewportPass& destination,
  const Camera& /*camera*/,
  ID3D11ShaderResourceView* /*worldPositionAOSRV*/,
  ID3D11ShaderResourceView* /*normalRoughnessSRV*/) {
  CBBloom cb{};
  cb.TexelSize = EU::Vector2(1.0f / static_cast<float>(source.getWidth()),
    1.0f / static_cast<float>(source.getHeight()));
  cb.Threshold = m_settings.bloomThreshold;
  cb.Knee = m_settings.bloomKnee;
  cb.Intensity = m_settings.bloomIntensity;
  m_bloomConstantBuffer.update(deviceContext, m_bloomConstantBuffer.m_buffer, 0, nullptr, &cb, 0, 0);

  const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  destination.begin(deviceContext, clearColor);
  destination.setViewport(deviceContext);
  bindFullScreenTriangleState(deviceContext, m_rasterizerState, m_depthStencilState);

  deviceContext.m_deviceContext->VSSetShader(m_bloomShader.m_VertexShader, nullptr, 0);
  deviceContext.m_deviceContext->PSSetShader(m_bloomShader.m_PixelShader, nullptr, 0);

  m_bloomConstantBuffer.render(deviceContext, 0, 1, true);

  ID3D11ShaderResourceView* srv = source.getSRV();
  deviceContext.PSSetShaderResources(0, 1, &srv);
  m_sampler.render(deviceContext, 0, 1);

  deviceContext.m_deviceContext->Draw(3, 0);

  ID3D11ShaderResourceView* nullSRV = nullptr;
  deviceContext.PSSetShaderResources(0, 1, &nullSRV);
}

void
PostProcessSystem::renderToneMappingPass(DeviceContext& deviceContext,
  EditorViewportPass& source,
  EditorViewportPass& destination,
  const Camera& /*camera*/,
  ID3D11ShaderResourceView* /*worldPositionAOSRV*/,
  ID3D11ShaderResourceView* /*normalRoughnessSRV*/) {
  CBToneMapping cb{};
  cb.Exposure = m_settings.exposure;
  cb._pad0 = 0.0f;
  cb._pad1 = 0.0f;
  cb._pad2 = 0.0f;
  m_toneMappingConstantBuffer.update(deviceContext, m_toneMappingConstantBuffer.m_buffer, 0, nullptr, &cb, 0, 0);

  const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  destination.begin(deviceContext, clearColor);
  destination.setViewport(deviceContext);
  bindFullScreenTriangleState(deviceContext, m_rasterizerState, m_depthStencilState);

  deviceContext.m_deviceContext->VSSetShader(m_toneMappingShader.m_VertexShader, nullptr, 0);
  deviceContext.m_deviceContext->PSSetShader(m_toneMappingShader.m_PixelShader, nullptr, 0);

  m_toneMappingConstantBuffer.render(deviceContext, 0, 1, true);

  ID3D11ShaderResourceView* srv = source.getSRV();
  deviceContext.PSSetShaderResources(0, 1, &srv);
  m_sampler.render(deviceContext, 0, 1);

  deviceContext.m_deviceContext->Draw(3, 0);

  ID3D11ShaderResourceView* nullSRV = nullptr;
  deviceContext.PSSetShaderResources(0, 1, &nullSRV);
}

void
PostProcessSystem::renderFXAAPass(DeviceContext& deviceContext,
  EditorViewportPass& source,
  EditorViewportPass& destination,
  const Camera& /*camera*/,
  ID3D11ShaderResourceView* worldPositionAOSRV,
  ID3D11ShaderResourceView* normalRoughnessSRV) {
  // SSAO needs both the world-position and normal/roughness G-Buffer targets. If either is
  // missing (e.g. Forward renderer active), silently drop SSAO for this frame -- same
  // pattern as Depth Fog above. FXAA is unaffected either way, it only reads `source`.
  const bool canApplySSAO = m_settings.enableSSAO &&
    (worldPositionAOSRV != nullptr) &&
    (normalRoughnessSRV != nullptr);

  CBFxaaSsao cb{};
  cb.TexelSize = EU::Vector2(1.0f / static_cast<float>(source.getWidth()),
    1.0f / static_cast<float>(source.getHeight()));
  cb.EnableFXAA = m_settings.enableFXAA ? 1.0f : 0.0f;
  cb.FxaaSubpixelQuality = m_settings.fxaaSubpixelQuality;
  cb.SsaoRadius = m_settings.ssaoRadius;
  cb.SsaoPower = m_settings.ssaoPower;
  cb.SsaoIntensity = m_settings.ssaoIntensity;
  cb.EnableSSAO = canApplySSAO ? 1.0f : 0.0f;
  m_fxaaSsaoConstantBuffer.update(deviceContext, m_fxaaSsaoConstantBuffer.m_buffer, 0, nullptr, &cb, 0, 0);

  const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  destination.begin(deviceContext, clearColor);
  destination.setViewport(deviceContext);
  bindFullScreenTriangleState(deviceContext, m_rasterizerState, m_depthStencilState);

  deviceContext.m_deviceContext->VSSetShader(m_fxaaSsaoShader.m_VertexShader, nullptr, 0);
  deviceContext.m_deviceContext->PSSetShader(m_fxaaSsaoShader.m_PixelShader, nullptr, 0);

  m_fxaaSsaoConstantBuffer.render(deviceContext, 0, 1, true);

  // t0 = scene color, t1 = world-position/AO, t2 = normal/roughness (either G-Buffer SRV
  // may be nullptr -- the shader only samples them when EnableSSAO is 1).
  ID3D11ShaderResourceView* srvs[3] = { source.getSRV(), worldPositionAOSRV, normalRoughnessSRV };
  deviceContext.PSSetShaderResources(0, 3, srvs);
  m_sampler.render(deviceContext, 0, 1);

  deviceContext.m_deviceContext->Draw(3, 0);

  ID3D11ShaderResourceView* nullSRVs[3] = { nullptr, nullptr, nullptr };
  deviceContext.PSSetShaderResources(0, 3, nullSRVs);
}

void
PostProcessSystem::destroy() {
  m_colorEffectsShader.destroy();
  m_colorEffectsConstantBuffer.destroy();
  m_bloomShader.destroy();
  m_bloomConstantBuffer.destroy();
  m_toneMappingShader.destroy();
  m_toneMappingConstantBuffer.destroy();
  m_fxaaSsaoShader.destroy();
  m_fxaaSsaoConstantBuffer.destroy();
  m_sampler.destroy();
  m_rasterizerState.destroy();
  m_depthStencilState.destroy();
  m_scratchA.destroy();
  m_scratchB.destroy();
}