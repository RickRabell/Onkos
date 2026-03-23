#pragma once
#include "Prerequisites.h"

/**
 * @class LayoutBuilder
 * @brief A utility class implementing the Builder pattern for Direct3D 11 Input Layouts.
 * @author Ricardo Rabell
 * @date 2026-03-15
 *
 * @details
 * **Context:** When you send a chunk of vertex data to the GPU (like an array of floats),
 * the GPU has no idea what those floats mean. Are the first three floats a position?
 * Is the next one a color? A texture coordinate? The Input Layout is the "blueprint"
 * that explains this memory structure to the GPU.
 * * **Why it exists:** Creating `D3D11_INPUT_ELEMENT_DESC` arrays manually in C++ is highly
 * verbose, error-prone, and visually clunky. This class uses a fluent interface
 * (returning `*this`) so you can chain `.Add().Add()` calls to create the blueprint cleanly.
 * * **Analogy:** Think of this as defining the columns in a spreadsheet. The GPU is the
 * spreadsheet software, and the `LayoutBuilder` creates the headers ("Position", "UV",
 * "Normal") so the software knows how to read the rows of data you paste in later.
 */
class 
LayoutBuilder {
public:
	/**
	 * @brief Appends a new vertex attribute descriptor to the layout.
	 * @param semantic The HLSL semantic name tying this element to the shader (e.g., "POSITION", "TEXCOORD").
	 * @param format The data type and size (e.g., DXGI_FORMAT_R32G32B32_FLOAT for a Vector3).
	 * @param semanticIndex Appended to the semantic name if there are multiples (e.g., TEXCOORD0, TEXCOORD1).
	 * @param inputSlot The vertex buffer slot this data comes from (0-15).
	 * @param alignedByteOffset Offset in bytes from the start of the vertex. D3D11_APPEND_ALIGNED_ELEMENT automatically calculates this.
	 * @param slotClass Indicates if this is per-vertex data or per-instance data.
	 * @param instanceStepRate How many instances to draw before stepping forward one unit in the instance buffer.
	 * @return LayoutBuilder& Reference to this builder to allow method chaining.
	 */
	LayoutBuilder&
	Add(const char* semantic,
			DXGI_FORMAT format,
			UINT semanticIndex = 0,
			UINT inputSlot = 0,
			UINT alignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_CLASSIFICATION slotClass = D3D11_INPUT_PER_VERTEX_DATA,
			UINT instanceStepRate = 0) {
		D3D11_INPUT_ELEMENT_DESC d{};

		d.SemanticName = semantic;
		d.SemanticIndex = semanticIndex;
		d.Format = format;
		d.InputSlot = inputSlot;
		d.AlignedByteOffset = alignedByteOffset;
		d.InputSlotClass = slotClass;
		d.InstanceDataStepRate = instanceStepRate;
		m_elems.push_back(d);

		return *this;
	}

	/**
	 * @brief A shortcut method specifically for adding instancing data layout elements.
	 * @note Instancing allows drawing thousands of identical meshes (like grass) in a single
	 * draw call by providing a secondary buffer with per-instance data (like positions).
	 * @param semantic The HLSL semantic name.
	 * @param format The data format.
	 * @param semanticIndex The semantic index.
	 * @param inputSlot Defaulted to 1, as slot 0 is typically the base vertex geometry.
	 * @param alignedByteOffset Automatic offset calculation.
	 * @param instanceStepRate Defaults to 1 (step forward for every instance).
	 * @return LayoutBuilder& Reference to this builder for chaining.
	 */
	LayoutBuilder&
	AddInstance(const char* semantic,
							DXGI_FORMAT format,
							UINT semanticIndex = 0,
							UINT inputSlot = 1,
							UINT alignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
							UINT instanceStepRate = 1) {
		return Add(semantic,
							 format,
							 semanticIndex,
							 inputSlot,
							 alignedByteOffset,
							 D3D11_INPUT_PER_INSTANCE_DATA,
							 instanceStepRate);
	}

	/**
	* @brief Retrieves the compiled vector of descriptors.
	* @return const std::vector<D3D11_INPUT_ELEMENT_DESC>& The layout descriptor array.
	*/
	const std::vector<D3D11_INPUT_ELEMENT_DESC>&
	Get() const {
		return m_elems;
	}

	/**
	* @brief Gets the number of elements in the layout.
	* @return UINT Element count.
	*/
	UINT
	Count() const {
		return (UINT)m_elems.size();
	}

private:
	/** @brief The internal storage for the layout descriptors. */
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_elems;
};