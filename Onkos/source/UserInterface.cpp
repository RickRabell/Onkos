#include "UserInterface.h"

void
UserInterface::init(void* window, 
                    ID3D11Device* device, 
                    ID3D11DeviceContext* deviceContext) {

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When Viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init((HWND)window);
	ImGui_ImplDX11_Init(device, deviceContext);
}

void
UserInterface::update() {
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

//void
//UserInterface::render() {
//	ImGui::Render();
//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
//	ImGuiIO& io = ImGui::GetIO();
//	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
//		ImGui::UpdatePlatformWindows();
//		ImGui::RenderPlatformWindowsDefault();
//	}
//}

void 
UserInterface::render() {
  // Crear la ventana de Propiedades
  ImGui::Begin("Inspector de Propiedades");

  if (m_selectedActor) {
    ImGui::Text("Actor: %s", m_selectedActor->getName().c_str());
    ImGui::Separator();

    // Obtener el componente Transform
    auto transformComponent = m_selectedActor->getComponent<Transform>();

    if (transformComponent) {
      if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        // --- POSICIÓN ---
        // Leemos los valores actuales
        EU::Vector3 pos = transformComponent->getPosition();
        float p[3] = { pos.x, pos.y, pos.z };

        // Dibujamos el control y si devuelve true (hubo cambios), actualizamos
        if (vec3Control("Position", p)) {
          transformComponent->setPosition(EU::Vector3(p[0], p[1], p[2]));
        }

        // --- ROTACIÓN ---
        // Leemos radianes, convertimos a grados para la UI
        EU::Vector3 rot = transformComponent->getRotation();
        float r[3] = { XMConvertToDegrees(rot.x), 
                       XMConvertToDegrees(rot.y), 
                       XMConvertToDegrees(rot.z) };

        if (vec3Control("Rotation", r)) {
          // Convertimos grados a radianes para guardarlo
          transformComponent->setRotation(EU::Vector3(XMConvertToRadians(r[0]), 
                                                      XMConvertToRadians(r[1]), 
                                                      XMConvertToRadians(r[2])));
        }

        // --- ESCALA ---
        EU::Vector3 scale = transformComponent->getScale();
        float s[3] = { scale.x, scale.y, scale.z };

        if (vec3Control("Scale", s, 1.0f)) { // Reset value es 1.0 para escala
          transformComponent->setScale(EU::Vector3(s[0], s[1], s[2]));
        }
      }
    }
  }
  else {
    ImGui::Text("No hay actor seleccionado.");
  }

  ImGui::End();

  // Renderizado final de ImGui
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void
UserInterface::destroy() {
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool 
UserInterface::vec3Control(const std::string& label, 
                           float* values, 
                           float resetValue, 
                           float columnWidth) {
  bool changed = false;
  ImGui::PushID(label.c_str());

  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, columnWidth);
  ImGui::Text(label.c_str());
  ImGui::NextColumn();

  ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

  float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
  ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

  // Eje X
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
  if (ImGui::Button("X", buttonSize)) {
    values[0] = resetValue;
    changed = true;
  }
  ImGui::PopStyleColor(3);

  ImGui::SameLine();
  if (ImGui::DragFloat("##X", &values[0], 0.1f, 0.0f, 0.0f, "%.2f")) changed = true;
  ImGui::PopItemWidth();
  ImGui::SameLine();

  // Eje Y
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
  if (ImGui::Button("Y", buttonSize)) {
    values[1] = resetValue;
    changed = true;
  }
  ImGui::PopStyleColor(3);

  ImGui::SameLine();
  if (ImGui::DragFloat("##Y", &values[1], 0.1f, 0.0f, 0.0f, "%.2f")) changed = true;
  ImGui::PopItemWidth();
  ImGui::SameLine();

  // Eje Z
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
  if (ImGui::Button("Z", buttonSize)) {
    values[2] = resetValue;
    changed = true;
  }
  ImGui::PopStyleColor(3);

  ImGui::SameLine();
  if (ImGui::DragFloat("##Z", &values[2], 0.1f, 0.0f, 0.0f, "%.2f")) changed = true;
  ImGui::PopItemWidth();

  ImGui::PopStyleVar();

  ImGui::Columns(1);
  ImGui::PopID();

  return changed;
}