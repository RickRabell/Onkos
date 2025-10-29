#include "SamplerState.h"
#include "Device.h"
#include "DeviceContext.h"

HRESULT
SamplerState::init(Device& device) {
	if(!device.m_device) {
		ERROR("SamplerState", "init", "Device is nullptr");
		return E_POINTER;
	}

  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device.m_device->CreateSamplerState(&sampDesc, &m_sampler);
  if(FAILED(hr)) {
    ERROR("SamplerState", "init", "Failed to create sampler state");
    return hr;
	}

	return S_OK;
}

void
SamplerState::update() {

}

void
SamplerState::render(DeviceContext& deviceContext,
                     unsigned int startSlot,
                     unsigned int numSamplers) {
  if(!m_sampler) {
    ERROR("SamplerState", "render", "Sampler state is nullptr");
    return;
	}

	deviceContext.PSSetSamplers(startSlot, numSamplers, &m_sampler);
}

void
SamplerState::destroy() {
  if (m_sampler) {
    SAFE_RELEASE(m_sampler);
  }
}