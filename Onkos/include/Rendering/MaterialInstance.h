#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

// Forward Declarations
class Material;
class DeviceContext;
class Texture;

/**
 * @class MaterialInstance
 * @brief An instanced variation of a Material, containing specific textures and parameters.
 * @author Ricardo Rabell
 * @date 2026-04-25
 *
 * @details
 * **Context:** This class holds the actual image data (Albedo, Normal maps) and
 * specific float parameters (like a base roughness multiplier) that are fed into
 * the parent `Material`'s shader.
 * * **Why it exists:** Memory efficiency. If you have 10 wooden crates and 10 metal barrels,
 * you only need 1 `Material` in memory (the PBR shader). You create 2 `MaterialInstance`s:
 * one pointing to the wood textures, one to the metal textures.
 * * **Analogy:** If `Material` is the recipe, `MaterialInstance` is the actual baked cake.
 * You can bake a chocolate cake (metal crate) and a vanilla cake (wood crate) using the
 * exact same underlying baking mechanics (the Material).
 */
class 
MaterialInstance {
public:
	// --- Setters ---
	/**
	 * @brief Sets the parent Material for this instance.
	 * @param material Pointer to the Material to use as the base shader/ruleset.
	 */
	void 
	setMaterial(Material* material) { m_material = material; }

	/**
	 * @brief Sets the Albedo (base color) texture.
	 * @param texture Pointer to the Texture to use as the Albedo map.
	 */
	void 
	setAlbedo(Texture* texture) { m_albedo = texture; }

	/**
	 * @brief Sets the Normal map texture.
	 * @param texture Pointer to the Texture to use as the Normal map.
	 */
	void 
	setNormal(Texture* texture) { m_normal = texture; }

	/**
	 * @brief Sets the Metallic map texture.
	 * @param texture Pointer to the Texture to use as the Metallic map.
	 */
	void 
	setMetallic(Texture* texture) { m_metallic = texture; }

	/**
	 * @brief Sets the Roughness map texture.
	 * @param texture Pointer to the Texture to use as the Roughness map.
	 */
	void 
	setRoughness(Texture* texture) { m_roughness = texture; }

	/**
	 * @brief Sets the Ambient Occlusion (AO) map texture.
	 * @param texture Pointer to the Texture to use as the AO map.
	 */
	void 
	setAO(Texture* texture) { m_ao = texture; }

	/**
	 * @brief Sets the Emissive (glow) map texture.
	 * @param texture Pointer to the Texture to use as the Emissive map.
	 */
	void 
	setEmissive(Texture* texture) { m_emissive = texture; }

	// --- Getters ---

	/**
	 * @brief Gets the parent Material of this instance.
	 * @return Pointer to the parent Material.
	 */
	Material* 
	getMaterial() const { return m_material; }

	/**
	 * @brief Gets the Albedo (base color) texture.
	 * @return Pointer to the Albedo Texture.
	 */
	Texture* 
	getAlbedo() const { return m_albedo; }

	/**
	 * @brief Gets the Normal map texture.
	 * @return Pointer to the Normal Texture.
	 */
	Texture* 
	getNormal() const { return m_normal; }

	/**
	 * @brief Gets the Metallic map texture.
	 * @return Pointer to the Metallic Texture.
	 */
	Texture* 
	getMetallic() const { return m_metallic; }

	/**
	 * @brief Gets the Roughness map texture.
	 * @return Pointer to the Roughness Texture.
	 */
	Texture* 
	getRoughness() const { return m_roughness; }

	/**
	 * @brief Gets the Ambient Occlusion (AO) map texture.
	 * @return Pointer to the AO Texture.
	 */
	Texture* 
	getAO() const { return m_ao; }

	/**
	 * @brief Gets the Emissive (glow) map texture.
	 * @return Pointer to the Emissive Texture.
	 */
	Texture* 
	getEmissive() const { return m_emissive; }

	/** @brief Gets a mutable reference to numerical parameters (colors, scalars) for animation/logic. */
	MaterialParams& 
	getParams() { return m_params; }

	/** @brief Gets a read-only reference to the numerical parameters. */
	const MaterialParams& 
	getParams() const { return m_params; }

	/**
	 * @brief Binds all the assigned textures to the GPU pixel shader slots.
	 * @param deviceContext The D3D11 device context.
	 */
	void 
	bindTextures(DeviceContext& deviceContext) const;

private:
	/** 
	 * @brief Pointer to the parent Material ruleset.
	 * @details This points to the shared Material (shader and logic) that this instance uses.
	 */
	Material* m_material = nullptr;      

	/** 
	 * @brief Base color (Albedo) texture map.
	 * @details Contains the main color information for the surface.
	 */
	Texture* m_albedo = nullptr;         

	/** 
	 * @brief Surface detail (Normal) map.
	 * @details Encodes per-pixel surface normals for lighting calculations.
	 */
	Texture* m_normal = nullptr;         

	/** 
	 * @brief Metalness texture map.
	 * @details Black/white map indicating metallic (white) or dielectric (black) areas.
	 */
	Texture* m_metallic = nullptr;       

	/** 
	 * @brief Micro-surface roughness texture map.
	 * @details Controls the sharpness or blurriness of reflections.
	 */
	Texture* m_roughness = nullptr;      

	/** 
	 * @brief Ambient Occlusion (AO) texture map.
	 * @details Adds crevice shadowing for enhanced realism.
	 */
	Texture* m_ao = nullptr;             

	/** 
	 * @brief Emissive (glow) texture map.
	 * @details Controls self-illumination for glowing effects.
	 */
	Texture* m_emissive = nullptr;       

	/** 
	 * @brief CPU-side struct for float/vector parameters.
	 * @details Holds numerical parameters (colors, scalars) for this instance.
	 */
	MaterialParams m_params;             
};