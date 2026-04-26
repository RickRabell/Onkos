#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

// Forward Declarations
class ShaderProgram;
class RasterizerState;
class DepthStencilState;
class SamplerState;

/**
 * @class Material
 * @brief Defines the visual rendering "rules" and shader logic for a surface.
 * @author Ricardo Rabell
 * @date 2026-04-25
 *
 * @details
 * **Context:** A Material holds pointers to the Shader, Blend State, and Rasterizer State.
 * It DOES NOT hold specific textures (like a brick wall image).
 * * **Why it exists:** State changes on the GPU are expensive. By separating the `Material`
 * (the rules) from the `MaterialInstance` (the textures), the Renderer can sort objects
 * by Material. It binds the "Standard PBR" shader once, and then rapidly draws 100
 * different objects just by swapping their textures, drastically improving performance.
 * * **Analogy:** `Material` is the recipe for a cake (bake at 350 degrees, use flour).
 * It defines the rules, but you can't eat a recipe.
 */
class 
Material {
public:
	// --- Setters ---
	/**
	 * @brief Sets the shader program used by this material.
	 * @param shader Pointer to the ShaderProgram to assign.
	 */
	void 
	setShader(ShaderProgram* shader) { m_shader = shader; }

	/**
	 * @brief Sets the rasterizer state for this material.
	 * @param state Pointer to the RasterizerState to assign.
	 */
	void 
	setRasterizerState(RasterizerState* state) { m_rasterizerState = state; }

	/**
	 * @brief Sets the depth-stencil state for this material.
	 * @param state Pointer to the DepthStencilState to assign.
	 */
	void 
	setDepthStencilState(DepthStencilState* state) { m_depthStencilState = state; }

	/**
	 * @brief Sets the sampler state for this material.
	 * @param state Pointer to the SamplerState to assign.
	 */
	void 
	setSamplerState(SamplerState* state) { m_samplerState = state; }

	/**
	 * @brief Sets the material domain (render queue) for this material.
	 * @param domain The MaterialDomain to assign.
	 */
	void 
	setDomain(MaterialDomain domain) { m_domain = domain; }

	/**
	 * @brief Sets the blend mode for this material.
	 * @param blendMode The BlendMode to assign.
	 */
	void 
	setBlendMode(BlendMode blendMode) { m_blendMode = blendMode; }

	// --- Getters ---

	/**
	 * @brief Gets the shader program used by this material.
	 * @return Pointer to the ShaderProgram.
	 */
	ShaderProgram* 
	getShader() const { return m_shader; }

	/**
	 * @brief Gets the rasterizer state for this material.
	 * @return Pointer to the RasterizerState.
	 */
	RasterizerState* 
	getRasterizerState() const { return m_rasterizerState; }

	/**
	 * @brief Gets the depth-stencil state for this material.
	 * @return Pointer to the DepthStencilState.
	 */
	DepthStencilState* 
	getDepthStencilState() const { return m_depthStencilState; }

	/**
	 * @brief Gets the sampler state for this material.
	 * @return Pointer to the SamplerState.
	 */
	SamplerState* 
	getSamplerState() const { return m_samplerState; }

	/**
	 * @brief Gets the material domain (render queue) for this material.
	 * @return The MaterialDomain value.
	 */
	MaterialDomain 
	getDomain() const { return m_domain; }

	/**
	 * @brief Gets the blend mode for this material.
	 * @return The BlendMode value.
	 */
	BlendMode 
	getBlendMode() const { return m_blendMode; }

private:
	/**
	 * @brief The HLSL shader program used by this material.
	 */
	ShaderProgram* m_shader = nullptr;

	/**
	 * @brief The rasterizer state, handles culling (e.g., solid vs wireframe).
	 */
	RasterizerState* m_rasterizerState = nullptr;

	/**
	 * @brief The depth-stencil state, handles depth testing rules.
	 */
	DepthStencilState* m_depthStencilState = nullptr;

	/**
	 * @brief The sampler state, handles texture filtering (e.g., Linear, Point).
	 */
	SamplerState* m_samplerState = nullptr;

	/**
	 * @brief The material domain, defines which render queue this belongs to.
	 */
	MaterialDomain m_domain = MaterialDomain::Opaque;

	/**
	 * @brief The blend mode, defines math for combining colors (Alpha, Additive).
	 */
	BlendMode m_blendMode = BlendMode::Opaque;
};