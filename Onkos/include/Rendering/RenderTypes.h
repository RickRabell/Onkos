#pragma once
#include "Prerequisites.h"

// Forward Declarations
class Mesh;
class MaterialInstance;

/** @enum MaterialDomain
 * @brief Classifies how a material interacts with the depth buffer and sorting queues.
 */
enum class 
MaterialDomain {
	Opaque = 0,    ///< Fully solid. Writes to depth buffer.
	Masked,        ///< Solid but with cutout holes (e.g., chainlink fence). Uses Alpha-to-Coverage or clip().
	Transparent    ///< Partially see-through. Requires back-to-front sorting.
};

/** @enum BlendMode
 * @brief Defines the math equation used by the Output Merger to blend pixel colors.
 */
enum class 
BlendMode {
	Opaque = 0,             ///< Overwrites existing pixel (Src * 1 + Dest * 0)
	Alpha,                  ///< Standard transparency (Src * Alpha + Dest * (1 - Alpha))
	Additive,               ///< Used for fire/magic (Src * 1 + Dest * 1)
	PremultipliedAlpha      ///< Advanced blending where RGB is pre-multiplied by Alpha.
};

/** @enum RenderPassType
 * @brief Identifies the current stage of the rendering pipeline.
 */
enum class 
RenderPassType {
	Shadow = 0,    ///< Drawing to the depth-only shadow map.
	Opaque,        ///< Drawing solid geometry.
	Skybox,        ///< Drawing the background environment.
	Transparent,   ///< Drawing glass/water.
	Editor         ///< Drawing gizmos, grid, and UI elements.
};

/** @enum LightType */
enum class 
LightType {
	Directional = 0, ///< Infinite parallel rays (like the Sun).
	Point,           ///< Emits light in all directions from a point (like a lightbulb).
	Spot             ///< Emits a cone of light (like a flashlight).
};

/**
 * @struct LightData
 * @brief Contains physical properties of a light source.
 */
struct 
LightData {
	LightType type = LightType::Directional;
	EU::Vector3 color = EU::Vector3(1.0f, 1.0f, 1.0f);
	float intensity = 1.0f;

	EU::Vector3 direction = EU::Vector3(0.0f, -1.0f, 0.0f);
	float range = 0.0f;

	EU::Vector3 position = EU::Vector3(0.0f, 0.0f, 0.0f);
	float spotAngle = 0.0f;
};

/**
 * @struct MaterialParams
 * @brief CPU-side representation of a material's numeric properties.
 */
struct 
MaterialParams {
	XMFLOAT4 baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	float metallic = 1.0f;
	float roughness = 1.0f;
	float ao = 1.0f;
	float normalScale = 1.0f;
	float emissiveStrength = 1.0f;
	float alphaCutoff = 0.5f;
};

//======================================================================================
// CONSTANT BUFFER STRUCTURES
// Context: These structs are copied directly into GPU memory. DirectX 11 enforces strict 
// 16-byte (float4) alignment rules for HLSL Constant Buffers. 
// Why the 'pad' variables exist: If a struct doesn't naturally end on a 16-byte boundary, 
// the GPU will silently misalign the data, causing graphical glitches. The 'pad' floats 
// force the C++ struct to match the HLSL memory layout perfectly.
//======================================================================================

/** @struct CBPerFrame
 * @brief Data updated exactly once per frame (Cameras, Environment, Lights).
 */
struct 
CBPerFrame {
	XMFLOAT4X4 View{};
	XMFLOAT4X4 Projection{};
	XMFLOAT4X4 LightViewProjection{};

	EU::Vector3 CameraPos{};
	float pad0 = 0.0f; // Pads CameraPos (12 bytes) to 16 bytes.

	EU::Vector3 LightDir = EU::Vector3(0.0f, -1.0f, 0.0f);
	float pad1 = 0.0f; // Pads LightDir (12 bytes) to 16 bytes.

	EU::Vector3 LightColor = EU::Vector3(1.0f, 1.0f, 1.0f);
	float pad2 = 0.0f; // Pads LightColor (12 bytes) to 16 bytes.
};

/** @struct CBPerObject
 * @brief Data updated for every single draw call (Transforms).
 */
struct 
CBPerObject {
	XMFLOAT4X4 World{}; // 64 bytes (exactly 4x 16-byte chunks). No padding needed.
};

/** @struct CBPerMaterial
 * @brief Data updated whenever the renderer switches to a different material instance.
 */
struct 
CBPerMaterial {
	XMFLOAT4 BaseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // 16 bytes

	// Next 6 floats = 24 bytes. 
	float Metallic = 1.0f;
	float Roughness = 1.0f;
	float AO = 1.0f;
	float NormalScale = 1.0f;
	float EmissiveStrength = 1.0f;
	float AlphaCutoff = 0.0f;

	// Padding to push the total struct size to the next multiple of 16 bytes.
	// 16 + 24 = 40 bytes. We need 48 bytes (16 * 3). So we add 8 bytes (2 floats).
	// Note: The code currently has 6 pad floats (24 bytes). This might be over-padding, 
	// but it is safe as long as it matches the HLSL struct identically.
	float pad0 = 0.0f;
	float pad1 = 0.0f;
	float pad2 = 0.0f;
	float pad3 = 0.0f;
	float pad4 = 0.0f;
	float pad5 = 0.0f;
};

/**
 * @struct RenderObject
 * @brief A complete instruction set for drawing one piece of geometry.
 * @details This is the payload passed from the ECS to the `RenderScene`.
 */
struct 
RenderObject {
	Mesh* mesh = nullptr;                               ///< The geometry to draw.
	MaterialInstance* materialInstance = nullptr;       ///< Legacy/Single material pointer.
	std::vector<MaterialInstance*> materialInstances;   ///< Array mapping materials to Submeshes.
	XMMATRIX world = XMMatrixIdentity();                ///< Its position in the world.
	bool castShadow = true;                             ///< Does it block light?
	bool transparent = false;                           ///< Should it go to the Transparent queue?
	float distanceToCamera = 0.0f;                      ///< Pre-calculated distance used for back-to-front sorting.
};