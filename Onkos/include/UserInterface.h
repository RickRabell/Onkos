#pragma once
#include "Prerequisites.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <imgui_internal.h>
#include "ECS/Actor.h"

class 
UserInterface {
public:
	UserInterface() = default;
	~UserInterface() = default;

	void
	init(void* window, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void
	update();

	void
	render();

	void
	destroy();

	bool
	vec3Control(const std::string& label,
								float* values,
								float resetValues = 0.0f,
								float columnWidth = 100.0f);

	void 
	setSelectedActor(Actor* actor) { m_selectedActor = actor; }

private:
	Actor* m_selectedActor = nullptr;
};

/*
// Supón que tienes una instancia de Actor llamada 'actor'
ImGui::Begin("Transform");

ImGui::Text("Position");
ImGui::DragFloat3("##Position", actor.transform.position, 0.1f);

ImGui::Text("Rotation");
ImGui::DragFloat3("##Rotation", actor.transform.rotation, 0.1f);

ImGui::Text("Scale");
ImGui::DragFloat3("##Scale", actor.transform.scale, 0.1f);

ImGui::End();

vec3Control("Position", actor.transform.position);
vec3Control("Rotation", actor.transform.rotation);
vec3Control("Scale", actor.transform.scale, 1.0f); // Por ejemplo, escala por defecto en 1
*/