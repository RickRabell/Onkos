#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Material;
class DeviceContext;
class Texture;

class
MaterialInstance {
public:
	// Setters
	void 
	SetMaterial(Material* material) { m_material = material; }
	
	void 
	SetAlbedo(Texture* texture) { m_albedo = texture; }
	
	void 
	SetNormal(Texture* texture) { m_normal = texture; }
	
	void 
	SetMetallic(Texture* texture) { m_metallic = texture; }
	
	void 
	SetRoughness(Texture* texture) { m_roughness = texture; }
	
	void 
	SetAO(Texture* texture) { m_ao = texture; }
	
	void 
	SetEmissive(Texture* texture) { m_emissive = texture; }

	// Getters
	Material* 
	GetMaterial() const { return m_material; }
	
	Texture* 
	GetAlbedo() const { return m_albedo; }
	
	Texture* 
	GetNormal() const { return m_normal; }
	
	Texture* 
	GetMetallic() const { return m_metallic; }
	
	Texture* 
	GetRoughness() const { return m_roughness; }
	
	Texture* 
	GetAO() const { return m_ao; }
	
	Texture* 
	GetEmissive() const { return m_emissive; }

	MaterialParams& 
	GetParams() { return m_params; }
	
	const MaterialParams& 
	GetParams() const { return m_params; }

	void 
	BindTextures(DeviceContext& deviceContext) const;

private:
	Material* m_material = nullptr;
	Texture* m_albedo = nullptr;
	Texture* m_normal = nullptr;
	Texture* m_metallic = nullptr;
	Texture* m_roughness = nullptr;
	Texture* m_ao = nullptr;
	Texture* m_emissive = nullptr;
	MaterialParams m_params;
};

