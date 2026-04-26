#pragma once
#include "Prerequisites.h"
#include "Buffer.h"
#include "DepthStencilState.h"
//#include "Rendering/RenderScene.h"
//#include "Rendering/RenderTypes.h"

class Device;
class DeviceContext;
class Camera;
class EditorViewportPass;

class
ForwardRenderer {
public:
	HRESULT 
	Init(Device& device);
	
	void 
	Resize(Device& device, unsigned int width, unsigned int height);
	
	void 
	UpdatePerFrame(const Camera& camera, const RenderScene& scene, DeviceContext& deviceContext);
	
	void 
	Render(DeviceContext& deviceContext,
				const Camera& camera,
				RenderScene& scene,
				EditorViewportPass& viewportPass);
	
	void 
	Destroy();

private:
	void 
	BuildQueues(RenderScene& scene, const Camera& camera);
	
	void 
	RenderOpaquePass(DeviceContext& deviceContext);
	
	void 
	RenderTransparentPass(DeviceContext& deviceContext);
	
	void 
	RenderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);
	
	void 
	RenderObject(DeviceContext& deviceContext, const RenderObject& object, RenderPassType passType);
	
	HRESULT 
	CreateAlphaBlendState(Device& device);

private:
	Buffer m_perFrameBuffer;
	Buffer m_perObjectBuffer;
	Buffer m_perMaterialBuffer;
	DepthStencilState m_transparentDepthStencil;
	ID3D11BlendState* m_alphaBlendState = nullptr;
	ID3D11BlendState* m_opaqueBlendState = nullptr;
	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	CBPerFrame m_cbPerFrame{};
	CBPerObject m_cbPerObject{};
	CBPerMaterial m_cbPerMaterial{};

	std::vector<const RenderObject*> m_opaqueQueue;
	std::vector<const RenderObject*> m_transparentQueue;
};