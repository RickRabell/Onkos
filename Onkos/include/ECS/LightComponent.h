#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"
#include "Rendering/RenderTypes.h"

// Forward Declaration
class DeviceContext;

/**
 * @class LightComponent
 * @brief Marks an entity as a light source in the scene and holds its physical lighting properties.
 * @author Ricardo Rabell
 * @date 2026-05-13
 *
 * @details
 * **Context:** In an Entity-Component System, an Entity is just an ID. To make an Entity emit
 * light, you attach this component to it. A separate "Lighting System" or the "Renderer"
 * will iterate over all entities that have a LightComponent, extract their `LightData`,
 * and pass that data to the GPU's constant buffers.
 * * **Why it exists (and why methods are empty):** You'll notice `update()` and `render()`
 * are empty. This is by design! A light doesn't "draw" itself. It is pure data. The spatial
 * position and rotation of the light come from the Entity's `Transform` component, while
 * this component provides the color, intensity, and type.
 * * **Analogy:** Think of the `Transform` component as the physical lamp stand in a room,
 * telling you where it is and where it points. The `LightComponent` is the lightbulb you
 * screw into the stand, defining whether it's a 60W warm white bulb or a red laser.
 * * @note The default constructor currently uses `ComponentType::NONE`. You may want to
 * update this to `ComponentType::LIGHT` to allow your ECS to query light components correctly.
 */
class 
LightComponent : public Component {
public:

	/** @brief Default constructor initializing the base component. */
	LightComponent()
		: Component(ComponentType::NONE) {
	}

	/** @brief Lifecycle init. Empty because lights are purely data-driven. */
	void 
	init() override {}

	/** @brief Lifecycle update. Empty because light data typically only changes via external scripts. */
	void 
	update(float deltaTime) override {}

	/** @brief Lifecycle render. Empty because lights don't draw geometry themselves. */
	void 
	render(DeviceContext& deviceContext) override {}

	/** @brief Lifecycle destroy. */
	void 
	destroy() override {}

	/**
	 * @brief Gets a mutable reference to the underlying light physics data.
	 * @return LightData&
	 */
	LightData& 
	getLightData() { return m_light; }

	/**
	 * @brief Gets a read-only reference to the underlying light physics data.
	 * @return const LightData&
	 */
	const LightData& 
	getLightData() const { return m_light; }

	/**
	 * @brief Toggles whether this light source generates shadow maps.
	 * @param value True to enable shadow casting.
	 */
	void 
	setCastShadow(bool value) { m_castShadow = value; }

	/**
	 * @brief Checks if this light is configured to cast shadows.
	 * @return bool True if shadows are enabled.
	 */
	bool 
	canCastShadow() const { return m_castShadow; }

private:
	/** @brief The raw physics data (color, intensity, type) mapped to the GPU struct. */
	LightData m_light;
	/** @brief Determines if the renderer should allocate a shadow map for this light. */
	bool m_castShadow = false;
};