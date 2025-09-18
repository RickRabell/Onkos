#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

class
Texture {
public:
	Texture() = default;
	~Texture() = default;

	HRESULT
		init(	Device& deivce, const std::string& textureName, ExtensionType extensionType	);

	HRESULT
	init(	Device& device, 
				unsigned int width, 
				unsigned int height, 
				DXGI_FORMAT format, 
				unsigned int bindFlags, 
				unsigned int sampleCount = 1, 
				unsigned int qualityLevels = 0	);

	HRESULT
	init(Device& device, Texture& textureRef, DXGI_FORMAT format);

	void
	update();

	void
	render(DeviceContext& deviceContext, unsigned int startSlot, unsigned int numViews);

	void
	destroy();

public:
	ID3D11Texture2D* m_texture = nullptr;

	ID3D11ShaderResourceView* m_textureFromImg = nullptr;

	std::string m_textureName;
};
