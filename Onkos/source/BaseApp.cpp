#include "BaseApp.h"
#include <ResourceManager.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, 
                                                             UINT msg, 
                                                             WPARAM wParam, 
                                                             LPARAM lParam);

HRESULT
BaseApp::awake() {
  HRESULT hr = S_OK;

  // Initialize DLL's and external elements to the engine
  m_sceneGraph.init();

  // Log success Message
  MESSAGE("Main", "Awake", "Application awake succesfully.");

  return hr;
}

int 
BaseApp::run(HINSTANCE hInst, int nCmdShow) {
  // 1) Initialize Window
  if (FAILED(m_window.init(hInst, nCmdShow, wndProc))) {
    ERROR("Main", "Run", "Failed to initialize window.");
    return 0;
  }
  // 2) Awake Application
  if (FAILED(awake())) {
    ERROR("Main", "Run", "Failed to awake application.");
    return 0;
  }
  // 3) Initialize Device and Device Context
  if (FAILED(init())) {
    ERROR("Main", "Run", "Failed to initialize device and device context.");
    return 0;
  }
  // 4) Initialize GUI
  m_gui.init(m_window, m_device, m_deviceContext);

  // Main message loop
  MSG msg = {};
  LARGE_INTEGER freq, prev;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&prev);
  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      LARGE_INTEGER curr;
      QueryPerformanceCounter(&curr);
      float deltaTime = static_cast<float>(curr.QuadPart - prev.QuadPart) / freq.QuadPart;
      prev = curr;
      update(deltaTime);
      render();
    }
  }
  return (int)msg.wParam;
}

HRESULT
BaseApp::init() {
    HRESULT hr = S_OK;

    // Create Swap Chain
    hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);

    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
           ("Failed to initialize SwapChain. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }

    // Create a render target view
    hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);

    if (FAILED(hr))
    {
      ERROR("Main", "InitDevice",
           ("Failed to initialize RenderTargetView. HRESULT: "
             + std::to_string(hr)).c_str());
      return hr;
    }

    // Create depth stencil texture
    hr = m_depthStencil.init(m_device,
                             m_window.m_width,
                             m_window.m_height,
                             DXGI_FORMAT_D24_UNORM_S8_UINT,
                             D3D11_BIND_DEPTH_STENCIL,
                             4,
                             16);

    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize DepthStencil. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }

    // Crete the depth stencil view
    hr = m_depthStencilView.init(m_device,
                                 m_depthStencil,
                                 DXGI_FORMAT_D24_UNORM_S8_UINT);

    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize DepthStencilView. HRESULT: "
          + std::to_string(hr)).c_str());
      return hr;
    }

    // Create the viewport
    hr = m_viewport.init(m_window);

    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
           ("Failed to initialize Viewport. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }

    // Load Resources -> Modelos, Texturas e Interfaz de Usuario
    std::array<std::string, 6> faces = {
      "Skybox/cubemap_0.png",
      "Skybox/cubemap_1.png",
      "Skybox/cubemap_2.png",
      "Skybox/cubemap_3.png",
      "Skybox/cubemap_4.png",
      "Skybox/cubemap_5.png"
    };
    m_skyboxTexture.CreateCubemap(m_device, m_deviceContext, faces, false);

		// Set AbeBowser Actor
    m_abeBowser = EU::MakeShared<Actor>(m_device);

    if (!m_abeBowser.isNull()) {
			// Create vertex and index buffers
      std::vector<MeshComponent> abeBowserMeshes;
      m_model = new Model3D("AbeBowser.fbx", ModelType::FBX);
      abeBowserMeshes = m_model->GetMeshes();

      std::vector<Texture> abeBowserTextures;
      hr = m_abeBowserAlbedo.init(m_device, "JapaneseShrineAlbedo", ExtensionType::PNG);

      // Load the Texture
      if (FAILED(hr)) {
        ERROR("Main", "InitDevice",
          ("Failed to initialize abeBowserAlbedo. HRESULT: " 
            + std::to_string(hr)).c_str());
        return hr;
      }
			abeBowserTextures.push_back(m_abeBowserAlbedo);

      m_abeBowser->setMesh(m_device, abeBowserMeshes);
			m_abeBowser->setTextures(abeBowserTextures);
			m_abeBowser->setName("AbeBowser");
			m_actors.push_back(m_abeBowser);

      m_abeBowser->getComponent<Transform>()->setTransform(EU::Vector3(2.0f, -4.90f, 11.60f),
                                                           EU::Vector3(-0.60f, 3.0f, -0.20f),
                                                           EU::Vector3(1.0f, 1.0f, 1.0f));
    }
    else {
      ERROR("Main", "InitDevice", "Failed to create Abe Bowser Actor.");
      return E_FAIL;
    }

    // Store the Actors in the Scene Graph
    for (auto& actor : m_actors) {
      m_sceneGraph.addEntity(actor.get());
    }

    // Define the input layout
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
    D3D11_INPUT_ELEMENT_DESC position;
    position.SemanticName = "POSITION";
    position.SemanticIndex = 0;
    position.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    position.InputSlot = 0;
    position.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT /*0*/;
    position.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    position.InstanceDataStepRate = 0;
    layout.push_back(position);

    D3D11_INPUT_ELEMENT_DESC texcoord;
    texcoord.SemanticName = "TEXCOORD";
    texcoord.SemanticIndex = 0;
    texcoord.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    texcoord.InputSlot = 0;
    texcoord.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT /*0*/;
    texcoord.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    texcoord.InstanceDataStepRate = 0;
    layout.push_back(texcoord);

    // Create the Shader Program
    hr = m_shaderProgram.init(m_device, "Onkos.fx", layout);
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize ShaderProgram. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }

    // Create the constant buffers
    hr = m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges));
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize NeverChanges Buffer. HRESULT: " 
          + std::to_string(hr)).c_str());
      return hr;
    }

    hr = m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize));
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize ChangeOnResize Buffer. HRESULT: " 
          + std::to_string(hr)).c_str());
      return hr;
    }

    // Initialize the Camera
    m_camera.setLens(XM_PIDIV4, m_window.m_width / (float)m_window.m_height, 0.01f, 100.0f);
    m_camera.setPosition(0.0f, 3.0f, -6.0f);

    cbNeverChanges.mView = XMMatrixTranspose(m_camera.getView());
    cbChangesOnResize.mProjection = XMMatrixTranspose(m_camera.getProj());

		// Initialize the Skybox pass -> Texture load + Buffer and Shaders creation, specific for the skybox pass
    m_skybox.init(m_device, &m_deviceContext, m_skyboxTexture);

    // Initialize default states (Rasterizer, DepthStencil)
    hr = m_defaultRasterizer.init(m_device, D3D11_FILL_SOLID, D3D11_CULL_BACK, false, true);
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize default Rasterizer. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }

    hr = m_defaultDepthStencil.init(m_device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS);
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize default DepthStencilState. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }

    return S_OK;
}

void 
BaseApp::update(float deltaTime) {
  // Update our time
  static float t = 0.0f;
  if (m_swapChain.m_driverType == D3D_DRIVER_TYPE_REFERENCE)
  {
    t += (float)XM_PI * 0.0125f;
  }
  else
  {
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if (dwTimeStart == 0)
      dwTimeStart = dwTimeCur;
    t = (dwTimeCur - dwTimeStart) / 1000.0f;
  }
	
  // Update User Interface (Inicia el frame de ImGui)
  m_gui.update(m_viewport, m_window);
	bool show_demo_window = true;

  m_gui.inspectorGeneral(m_actors[m_gui.selectedActorIndex]);
  m_gui.outliner(m_actors);

	// Update the projection and view matrices in the constant buffers
  m_camera.updateViewMatrix();
  cbNeverChanges.mView = XMMatrixTranspose(m_camera.getView());
  m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);
  m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);
  //cbChangesOnResize.mProjection = XMMatrixTranspose(m_camera.getProj());

  // Update Actors
  m_sceneGraph.update(deltaTime, m_deviceContext);

  m_gui.editTransform(m_camera.getView(), 
                      m_camera.getProj(), 
                      m_actors[m_gui.selectedActorIndex]);
}

void
BaseApp::render() {
  // Set Render Target View
  float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
  m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);

  // Set Viewport
  m_viewport.render(m_deviceContext);

  // Set depth stencil view
  m_depthStencilView.render(m_deviceContext);

  // 1) SKYBOX PASS
  m_skybox.render(m_deviceContext, m_camera);

  // 2) RESTAURAR ESTADOS + PIPELINE DE ESCENA
  m_defaultRasterizer.render(m_deviceContext);
  m_defaultDepthStencil.render(m_deviceContext, 0, false);

  // limpia SRVs por seguridad
  ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
  m_deviceContext.m_deviceContext->PSSetShaderResources(10, 1, nullSRV);
  m_deviceContext.m_deviceContext->PSSetShaderResources(0, 1, nullSRV);

  // Re-bindea shader/layout de escena
  m_shaderProgram.render(m_deviceContext);
  //m_deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // CBs para VS (view/proj)
  m_cbNeverChanges.render(m_deviceContext, 0, 1);
  m_cbChangeOnResize.render(m_deviceContext, 1, 1);

  // 3) SCENE PASS
  m_sceneGraph.render(m_deviceContext);

  // 4) Render UI
  m_gui.render();

  // Present our back buffer to our front buffer
  m_swapChain.present();
}

void
BaseApp::destroy() {
  if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();

	m_sceneGraph.destroy();
  m_cbNeverChanges.destroy();
  m_cbChangeOnResize.destroy();
  m_shaderProgram.destroy();
  m_depthStencil.destroy();
  m_depthStencilView.destroy();
  m_renderTargetView.destroy();
  m_swapChain.destroy();
  m_backBuffer.destroy();

  m_gui.destroy();

  m_deviceContext.destroy();
  m_device.destroy();
}

LRESULT 
BaseApp::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  // Permitir que ImGui procese los eventos primero
  if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
    return true;
  }
  
  switch (message) {
  case WM_CREATE: {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
  }
  return 0;
  case WM_PAINT: {
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
  }
  return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
  /*
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message)
  {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
  */
}