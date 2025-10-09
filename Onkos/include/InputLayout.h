#pragma once
#include "Prerequisites.h"

// Forward declarations
class Device;
class DeviceContext;

/**
 * @class InputLayout
 * @brief Manages a Direct3D 11 Input Layout object.
 * @author Ricardo Rabell
 * @date 2025-10-09
 *
 * This class encapsulates an ID3D11InputLayout. The input layout's critical
 * role is to describe how vertex data from a vertex buffer is streamed into the
 * Input-Assembler (IA) stage of the graphics pipeline. It defines the format,
 * size, and semantic meaning of each element in a vertex (e.g., position, color,
 * texture coordinate) so that it correctly matches the input signature of the
 * vertex shader.
 */
class InputLayout {
public:
	/**
	 * @brief Default constructor.
	 */
	InputLayout() = default;

	/**
	 * @brief Default destructor.
	 */
	~InputLayout() = default;

	/**
	 * @brief Initializes the Input Layout object.
	 * This function creates the ID3D11InputLayout object by describing the
	 * per-vertex data structure and validating it against a compiled vertex shader's
	 * input signature.
	 * @param device The graphics device used to create the input layout.
	 * @param layout A vector of D3D11_INPUT_ELEMENT_DESC structures that defines the vertex format.
	 * @param vertexShaderData A pointer to the compiled vertex shader bytecode, used for validation.
	 * @return HRESULT S_OK if the layout is created successfully and matches the shader signature.
	 */
	HRESULT
	init(Device& device,
			std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
			ID3DBlob* vertexShaderData);

	/**
	 * @brief Per-frame update logic for the input layout.
	 */
	void
	update();

	/**
	 * @brief Binds the input layout to the Input-Assembler (IA) stage of the pipeline.
	 * @param deviceContext The device context used to issue the binding command.
	 */
	void
	render(DeviceContext& deviceContext);

	/**
	 * @brief Releases the underlying ID3D11InputLayout COM object.
	 */
	void
	destroy();

public:
	/**
	 * @brief Pointer to the underlying DirectX 11 Input Layout interface.
	 */
	ID3D11InputLayout* m_inputLayout = nullptr;
};