#pragma once
#include "Prerequisites.h"
#include "EngineUtilities/Vectors/Vector3.h"
#include "Component.h"

/**
 * @class Transform
 * @brief Manages the position, rotation, and scale of an entity.
 * @author Ricardo Rabell
 * @date 2025-11-27
 *
 * The Transform component is fundamental for 3D rendering. It stores spatial
 * data and computes the World Matrix (SRT Matrix) used to position the
 * entity in the 3D world relative to the origin.
 */
class 
Transform : public Component {
public:
	/**
	 * @brief Default constructor.
	 * Initializes position to (0,0,0), rotation to (0,0,0), and scale to (0,0,0).
	 * Sets the component type to TRANSFORM.
	 */
	Transform() : position(),
								rotation(),
								scale(),
								matrix(),
								worldMatrix(),
								Component(ComponentType::TRANSFORM) { }

	/**
	 * @brief Initializes the transform.
	 * Sets scale to (1,1,1) and resets the matrix to Identity.
	 */
	void
	init() {
		scale.one();
		matrix = XMMatrixIdentity();
		worldMatrix = XMMatrixIdentity();
	}

	/**
	 * @brief Updates the World Matrix based on current position, rotation, and scale.
	 * @note The matrix is calculated in the order: Scale -> Rotation -> Translation.
	 * @param deltaTime Time elapsed since the last frame.
	 */
	void
	update(float deltaTime) override {
		// Apply scaling
		XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, 
																					 scale.y, 
																					 scale.z);
		// Apply rotation (Roll, Pitch, Yaw)
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, 
																													 rotation.y, 
																													 rotation.z);
		// Apply translation
		XMMATRIX translationMatrix = XMMatrixTranslation(position.x, 
																										 position.y, 
																										 position.z);

		// Compose the final matrix: Scale * Rotation * Translation
		matrix = scaleMatrix * rotationMatrix * translationMatrix;
		worldMatrix = matrix;
	}

	/**
	 * @brief Render logic for transform.
	 * @note Typically empty, as the transform provides data for other components
	 * rather than rendering itself.
	 * @param deviceContext The graphics device context.
	 */
	void
	render(DeviceContext& deviceContext) override {}

	/**
	 * @brief Cleanup logic.
	 */
	void
	destroy() {}

	/** @brief Gets the position vector. */
	const EU::Vector3&
	getPosition() const { return position; }

	/** @brief Sets the position vector. */
	void
	setPosition(const EU::Vector3& newPos) { position = newPos; }

	/** @brief Gets the rotation vector (in radians or degrees depending on implementation). */
	const EU::Vector3&
	getRotation() const { return rotation; }

	/** @brief Sets the rotation vector. */
	void
	setRotation(const EU::Vector3& newRot) { rotation = newRot; }

	/** @brief Gets the scale vector. */
	const EU::Vector3&
	getScale() const { return scale; }

	/** @brief Sets the scale vector. */
	void
	setScale(const EU::Vector3& newScale) { scale = newScale; }

	/**
	 * @brief Sets position, rotation, and scale simultaneously.
	 * @param newPos The new position.
	 * @param newRot The new rotation.
	 * @param newSca The new scale.
	 */
	void
	setTransform(const EU::Vector3& newPos,
							 const EU::Vector3& newRot,
							 const EU::Vector3& newSca) {
		position = newPos;
		rotation = newRot;
		scale = newSca;
	}

	/**
	 * @brief Moves the position by a delta vector.
	 * @param translation The vector to add to the current position.
	 */
	void
	translate(const EU::Vector3& translation);

private:
	/** @brief Local position in 3D space. */
	EU::Vector3 position;

	/** @brief Local rotation (Euler angles). */
	EU::Vector3 rotation;

	/** @brief Local scale. */
	EU::Vector3 scale;

public:
	/** 
	 * @brief The calculated World Matrix (SRT).
	 * Passed to shaders to transform vertices from Model Space to World Space.
	 */
	XMMATRIX matrix;

	XMMATRIX worldMatrix;
};