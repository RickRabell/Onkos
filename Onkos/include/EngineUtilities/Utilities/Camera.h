#pragma once
#include "Prerequisites.h"
#include "EngineUtilities\Vectors\Vector3.h"

/**
* @class Camera
* @brief Manages the 3D viewing volume and spatial transformations for the scene.
* @author Ricardo Rabell
* @date 2026-02-15
*
* This class encapsulates the math required to generate the View and Projection matrices.
* * Context: In a 3D pipeline, the View matrix transforms vertices from World Space
* to View (Camera) Space, effectively placing the camera at the origin (0,0,0) looking
* down the Z-axis. The Projection matrix then maps this 3D view into a 2D projection
* window while preserving depth (Z-buffer) information.
* * Analogy: If the Scene Graph is the set where actors are placed, the Camera is the
* physical lens filming them. Moving the camera forward is mathematically identical to
* moving the entire world backward.
*/
class 
Camera {
public:
	/** @brief Default constructor. */
	Camera();

	/** @brief Default destructor. */
	~Camera() = default;

	/**
		* @brief Sets the camera's lens parameters to build the Projection Matrix.
		* @param fovYRadians The vertical Field of View in radians. (e.g., PI/4 for 45 degrees).
		* @param aspectRatio The ratio of the viewport's width to its height (Width / Height).
		* @param nearPlane The closest distance the camera can see. Objects closer are clipped.
		* @param farPlane The furthest distance the camera can see. Objects further are clipped.
		*/
	void
	setLens(float fovYRadians, float aspectRatio, float nearPlane, float farPlane);

	/**
		* @brief Explicitly sets the camera's position in world space.
		* @param x X coordinate.
		* @param y Y coordinate.
		* @param z Z coordinate.
		*/
	void
	setPosition(float x, float y, float z);

	/**
		* @brief Explicitly sets the camera's position using a Vector3.
		* @param pos The new position vector.
		*/
	void
	setPosition(const EU::Vector3& pos);

	/**
		* @brief Retrieves the current world-space position of the camera.
		* @return EU::Vector3 The position vector.
		*/
	EU::Vector3
	getPosition() const { return m_position; }

    /**
    * @brief Retrieves a reference to the current world-space position of the camera.
    * @return EU::Vector3& Reference to the position vector, allowing direct modification.
    */
  EU::Vector3&
  getPosition() { return m_position; }

	/**
		* @brief Orients the camera to look at a specific point in space.
		* Context: This function calculates the orthonormal basis vectors (Right, Up, Forward)
		* by computing the directional vector towards the target, and using cross products
		* with the global "Up" vector to find the orthogonal axes.
		* @param pos The position of the camera.
		* @param target The point in 3D space the camera should look at.
		* @param up The global up direction (usually 0, 1, 0).
		*/
	void
	lookAt(const EU::Vector3& pos,
				 const EU::Vector3& target,
				 const EU::Vector3& up = EU::Vector3(0, 1, 0));

	/**
		* @brief Moves the camera along its own local Forward/Backward axis.
		* @param d The distance to move. Positive moves forward, negative moves backward.
		*/
	void
	walk(float d);

	/**
		* @brief Moves the camera along its own local Right/Left axis.
		* @param d The distance to move. Positive moves right, negative moves left.
		*/
	void
	strafe(float d);

	/**
		* @brief Rotates the camera left or right around the Y axis.
		* @param radians The angle to rotate.
		*/
	void
	yaw(float radians);

	/**
		* @brief Rotates the camera up or down around its local Right axis.
		* @param radians The angle to rotate.
		*/
	void
	pitch(float radians);

	/**
		* @brief Re-calculates the View Matrix based on the current position and basis vectors.
		* Optimization Context: This relies on the `m_viewDirty` flag. Matrix multiplication
		* and normalization are computationally expensive. This function ensures the math
		* is only processed if the camera actually moved or rotated this frame.
		*/
	void
	updateViewMatrix();

	/**
		* @brief Retrieves the compiled View Matrix for rendering.
		* @return XMMATRIX The view matrix, ready for shader constant buffers.
		*/
	XMMATRIX
	getView() const { return XMLoadFloat4x4(&m_view); }

	/**
		* @brief Retrieves the compiled Projection Matrix for rendering.
		* @return XMMATRIX The projection matrix.
		*/
	XMMATRIX
	getProj() const { return XMLoadFloat4x4(&m_proj); }

	/**
		* @brief Retrieves a modified View Matrix that only contains rotation, stripping out translation.
		* * Why this exists: This is a classic rendering trick used exclusively for drawing Skyboxes.
		* By removing translation, the camera can look around the skybox, but no matter how far
		* the player walks, they will never get "closer" to the sky.
		* @return XMMATRIX The rotation-only view matrix.
		*/
	XMMATRIX
	GetViewNoTranslation() const {
		XMMATRIX v = getView();
		// Strip translation by zeroing out the 4th row (X, Y, Z translation values).
		v.r[3] = XMVectorSet(0, 0, 0, 1);
		return v;
	}

	// Frustum getters
	float getFovY()   const { return m_fovY; }
	float getAspect() const { return m_aspectRatio; }
	float getNearZ()  const { return m_nearPlane; }
	float getFarZ()   const { return m_farPlane; }

	// Orthonormal Basis getters (Local axes)
	EU::Vector3 GetRight()   const { return m_right; }
	EU::Vector3 GetUp()      const { return m_up; }
	EU::Vector3 GetForward() const { return m_forward; }

	/**
		* @brief Utility function to convert a SIMD-optimized DirectX Vector (FXMVECTOR)
		* into the engine's standard Vector3 struct.
		* @param v The DirectX vector to convert.
		* @return EU::Vector3 The converted vector.
		*/
	inline EU::Vector3
	FromXM(FXMVECTOR v) {
		XMFLOAT3 t;
		XMStoreFloat3(&t, v);
		return EU::Vector3(t.x, t.y, t.z);
	}

private:
	/** @brief The spatial position of the camera in the world. */
	EU::Vector3 m_position;

	// Orthonormal Basis (The camera's local coordinate system)

	/** @brief The camera's local X-axis. */
	EU::Vector3 m_right{ 1.0f, 0.0f, 0.0f };
	/** @brief The camera's local Y-axis. */
	EU::Vector3 m_up{ 0.0f, 1.0f, 0.0f };
	/** @brief The camera's local Z-axis (look direction). */
	EU::Vector3 m_forward{ 0.0f, 0.0f, 1.0f };
	/** @brief Cached view matrix data (stored as 4x4 float to avoid alignment issues in classes). */
	XMFLOAT4X4 m_view{};
	/** @brief Cached projection matrix data. */
	XMFLOAT4X4 m_proj{};

	// Projection Parameters

	/** @brief Field of View on the Y-axis. Default is PI/4 (45 degrees). */
	float m_fovY{ XM_PIDIV4 };
	/** @brief Aspect ratio of the viewport. */
	float m_aspectRatio = 1.0f;
	/** @brief Distance to the near clipping plane. */
	float m_nearPlane = 0.01f;
	/** @brief Distance to the far clipping plane. */
	float m_farPlane = 1000.0f;

	/** * @brief Dirty flag pattern.
		* True if the position or rotation changed since the last view matrix update.
		*/
	bool m_viewDirty = true;
};