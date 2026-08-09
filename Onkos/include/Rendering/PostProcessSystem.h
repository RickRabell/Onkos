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
};

/**
 * @brief CPU-side mirror of the constant buffer consumed by GrayScalePostProcess.hlsl.
 */
struct
CBPostProcess {
  int enableGrayscale = 0;
  float padding[3] = { 0.0f, 0.0f, 0.0f };
};

/**
 * @class PostProcessSystem
 * @brief Applies full-screen post-processing effects to an already-rendered scene texture.
 * @author Ricardo Rabell
 * @date 2026-05-01
 *
 * @details
 * **Context:** Once the 3D scene has been rendered into an off-screen texture (via
 * `EditorViewportPass`), we often want to apply effects that work on the final 2D image
 * rather than the 3D geometry (grayscale, bloom, color grading, etc).
 * * **Why it exists:** Rather than mixing this logic into the main render pipeline, this
 * system owns its own full-screen-triangle shader and state objects, and exposes a single
 * `render()` entry point that reads from one `EditorViewportPass` and writes into another,
 * enabling ping-pong style post-processing chains.
 * * **Analogy:** Think of this as an Instagram-style filter applied after the photo (the 3D
 * render) has already been taken. The photo itself never changes; a new copy is produced
 * with the filter baked in.
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
   * @brief Applies the configured post-processing effects to the source texture.
   * @param deviceContext The device context used to issue rendering commands.
   * @param source The pass containing the SRV of the previously rendered scene.
   * @param destination The pass whose RTV will receive the processed image.
   * @note `source` and `destination` must be different `EditorViewportPass` instances.
   */
  void
  render(DeviceContext& deviceContext, EditorViewportPass& source, EditorViewportPass& destination);

  /** @brief Releases all GPU resources owned by this system. */
  void
  destroy();

public:
  /** @brief Runtime toggles for the available post-processing effects. */
  PostProcessSettings m_settings;

private:
  /** @brief The full-screen quad vertex/pixel shader program. */
  ShaderProgram m_shaderProgram;

  /** @brief The constant buffer holding the effect toggles (CBPostProcess). */
  Buffer m_constantBuffer;

  /** @brief Sampler state used to sample the source scene texture. */
  SamplerState m_sampler;

  /** @brief Rasterizer state used while drawing the full-screen triangle. */
  RasterizerState m_rasterizerState;

  /** @brief Depth-stencil state with depth testing disabled (post-processing is 2D). */
  DepthStencilState m_depthStencilState;
};
