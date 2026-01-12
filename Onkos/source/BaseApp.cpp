#include "BaseApp.h"
#include <ResourceManager.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, 
                                                             UINT msg, 
                                                             WPARAM wParam, 
                                                             LPARAM lParam);

HRESULT
BaseApp::awake() {
  HRESULT hr = S_OK;

  // Initialization of DLL's and extern elements to the engine.

  // Log success Message
  MESSAGE("Main", "Awake", "Application awake succesfully");

  return hr;
}

int 
BaseApp::run(HINSTANCE hInst, int nCmdShow) {
  // 1) Initialize Window
  if (FAILED(m_window.init(hInst, nCmdShow, wndProc))) {
    ERROR("Main", "Run", "Failed to initialize Window.");
    return 0;
  }
  // 2) Awake Application
  if (FAILED(awake())) {
    ERROR("Main", "Run", "Failed to awake application.");
  }
  // 3) Initialize Device and Device Contex
  if (FAILED(init())) {
    ERROR("Main", "Run", "Failed to initialize Device and Device Context.");
    return 0;
  }

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
         0);

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

    /*
    bool loadSuccess = m_modelLoader.loadModel("test.obj", m_mesh);

    if (!loadSuccess)
    {
      ERROR("BaseApp.cpp", "init", "Failed to load model .obj");
      return E_FAIL;
    }
    */

    // Create vertex buffer
    //hr = m_vertexBuffer.init(m_device, Bowser[0], D3D11_BIND_VERTEX_BUFFER);

    //if (FAILED(hr)) {
    //  ERROR("Main", "InitDevice",
    //    ("Failed to initialize VertexBuffer. HRESULT: " + std::to_string(hr)).c_str());
    //  return hr;
    //}

    // Create index buffer
    //hr = m_indexBuffer.init(m_device, Bowser[0], D3D11_BIND_INDEX_BUFFER);

    //if (FAILED(hr)) {
    //  ERROR("Main", "InitDevice",
    //    ("Failed to initialize IndexBuffer. HRESULT: " + std::to_string(hr)).c_str());
    //  return hr;
    //}

    //auto& resourceMan = ResourceManager::getInstance();

    //std::shared_ptr<Model3D> model = resourceMan.GetOrLoad<Model3D>("CubeModel", "AbeBowser.fbx", ModelType::FBX);

    // Set primitive topology
    //m_deviceContext.m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

    /*
    hr = m_cbChangesEveryFrame.init(m_device, sizeof(CBChangesEveryFrame));
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize ChangesEveryFrame Buffer. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }
    */

    // Create the sample state
    /*
    hr = m_samplerState.init(m_device);
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize SamplerState. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }
    */

    // Initialize the world matrices
    //m_World = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    cbNeverChanges.mView = XMMatrixTranspose(m_View);
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4,
                                            m_window.m_width / (FLOAT)m_window.m_height,
                                            0.01f,
                                            100.0f);
    cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);

    m_userInterface.init(m_window.m_hWnd, 
                         m_device.m_device, 
                         m_deviceContext.m_deviceContext);

    // Asignar el actor que queremos editar (AbeBowser)
    if (!m_abeBowser.isNull()) {
      m_userInterface.setSelectedActor(m_abeBowser.get());
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
  m_userInterface.update();

  // Actualizar la matriz de proyección y vista
  cbNeverChanges.mView = XMMatrixTranspose(m_View);
  m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);
  m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 
                                          m_window.m_width / (FLOAT)m_window.m_height, 
                                          0.01f, 100.0f);

  cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
  m_cbChangeOnResize.update(m_deviceContext, 
                            nullptr, 
                            0, 
                            nullptr, 
                            &cbChangesOnResize, 
                            0, 
                            0);

  // Update Actors
  for (auto& actor : m_actors) {
		actor->update(deltaTime, m_deviceContext);
  }

  // Modify the color
  //m_vMeshColor.x = (sinf(t * 1.0f) + 1.0f) * 0.5f;
  //m_vMeshColor.y = (cosf(t * 3.0f) + 1.0f) * 0.5f;
  //m_vMeshColor.z = (sinf(t * 5.0f) + 1.0f) * 0.5f;

 // m_vMeshColor.x = 1.0f;
 //m_vMeshColor.y = 1.0f;
 //m_vMeshColor.z = 1.0f;

  // Rotate cube around the origin
  // Apply Scale
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	// Apply Rotation
	//XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(-0.60f, 3.0f, -0.20f);
	// Apply Translation
  //XMMATRIX translationMatrix = XMMatrixTranslation(2.0f, -4.9f, 11.0f);

	// Compose the final matrix in order: scale -> rotate -> translate
	/*
  m_World = scaleMatrix * rotationMatrix * translationMatrix;
  cb.mWorld = XMMatrixTranspose(m_World);
  cb.vMeshColor = m_vMeshColor;
  m_cbChangesEveryFrame.update(m_deviceContext, nullptr, 0, nullptr, &cb, 0, 0);
  */
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

  // Set shader program
  m_shaderProgram.render(m_deviceContext);

  // Asignar buffers constantes
  m_cbNeverChanges.render(m_deviceContext, 0, 1);
  m_cbChangeOnResize.render(m_deviceContext, 1, 1);

  // Render All Actors
  for (auto& actor : m_actors) {
		actor->render(m_deviceContext);
  }

  // Render UI (Dibuja la ventana y emite comandos de DX11)
  m_userInterface.render();
  // Render the cube
  // Asignar buffers Vertex e Index
  //m_vertexBuffer.render(m_deviceContext, 0, 1);
  //m_indexBuffer.render(m_deviceContext, 0, 1, false, DXGI_FORMAT_R32_UINT);
  //m_cbChangesEveryFrame.render(m_deviceContext, 2, 1);
  //m_cbChangesEveryFrame.render(m_deviceContext, 2, 1, true);
  // Asignar textura y sampler
  //m_textureCube.render(m_deviceContext, 0, 1);
  //m_samplerState.render(m_deviceContext, 0, 1);
  //m_deviceContext.DrawIndexed(Bowser[0].m_numIndex, 0, 0);
  // Set Primitive Topology
  //m_deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  //
  // Present our back buffer to our front buffer
  //

	// Present our back buffer to our front buffer
  m_swapChain.present();
}

void
BaseApp::destroy() {
  if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();
  
  //m_samplerState.destroy();
  //m_textureCube.destroy();

  m_userInterface.destroy();

  m_cbNeverChanges.destroy();
  m_cbChangeOnResize.destroy();
  //m_cbChangesEveryFrame.destroy();
  //m_vertexBuffer.destroy();
  //m_indexBuffer.destroy();
  m_shaderProgram.destroy();
  m_depthStencil.destroy();
  m_depthStencilView.destroy();
  m_renderTargetView.destroy();
  m_swapChain.destroy();
  m_backBuffer.destroy();
  m_deviceContext.destroy();
  m_device.destroy();
}

LRESULT 
BaseApp::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  // Permitir que ImGui procese los eventos primero
  if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    return true;
  
  switch (message)
  {
  case WM_CREATE:
  {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
  }
  return 0;
  case WM_PAINT:
  {
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