#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Texture;
class DepthStencilView;

class RenderTargetView {
public:
	RenderTargetView() = default;
	~RenderTargetView() = default;

	HRESULT
	init(Device& device, Texture& backBuffer, DXGI_FORMAT format);

	HRESULT
	init(Device& device, 
			 Texture& inTex, 
			 D3D11_RTV_DIMENSION ViweDimension, 
			 DXGI_FORMAT format);

	void
	update();

	void
	render(DeviceContext& deviceContext, 
				 DepthStencilView& depthStencilView, 
				 unsigned int numViwes, 
				 const float ClearColor[4]);

	void
	render(DeviceContext& deviceContext, unsigned int numViews);

	void
	destroy();

private:
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
};