#pragma once
#include "Prerequisites.h"
#include "Entity.h"
#include "Buffer.h"
#include "Texture.h"
#include "Transform.h"
#include "SamplerState.h"
//#include "Rasterizer.h"
//#include "BlendState.h"
#include "ShaderProgram.h"
//#include "DepthStencilState.h"

// Forward declarations
class Device;
class DeviceContext;
class MeshComponent;

/**
 * @class Actor
 * @brief Represents a renderable entity in the game world.
 * @author Ricardo Rabell
 * @date 2025-11-27
 *
 * The Actor class inherits from Entity (providing position/rotation/scale) and
 * adds visual capabilities. It manages one or more meshes, materials (textures),
 * and the corresponding GPU buffers (Vertex/Index). It also handles its own
 * rendering logic, including shadow mapping support.
 */
class 
Actor : public Entity {
public:
  /**
   * @brief Default constructor.
   */
  Actor() = default;

  /**
   * @brief Constructor utilizing a device for immediate initialization (optional).
   * @param device The graphics device.
   */
  Actor(Device& device);

  /**
   * @brief Virtual destructor.
   */
  virtual 
  ~Actor() = default;

  void
  awake() override {};

  /**
   * @brief Initializes the actor.
   * @note Override from Entity. Implementation is currently empty as initialization usually happens via setMesh.
   */
  void
  init() override {}

  /**
   * @brief Updates the actor logic per frame.
   * @param deltaTime Time elapsed since the last frame.
   * @param deviceContext The device context (used if buffers need updating).
   */
  void
  update(float deltaTime, DeviceContext& deviceContext) override;

  /**
   * @brief Renders the actor to the current render target.
   * Sets vertex/index buffers, updates constant buffers with the World matrix, and draws the geometry.
   * @param deviceContext The device context used for drawing.
   */
  void
  render(DeviceContext& deviceContext) override;

  /**
   * @brief Releases GPU resources (buffers, textures).
   */
  void
  destroy();

  /**
   * @brief Assigns mesh data to the actor and creates the necessary GPU buffers.
   * @param device The graphics device used to create vertex/index buffers.
   * @param meshes A vector of MeshComponents containing geometry data.
   */
  void
  setMesh(Device& device, std::vector<MeshComponent> meshes);

  /**
   * @brief Gets the name of the actor.
   * @return std::string The name.
   */
  std::string
  getName() { return m_name; }

  /**
   * @brief Sets the name of the actor.
   * @param name The new name.
   */
  void
  setName(const std::string& name) { m_name = name; }

  /**
   * @brief Sets the textures (materials) for the actor.
   * @param textures A vector of initialized Texture objects.
   */
  void
    setTextures(std::vector<Texture> textures) { m_textures = textures; }

  /**
   * @brief Enables or disables shadow casting for this actor.
   * @param v True to cast shadows, false otherwise.
   */
  void
  setCastShadow(bool v) { castShadow = v; }

  /**
   * @brief Checks if the actor casts shadows.
   * @return bool True if shadows are enabled.
   */
  bool
  canCastShadow() const { return castShadow; }

  /**
   * @brief Renders the actor specifically for the shadow map pass.
   * Uses a specific shader/technique to write only depth data.
   * @param deviceContext The device context used for drawing.
   */
  void
  renderShadow(DeviceContext& deviceContext);

private:
  /** @brief CPU-side mesh data. */
  std::vector<MeshComponent> m_meshes;

  /** @brief Textures applied to the meshes. */
  std::vector<Texture> m_textures;

  /** @brief GPU vertex buffers (one per mesh). */
  std::vector<Buffer> m_vertexBuffers;

  /** @brief GPU index buffers (one per mesh). */
  std::vector<Buffer> m_indexBuffers;

  // Render States (Commented out in source, presumably for future implementation)
  //BlendState m_blendstate;
  //Rasterizer m_rasterizer;
  //BlendState m_shadowBlendState;
  //DepthStencilState m_shadowDepthStencilState;

  /** @brief Sampler state for texture sampling. */
  SamplerState m_sampler;

  /** @brief Constant buffer structure for per-frame data (World Matrix). */
  CBChangesEveryFrame m_model;

  /** @brief GPU constant buffer for the model data. */
  Buffer m_modelBuffer;

  // Shadow Mapping Resources

  /** @brief Shader program used for rendering the shadow map depth. */
  ShaderProgram m_shaderShadow;

  /** @brief Constant buffer specifically for the shadow pass. */
  Buffer m_shaderBuffer;

  /** @brief Data structure for the shadow constant buffer. */
  CBChangesEveryFrame m_cbShadow;

  /** @brief Position of the light source (used for shadow calculations). */
  XMFLOAT4 m_LightPos;

  /** @brief Identifier name for the actor. */
  std::string m_name = "Actor";

  /** @brief Flag determining if this object contributes to shadow maps. */
  bool castShadow = true;
};