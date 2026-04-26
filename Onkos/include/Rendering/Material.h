#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class ShaderProgram;
class RasterizerState;
class DepthStencilState;
class SamplerState;

class
Material {
public:
	// Setters
	void 
	SetShader(ShaderProgram* shader) { m_shader = shader; }
	
	void 
	SetRasterizerState(RasterizerState* state) { m_rasterizerState = state; }
	
	void 
	SetDepthStencilState(DepthStencilState* state) { m_depthStencilState = state; }
	
	void 
	SetSamplerState(SamplerState* state) { m_samplerState = state; }
	
	void 
	SetDomain(MaterialDomain domain) { m_domain = domain; }
	
	void 
	SetBlendMode(BlendMode blendMode) { m_blendMode = blendMode; }

	// Getters
	ShaderProgram* 
	GetShader() const { return m_shader; }
	
	RasterizerState* 
	GetRasterizerState() const { return m_rasterizerState; }
	
	DepthStencilState* 
	GetDepthStencilState() const { return m_depthStencilState; }
	
	SamplerState* 
	GetSamplerState() const { return m_samplerState; }
	
	MaterialDomain 
	GetDomain() const { return m_domain; }
	
	BlendMode 
	GetBlendMode() const { return m_blendMode; }

private:
	ShaderProgram* m_shader = nullptr;                   
	RasterizerState* m_rasterizerState = nullptr;        
	DepthStencilState* m_depthStencilState = nullptr;    
	SamplerState* m_samplerState = nullptr;              
	MaterialDomain m_domain = MaterialDomain::Opaque;    
	BlendMode m_blendMode = BlendMode::Opaque;           
};