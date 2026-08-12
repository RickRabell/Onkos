#pragma once
#include "Prerequisites.h"
#include "ShaderProgram.h"
#include "Buffer.h"
#include "SamplerState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "EngineUtilities\Utilities\LayoutBuilder.h"
#include "EngineUtilities\Utilities\EditorViewportPass.h"

// Forward declarations
class Device;
class DeviceContext;
class Camera;

/**
 * @struct PostProcessSettings
 * @brief Holds the CPU-side toggles for the available post-processing effects.
 * @author Ricardo Rabell
 * @date 2026-05-01
 *
 * @details
 * **Context:** Post-processing effects are enabled/disabled at runtime from the editor UI.
 * This struct centralizes those flags so both the GUI and the render pipeline can share
 * a single source of truth without tightly coupling to each other.
 *
 * **Stack layout:** Flags are grouped by which stage of the `PostProcessSystem` pass stack
 * they belong to (see `PostProcessSystem::render`). Every stage below has a real shader.
 */
struct
PostProcessSettings {
  // --- Color Effects stage (Grayscale + Depth Fog + Vignette, single combined shader) ---
  /** @brief When true, the final image is converted to grayscale. */
  bool enableGrayscale = false;

  // --- Depth Fog ---
  /** @brief When true, applies world-position based distance fog. */
  bool enableFog = false;
  /** @brief Tint applied to pixels as they fade into fog. */
  EU::Vector3 fogColor{ 0.55f, 0.60f, 0.65f };
  /** @brief Distance from the camera (world units) where fog begins. */
  float fogStart = 10.0f;
  /** @brief Distance (world units) over which fog ramps from 0% to 100%. */
  float fogRange = 60.0f;

  // --- Vignette ---
  /** @brief When true, applies cinematic edge-darkening. */
  bool enableVignette = false;
  /** @brief 0 = no darkening, 1 = corners fade to black. */
  float vignetteIntensity = 0.55f;
  /** @brief 0 = hard edge, 1 = very soft/gradual falloff. */
  float vignetteSmoothness = 0.45f;

  // --- Bloom stage ---
  /** @brief When true, blooms out bright pixels above bloomThreshold. */
  bool enableBloom = false;
  /** @brief Luminance threshold above which pixels start contributing to the bloom. */
  float bloomThreshold = 1.0f;
  /** @brief Soft-knee width around the threshold (0 = hard cutoff, larger = smoother ramp-in). */
  float bloomKnee = 0.5f;
  /** @brief Strength of the blurred bright-pass added back onto the scene. */
  float bloomIntensity = 0.6f;

  // --- Tone Mapping stage ---
  /** @brief When true, tone-maps the scene color down to displayable range (ACES filmic curve). */
  bool enableToneMapping = false;
  /** @brief Exposure multiplier applied before the tone-mapping curve. */
  float exposure = 1.0f;

  // --- FXAA stage ---
  /** @brief When true, applies Fast Approximate Anti-Aliasing to smooth jagged edges. */
  bool enableFXAA = false;
  /** @brief 0 = edges only, 1 = maximum sub-pixel blur. Trades sharpness for smoothness. */
  float fxaaSubpixelQuality = 0.75f;

  // --- SSAO stage ---
  /** @brief When true, darkens crevices/contact points using screen-space ambient occlusion. */
  bool enableSSAO = false;
  /** @brief World-space sampling radius used to look for occluding geometry. */
  float ssaoRadius = 0.75f;
  /** @brief Contrast/falloff curve applied to the raw occlusion term. */
  float ssaoPower = 1.5f;
  /** @brief Overall strength of the darkening effect. */
  float ssaoIntensity = 1.0f;

  /**
   * @brief Whether any effect is currently enabled.
   * Context: Used both to decide if the post-process pass should run at all, and to
   * pick which EditorViewportPass SRV to hand to the GUI viewport panel when none are.
   */
  bool
  hasActiveEffects() const {
    return enableGrayscale || enableFog || enableVignette ||
      enableBloom || enableToneMapping || enableFXAA || enableSSAO;
  }
};

/**
 * @brief CPU-side mirror of the constant buffer consumed by PostProcess.hlsl (Color Effects stage).
 * @details Laid out in strict 16-byte rows (the same "vector3 + trailing float" padding
 * idiom already used by CBMain in Prerequisites.h). Every enable flag is packed as a
 * 0.0f/1.0f float rather than an HLSL bool to avoid cbuffer bool-packing pitfalls, and
 * the shader combines effects multiplicatively (lerp/scale by the flag) instead of
 * branching, so there is no dynamic flow control to get wrong.
 *
 * Row 1: FogColor.xyz        | FogStart
 * Row 2: CameraPos.xyz       | FogRange
 * Row 3: VignetteIntensity   | VignetteSmoothness | EnableFog | EnableVignette
 * Row 4: EnableGrayscale     | _pad0 | _pad1 | _pad2
 */
struct
CBPostProcess {
  EU::Vector3 FogColor;
  float       FogStart;
  EU::Vector3 CameraPos;
  float       FogRange;
  float       VignetteIntensity;
  float       VignetteSmoothness;
  float       EnableFog;
  float       EnableVignette;
  float       EnableGrayscale;
  float       _pad0;
  float       _pad1;
  float       _pad2;
};

static_assert(sizeof(CBPostProcess) % 16 == 0,
  "CBPostProcess must stay a multiple of 16 bytes -- D3D11 constant buffers require it.");

/**
 * @brief CPU-side mirror of the constant buffer consumed by Bloom.hlsl.
 * Row 1: TexelSize.xy (1/width, 1/height of the source being sampled) | Threshold | Knee.
 * Row 2: Intensity | _pad0 | _pad1 | _pad2.
 */
struct
CBBloom {
  EU::Vector2 TexelSize;
  float       Threshold;
  float       Knee;
  float       Intensity;
  float       _pad0;
  float       _pad1;
  float       _pad2;
};

static_assert(sizeof(CBBloom) % 16 == 0,
  "CBBloom must stay a multiple of 16 bytes -- D3D11 constant buffers require it.");

/**
 * @brief CPU-side mirror of the constant buffer consumed by ToneMapping.hlsl.
 * Row 1: Exposure | _pad0 | _pad1 | _pad2.
 */
struct
CBToneMapping {
  float Exposure;
  float _pad0;
  float _pad1;
  float _pad2;
};

static_assert(sizeof(CBToneMapping) % 16 == 0,
  "CBToneMapping must stay a multiple of 16 bytes -- D3D11 constant buffers require it.");

/**
 * @brief CPU-side mirror of the constant buffer consumed by FXAA_SSAO.hlsl.
 * Row 1: TexelSize.xy | EnableFXAA | FxaaSubpixelQuality.
 * Row 2: SsaoRadius | SsaoPower | SsaoIntensity | EnableSSAO.
 */
struct
CBFxaaSsao {
  EU::Vector2 TexelSize;
  float       EnableFXAA;
  float       FxaaSubpixelQuality;
  float       SsaoRadius;
  float       SsaoPower;
  float       SsaoIntensity;
  float       EnableSSAO;
};

static_assert(sizeof(CBFxaaSsao) % 16 == 0,
  "CBFxaaSsao must stay a multiple of 16 bytes -- D3D11 constant buffers require it.");

/**
 * @class PostProcessSystem
 * @brief Applies a stack of full-screen post-processing effects to an already-rendered scene texture.
 * @author Ricardo Rabell
 * @date 2026-05-01
 *
 * @details
 * **Context:** Once the 3D scene has been rendered into an off-screen texture (via
 * `EditorViewportPass`), we often want to apply effects that work on the final 2D image
 * rather than the 3D geometry (grayscale, depth fog, vignette, bloom, tone mapping, FXAA, SSAO...).
 *
 * **Why Ping-Ponging:** Each stage in the stack needs to read the output of the previous
 * stage and write somewhere new -- a texture can't be bound as both an input (SRV) and a
 * render target (RTV) at the same time. With only two textures ("ping" and "pong") we can
 * chain an arbitrary number of stages: stage N reads from whichever texture stage N-1 wrote
 * to, and writes into the other one. `PostProcessSystem` owns two internal scratch
 * `EditorViewportPass` buffers for exactly this purpose, so the caller (`BaseApp`) only ever
 * has to provide the original scene texture (`source`) and where the final image should end
 * up (`destination`) -- it doesn't need to know or care how many stages ran in between.
 *
 * **The Stack (in execution order):**
 *   1. Color Effects  -- Grayscale + Depth Fog + Vignette      (PostProcess.hlsl)
 *   2. Bloom           -- soft-threshold bright-pass + blur    (Bloom.hlsl)
 *   3. Tone Mapping     -- exposure + ACES filmic curve         (ToneMapping.hlsl)
 *   4. FXAA / SSAO      -- edge anti-aliasing + screen-space AO (FXAA_SSAO.hlsl)
 * Each stage only runs if its settings flag(s) are enabled; disabled stages are skipped
 * entirely (no wasted GPU passes).
 *
 * **Analogy:** Think of this as a photo-editing filter stack (Instagram-style). Each filter
 * takes the previous filter's output as its input and produces a new image. Ping-ponging
 * between two scratch canvases means you never need more canvases than filters running
 * simultaneously (which is always at most 2 -- "the one I'm reading" and "the one I'm
 * painting"), no matter how long the stack grows.
 */
class
PostProcessSystem {
public:
  /** @brief Default constructor. */
  PostProcessSystem() = default;

  /** @brief Default destructor. */
  ~PostProcessSystem() = default;

  /**
   * @brief Compiles every stage's shader, creates the required GPU states, and allocates
   * the internal ping-pong scratch buffers used to chain multiple stages.
   * @param device The D3D11 device used to create resources.
   * @param width Initial width of the scratch buffers. Should match the editor viewport size.
   * @param height Initial height of the scratch buffers. Should match the editor viewport size.
   * @return HRESULT S_OK on success.
   */
  HRESULT
  init(Device& device, unsigned int width, unsigned int height);

  /**
   * @brief Re-allocates the internal ping-pong scratch buffers to match a new viewport size.
   * @note Must be called whenever the caller resizes its own `EditorViewportPass` instances
   * (e.g. from `BaseApp::handleEditorViewportResize`), or intermediate stages will sample/
   * write at the wrong resolution.
   * @param device The D3D11 device.
   * @param width New width in pixels.
   * @param height New height in pixels.
   */
  void
  resize(Device& device, unsigned int width, unsigned int height);

  /**
   * @brief Runs every enabled effect stage, in stack order, in a single call.
   * @param deviceContext The device context used to issue rendering commands.
   * @param source The pass containing the SRV of the previously rendered scene.
   * @param destination The pass whose RTV will receive the final processed image.
   * @param camera The active camera; its world position drives the depth-fog distance calculation.
   * @param worldPositionAOSRV The deferred G-Buffer's world-position/AO SRV
   * (`RenderPipeline::getGBufferWorldAoSRV()`). May be `nullptr` (e.g. Forward renderer
   * active); Fog and SSAO are automatically skipped for this frame when it is, regardless
   * of their checkbox state.
   * @param normalRoughnessSRV The deferred G-Buffer's normal/roughness SRV
   * (`RenderPipeline::getGBufferNormalRoughnessSRV()`). Only consumed by SSAO. May be
   * `nullptr`; SSAO is skipped for this frame when it is.
   * @note `source` and `destination` must be different `EditorViewportPass` instances.
   * Internally, stages before the last one write into the system's own scratch buffers
   * instead of `destination` -- only the final enabled stage writes into `destination`.
   */
  void
  render(DeviceContext& deviceContext,
         EditorViewportPass& source,
         EditorViewportPass& destination,
         const Camera& camera,
         ID3D11ShaderResourceView* worldPositionAOSRV,
         ID3D11ShaderResourceView* normalRoughnessSRV = nullptr);

  /** @brief Releases all GPU resources owned by this system, including the scratch buffers. */
  void
  destroy();

public:
  /** @brief Runtime toggles for the available post-processing effects. */
  PostProcessSettings m_settings;

private:
  //--------------------------------------------------------------------------------------
  // Stage implementations
  // Context: Each stage has the exact same signature so the orchestrator in render() can
  // hold them as plain member-function pointers and iterate over the stack generically.
  //--------------------------------------------------------------------------------------

  /** @brief Stage 1: Grayscale + Depth Fog + Vignette, combined in a single shader pass. */
  void
  renderColorEffectsPass(DeviceContext& deviceContext,
                         EditorViewportPass& source,
                         EditorViewportPass& destination,
                         const Camera& camera,
                         ID3D11ShaderResourceView* worldPositionAOSRV,
                         ID3D11ShaderResourceView* normalRoughnessSRV);

  /**
   * @brief Stage 2: Bloom -- soft-threshold bright-pass extraction blurred and added back
   * onto the source image in a single pass (see Bloom.hlsl).
   */
  void
  renderBloomPass(DeviceContext& deviceContext,
                  EditorViewportPass& source,
                  EditorViewportPass& destination,
                  const Camera& camera,
                  ID3D11ShaderResourceView* worldPositionAOSRV,
                  ID3D11ShaderResourceView* normalRoughnessSRV);

  /**
   * @brief Stage 3: Tone Mapping -- exposure multiply + ACES filmic curve (see ToneMapping.hlsl).
   */
  void
  renderToneMappingPass(DeviceContext& deviceContext,
                        EditorViewportPass& source,
                        EditorViewportPass& destination,
                        const Camera& camera,
                        ID3D11ShaderResourceView* worldPositionAOSRV,
                        ID3D11ShaderResourceView* normalRoughnessSRV);

  /**
   * @brief Stage 4: FXAA + SSAO, combined in a single shader pass (see FXAA_SSAO.hlsl).
   * @note SSAO needs both worldPositionAOSRV and normalRoughnessSRV; when either is null
   * (e.g. Forward renderer active, no G-Buffer) SSAO is silently skipped for the frame.
   */
  void
  renderFXAAPass(DeviceContext& deviceContext,
                 EditorViewportPass& source,
                 EditorViewportPass& destination,
                 const Camera& camera,
                 ID3D11ShaderResourceView* worldPositionAOSRV,
                 ID3D11ShaderResourceView* normalRoughnessSRV);

  /** @brief Grows/shrinks the internal ping-pong scratch buffers if they no longer match. */
  void
  ensureScratchSize(Device& device, unsigned int width, unsigned int height);

private:
  //--------------------------------------------------------------------------------------
  // Shared full-screen-triangle pipeline state
  // Context: Every stage draws the exact same procedurally-generated full-screen triangle
  // (see any stage's .hlsl VS()), so the raster/depth-stencil/sampler states are shared
  // rather than duplicated per stage.
  //--------------------------------------------------------------------------------------
  SamplerState m_sampler;
  RasterizerState m_rasterizerState;
  DepthStencilState m_depthStencilState;

  // --- Stage 1: Color Effects (Grayscale + Depth Fog + Vignette) ---
  ShaderProgram m_colorEffectsShader;
  Buffer m_colorEffectsConstantBuffer;

  // --- Stage 2: Bloom ---
  ShaderProgram m_bloomShader;
  Buffer m_bloomConstantBuffer;

  // --- Stage 3: Tone Mapping ---
  ShaderProgram m_toneMappingShader;
  Buffer m_toneMappingConstantBuffer;

  // --- Stage 4: FXAA / SSAO ---
  ShaderProgram m_fxaaSsaoShader;
  Buffer m_fxaaSsaoConstantBuffer;

  //--------------------------------------------------------------------------------------
  // Ping-pong scratch buffers
  // Context: Owned internally (rather than by BaseApp) so the number of buffers the caller
  // has to manage never grows as more stages are added to the stack -- it's always exactly
  // "source" + "destination", regardless of whether 1 stage or 4 stages end up running.
  //--------------------------------------------------------------------------------------
  EditorViewportPass m_scratchA;
  EditorViewportPass m_scratchB;

  /** @brief Current allocated size of m_scratchA/m_scratchB, used by ensureScratchSize(). */
  unsigned int m_scratchWidth = 0;
  unsigned int m_scratchHeight = 0;
};