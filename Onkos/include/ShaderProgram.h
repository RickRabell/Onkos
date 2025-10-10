#pragma once
#include "Prerequisites.h"
#include "InputLayout.h"

// Forward Declarations
class Device;
class DeviceContext;

/**
 * @class ShaderProgram
 * @brief Manages the lifecycle of HLSL shaders.
 * @author Ricardo Rabell
 * @date 2025-10-09
 *
 * This class is responsible for loading HLSL shader source code from files,
 * compiling it into bytecode, creating the corresponding GPU shader objects
 * (Vertex and Pixel shaders), and managing the associated Input Layout. It also
 * handles binding these shaders to the graphics pipeline for rendering.
 */
class
ShaderProgram {
public:
	/**
	 * @brief Default constructor.
	 */
	ShaderProgram() = default;

	/**
	 * @brief Default destructor.
	 */
	~ShaderProgram() = default;

	/**
	 * @brief Initializes the complete shader program.
	 * This high-level function compiles both vertex and pixel shaders from a single
	 * file and creates the corresponding input layout.
	 * @param device The graphics device for resource creation.
	 * @param fileName The path to the .hlsl shader file.
	 * @param layout A vector describing the vertex input layout.
	 */
	HRESULT
	init(Device& device,
			 const std::string& fileName,
			 std::vector<D3D11_INPUT_ELEMENT_DESC> layout);

	/**
	 * @brief Per-frame update logic for the shader program.
	 * @note Placeholder for future functionality like hot-reloading.
	 * @todo Implementation details should be added.
	 */
	void
	update();

	/**
	 * @brief Binds both the vertex and pixel shaders to the graphics pipeline.
	 * @param deviceContext The device context used to issue binding commands.
	 */
	void
	render(DeviceContext& deviceContext);

	/**
	 * @brief Binds a specific shader type (Vertex or Pixel) to the pipeline.
	 * @param deviceContext The device context used to issue binding commands.
	 * @param type The type of shader to bind.
	 */
	void
	render(DeviceContext& deviceContext, ShaderType type);

	/**
	 * @brief Releases all shader objects and compiled bytecode blobs.
	 */
	void
	destroy();

	/**
 * @brief Creates the input layout object.
 * @param device The graphics device for resource creation.
 * @param layout A vector describing the vertex input layout.
 * @return HRESULT S_OK if successful.
 */
	HRESULT
	CreateInputLayout(Device& device,
										std::vector<D3D11_INPUT_ELEMENT_DESC> layout);

	/**
	 * @brief Creates a shader object from pre-compiled member data.
	 * @param device The graphics device for resource creation.
	 * @param type The type of shader to create (Vertex or Pixel).
	 * @return HRESULT S_OK if successful.
	 */
	HRESULT
	CreateShader(Device& device, ShaderType type);

	/**
	 * @brief Compiles and creates a shader object from a specific file.
	 * @param device The graphics device for resource creation.
	 * @param type The type of shader to create.
	 * @param fileName The path to the .hlsl shader file.
	 * @return HRESULT S_OK if successful.
	 */
	HRESULT
	CreateShader(Device& device, ShaderType type, const std::string& fileName);

	/**
	 * @brief A low-level helper function to compile a shader from a file.
	 * @param szFileName Path to the shader file.
	 * @param szEntryPoint The name of the shader's entry-point function.
	 * @param szShaderModel The shader model to compile against (e.g., "vs_5_0").
	 * @param ppBlobOut A pointer to receive the compiled shader bytecode.
	 * @return HRESULT S_OK if compilation is successful.
	 */
	HRESULT
	CompileShaderFromFile(char* szFileName,
												LPCSTR szEntryPoint,
												LPCSTR szShaderModel,
												ID3DBlob** ppBlobOut);

public:
	/** @brief Pointer to the DirectX 11 Vertex Shader object. */
	ID3D11VertexShader* m_VertexShader = nullptr;

	/** @brief Pointer to the DirectX 11 Pixel Shader object. */
	ID3D11PixelShader* m_PixelShader = nullptr;
	
	/** @brief The Input Layout object associated with the vertex shader. */
	InputLayout m_inputLayout;

private:
	/** @brief The file name of the HLSL shader source. */
	std::string m_shaderFileName;

	/** @brief Blob containing the compiled vertex shader bytecode. */
	ID3DBlob* m_vertexShaderData = nullptr;
	
	/** @brief Blob containing the compiled pixel shader bytecode. */
	ID3DBlob* m_pixelShaderData = nullptr;
};