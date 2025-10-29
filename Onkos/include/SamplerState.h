#pragma once
#include "Prerequisites.h"

// Forward declarations
class Device;
class DeviceContext;

/**
 * @class SamplerState
 * @brief Manages a Direct3D 11 Sampler State object.
 * @author Ricardo Rabell
 * @date 2025-10-29
 *
 * This class encapsulates an ID3D11SamplerState. The sampler state defines
 * how texture data is sampled by a shader. It controls parameters like
 * filtering (e.g., linear, point, anisotropic), addressing modes (e.g., wrap,
 * clamp, mirror), and mipmap LOD (Level of Detail) biasing.
 */
class
SamplerState {
public:
	/**
	 * @brief Default constructor.
	 */
	SamplerState() = default;

	/**
	 * @brief Default destructor.
	 */
	~SamplerState() = default;

	/**
	 * @brief Initializes the sampler state object.
	 * This method creates the underlying ID3D11SamplerState with
	 * specific filter, address, and LOD parameters.
	 * @param device The graphics device used to create the sampler state.
	 * @return HRESULT S_OK if successful, otherwise an error code.
	 */
	HRESULT
	init(Device& device);

	/**
	 * @brief Per-frame update logic for the sampler state.
	 */
	void
	update();

	/**
	 * @brief Binds the sampler state to a shader stage (typically the Pixel Shader).
	 * @param deviceContext The device context used to issue the binding command.
	 * @param startSlot The starting shader register slot to bind the sampler to (e.g., 0).
	 * @param numSamplers The number of samplers to bind (typically 1).
	 */
	void
	render(DeviceContext& deviceContext,
				 unsigned int startSlot,
				 unsigned int numSamplers);

	/**
	 * @brief Releases the underlying ID3D11SamplerState COM object.
	 */
	void
	destroy();

public:
	/**
	 * @brief Pointer to the underlying DirectX 11 Sampler State interface.
	 */
	ID3D11SamplerState* m_sampler = nullptr;
};