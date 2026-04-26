#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

// Forward Declaration
class Skybox;

/**
 * @class RenderScene
 * @brief A data transfer object (DTO) containing a snapshot of all visible objects for a single frame.
 * @author Ricardo Rabell
 * @date 2026-04-25
 *
 * @details
 * **Context:** In a modern engine, the Game Logic (ECS) and the Renderer should be decoupled.
 * The ECS updates positions, animations, and physics. Once the frame is updated, it extracts
 * all the visible data and packages it into this `RenderScene` object, handing it off to the `ForwardRenderer`.
 * * **Why it exists:** This prevents the Renderer from having to know what an "Actor" or "Entity" is.
 * It only cares about raw geometry (`RenderObject`), lights, and the skybox. This separation makes
 * multi-threading much easier in the future (Logic thread builds the scene, Render thread draws it).
 * * **Analogy:** The ECS is the movie director setting up the actors on stage. The `RenderScene`
 * is a photograph of that setup. The Renderer is the painter who looks ONLY at the photograph
 * to paint the final canvas.
 */
class 
RenderScene {
public:
	/**
	 * @brief Clears all object and light lists to prepare for the next frame's data.
	 */
	void
	clear();

public:
	/** @brief List of solid objects that block light. Sorted front-to-back by the renderer. */
	std::vector<RenderObject> opaqueObjects;

	/** @brief List of objects with transparency (glass, water). Sorted back-to-front by the renderer. */
	std::vector<RenderObject> transparentObjects;

	/** @brief List of active lights affecting the scene. */
	std::vector<LightData> directionalLights;

	/** @brief Pointer to the active skybox environment, if any. */
	Skybox* skybox = nullptr;
};