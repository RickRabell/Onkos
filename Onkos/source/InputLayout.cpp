#include "InputLayout.h"
#include "Device.h"
#include "DeviceContext.h"

HRESULT
InputLayout::init(Device& device,
									std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
									ID3DBlob* vertexShaderData) {
	if (layout.empty()) {
		ERROR("InputLayout", "init", "Layout vector is empty");
		return E_INVALIDARG;
	}
	if (!vertexShaderData) {
		ERROR("InputLayout", "init", "VertexShaderData is nullptr");
		return E_POINTER;
	}

	HRESULT hr = device.CreateInputLayout(layout.data(),
																				static_cast<unsigned int>(layout.size()),
																				vertexShaderData->GetBufferPointer(),
																				vertexShaderData->GetBufferSize(),
																				&m_inputLayout);

	if (FAILED(hr)) {
		ERROR("InputLayout", "init", 
			("Failed to create InputLayout. HRESULT: " + std::to_string(hr)).c_str());
		return hr;
	}

	return S_OK;
}

void
InputLayout::update() {
	// Aquí se podría utilizar en caso de necesitar cambios dinámicos en el layout
}

void
InputLayout::render(DeviceContext& deviceContext) {
	if (!m_inputLayout) {
		ERROR("InputLayout", "render", "InputLayout is nullptr");
		return;
	}

	deviceContext.m_deviceContext->IASetInputLayout(m_inputLayout);
}

void
InputLayout::destroy() {
	SAFE_RELEASE(m_inputLayout);
}