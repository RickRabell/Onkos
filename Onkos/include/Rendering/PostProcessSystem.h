#pragma once
#include "Prerequisites.h"
#include "ShaderProgram.h"
#include "Buffer.h"
#include "SamplerState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "EngineUtilities\Utilities\LayoutBuilder.h"

// Forward declarations
class Device;
class DeviceContext;
class EditorViewportPass;
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
 */
struct
PostProcessSettings {
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

  /**
   * @brief Whether any effect is currently enabled.
   * Context: Used both to decide if the post-process pass should run at all, and to
   * pick which EditorViewportPass SRV to hand to the GUI viewport panel when none are.
   */
  bool
  hasActiveEffects() const {
    return enableGrayscale || enableFog || enableVignette;
  }
};

/**
 * @brief CPU-side mirror of the constant buffer consumed by PostProcess.hlsl.
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
 * @class PostProcessSystem
 * @brief Applies full-screen post-processing effects to an already-rendered scene texture.
 * @author Ricardo Rabell
 * @date 2026-05-01
 *
 * @details
 * **Context:** Once the 3D scene has been rendered into an off-screen texture (via
 * `EditorViewportPass`), we often want to apply effects that work on the final 2D image
 * rather than the 3D geometry (grayscale, depth fog, vignette, etc).
 * * **Why it exists:** Rather than mixing this logic into the main render pipeline, this
 * system owns its own full-screen-triangle shader and state objects, and exposes a single
 * `render()` entry point that reads from one `EditorViewportPass` and writes into another.
 * * **Why one unified pass instead of ping-ponging per effect:** Grayscale, Depth Fog and
 * Vignette all read the same input pixel and write the same output pixel independently --
 * there's no reason to pay for extra render-target swaps and extra full-screen draws when
 * one pixel shader can blend all three in a single pass. Fewer render-target transitions
 * means fewer chances for the OM stage to be left in a bad state between frames.
 * * **Analogy:** Think of this as an Instagram-style filter applied after the photo (the 3D
 * render) has already been taken. The photo itself never changes; a new copy is produced
 * with every enabled filter baked in at once.
 */
class
PostProcessSystem {
public:
  /** @brief Default constructor. */
  PostProcessSystem() = default;

  /** @brief Default destructor. */
  ~PostProcessSystem() = default;

  /**
   * @brief Compiles the full-screen quad shader and creates the required GPU states.
   * @param device The D3D11 device used to create resources.
   * @return HRESULT S_OK on success.
   */
  HRESULT
  init(Device& device);

  /**
   * @brief Applies every enabled effect to the source texture in a single full-screen pass.
   * @param deviceContext The device context used to issue rendering commands.
   * @param source The pass containing the SRV of the previously rendered scene.
   * @param destination The pass whose RTV will receive the processed image.
   * @param camera The active camera; its world position drives the depth-fog distance calculation.
   * @param worldPositionAOSRV The deferred G-Buffer's world-position/AO SRV
   * (`RenderPipeline::getGBufferWorldAoSRV()`). May be `nullptr` (e.g. Forward renderer
   * active); fog is automatically skipped for this pass when it is, regardless of the
   * checkbox state.
   * @note `source` and `destination` must be different `EditorViewportPass` instances.
   */
  void
  render(DeviceContext& deviceContext,
         EditorViewportPass& source,
         EditorViewportPass& destination,
         const Camera& camera,
         ID3D11ShaderResourceView* worldPositionAOSRV);

  /** @brief Releases all GPU resources owned by this system. */
  void
  destroy();

public:
  /** @brief Runtime toggles for the available post-processing effects. */
  PostProcessSettings m_settings;

private:
  /** @brief The full-screen quad vertex/pixel shader program. */
  ShaderProgram m_shaderProgram;

  /** @brief The constant buffer holding the effect toggles and parameters (CBPostProcess). */
  Buffer m_constantBuffer;

  /** @brief Sampler state used to sample the source scene texture and G-Buffer. */
  SamplerState m_sampler;

  /** @brief Rasterizer state used while drawing the full-screen triangle (cull none). */
  RasterizerState m_rasterizerState;

  /** @brief Depth-stencil state with depth testing disabled (post-processing is 2D). */
  DepthStencilState m_depthStencilState;
};