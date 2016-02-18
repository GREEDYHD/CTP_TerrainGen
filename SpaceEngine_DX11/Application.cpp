#include "Application.h"

using namespace std;

Application::Application()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_ShaderColor = 0;
	m_Timer = 0;
	m_CameraController = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_ShaderFont = 0;
	m_Text = 0;
	m_ShaderTerrain = 0;
	m_Light = 0;
	m_Frustum = 0;
	m_PerlinNoise = 0;
}

Application::Application(const Application& other)
{

}

Application::~Application()
{

}

bool Application::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	float cameraX, cameraY, cameraZ;
	XMFLOAT4X4 baseViewMatrix;
	char videoCard[128];
	int videoMemory;

	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new Input;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new Direct3D;
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new Camera;
	if (!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Set the initial position of the camera.
	cameraX = 0.0f;
	cameraY = 10.0f;
	cameraZ = 0.0f;

	m_Camera->SetPosition(cameraX, cameraY, cameraZ);

	m_PerlinNoise = new PerlinNoise();
	XMFLOAT3* pos = new XMFLOAT3(0, 0, 0);
	m_QuadTreeRootNode = new QuadTree(0, m_Direct3D->GetDevice(), m_PerlinNoise, *pos, 4);

	int mapSize = 4;

	for (int i = 0; i < pow(mapSize, 2); i++)
	{
		m_Terrain.push_back(new Terrain());
		//m_QuadTree.push_back(new QuadTree());
	}

	//int terrainSize = 256;

	//float p = 0, q = 0;
	//for (vector<Terrain*>::iterator it = m_Terrain.begin(); it != m_Terrain.end(); it++)
	//{
	//	XMFLOAT3 pos = XMFLOAT3((p - 1) * (terrainSize - 1), 0, (q - 1) * (terrainSize - 1));
	//	result = (*it)->Initialize(m_Direct3D->GetDevice(), m_PerlinNoise, L"TEX_STONE_COLOR.dds", pos, terrainSize);

	//	if (!result)
	//	{
	//		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
	//		return false;
	//	}
	//	if (p == mapSize - 1)
	//	{
	//		q++;
	//		p = 0;
	//	}
	//	else
	//	{
	//		p++;
	//	}
	//}

	m_Frustum = new Frustum;
	if (!m_Frustum)
	{
		return false;
	}

	//int terrainNum = 0;

	//for (vector<QuadTree*>::iterator it = m_QuadTree.begin(); it != m_QuadTree.end(); it++)
	//{
	//	result = (*it)->Initialize(m_Terrain[0], m_Direct3D->GetDevice());
	//	//Initialize the quad tree object.
	//	if (!result)
	//	{
	//		MessageBox(hwnd, L"Could not initialize the quad tree object.", L"Error", MB_OK);
	//		return false;
	//	}
	//	terrainNum++;
	//}

	// Create the color shader object.
	m_ShaderColor = new ShaderColor;
	if (!m_ShaderColor)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ShaderColor->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the timer object.
	m_Timer = new Timer;
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_CameraController = new CameraController;
	if (!m_CameraController)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_CameraController->SetPosition(cameraX, cameraY, cameraZ);

	// Create the fps object.
	m_Fps = new InfoFPS;
	if (!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new InfoCPU;
	if (!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the font shader object.
	m_ShaderFont = new ShaderFont;
	if (!m_ShaderFont)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_ShaderFont->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the text object.
	m_Text = new Text;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Retrieve the video card information.
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	m_ShaderTerrain = new ShaderTerrain;
	if (!m_ShaderTerrain)
	{
		return false;
	}

	// Initialize the terrain shader object.
	result = m_ShaderTerrain->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new Light;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, -1.0f, -0.5f);

	m_Objects.push_back(m_Light);

	// Initialize AntTweakBar
	if (!TwInit(TW_DIRECT3D11, m_Direct3D->GetDevice()))
	{
		MessageBoxA(hwnd, TwGetLastError(), "AntTweakBar initialization failed", MB_OK | MB_ICONERROR);
		return 0;
	}

	//// Create a tweak bar
	TwBar *bar = TwNewBar("Terrain Parameters");
	TwDefine(" GLOBAL help='Help Menu' "); // Message added to the help bar.
	int barSize[2] = { 300, 200 };
	int barPos[2] = { 1280 - barSize[1] - 110, 10 };

	//int barSize[2] = { 300, 500 };
	//int barPos[2] = { 10, 10 };

	//TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
	//TwSetParam(bar, NULL, "position", TW_PARAM_INT32, 2, barPos);
	//int width = 150; // pixels
	//TwSetParam(bar, NULL, "valueswidth", TW_PARAM_INT32, 1, &width);

	////TERRAIN
	//TwAddVarRW(bar, "Seed", TW_TYPE_INT32, m_Terrain[0]->GetSeed(), "group=TERRAIN");
	//TwAddVarRW(bar, "Amplitude", TW_TYPE_INT32, m_Terrain[0]->GetTerrainHeight(), "group=TERRAIN");
	//TwAddVarRW(bar, "Frequency", TW_TYPE_FLOAT, m_Terrain[0]->GetNoiseSize(), "group=TERRAIN");
	//TwAddVarRW(bar, "Persistence", TW_TYPE_FLOAT, m_Terrain[0]->GetPersistence(), "group=TERRAIN");
	//TwAddVarRW(bar, "Octaves", TW_TYPE_INT32, m_Terrain[0]->GetOctaves(), "group=TERRAIN");
	TwAddVarRW(bar, "Wireframe", TW_TYPE_BOOL32, m_Direct3D->GetWireframeValue(), "group=TERRAIN");
	TwAddVarRW(bar, "FPS", TW_TYPE_INT32, m_Fps->GetFPS(), "group=TERRAIN");


	////LIGHTING
	//TwAddVarRW(bar, "Direction", TW_TYPE_DIR3F, m_Light->GetDirection(), "group=LIGHTING");

	return true;
}

void Application::Shutdown()
{
	//if (m_QuadTree)
	//{
	//	m_QuadTree->Shutdown();
	//	delete m_QuadTree;
	//	m_QuadTree = 0;
	//}

	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the terrain shader object.
	if (m_ShaderTerrain)
	{
		m_ShaderTerrain->Shutdown();
		delete m_ShaderTerrain;
		m_ShaderTerrain = 0;
	}

	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the font shader object.
	if (m_ShaderFont)
	{
		m_ShaderFont->Shutdown();
		delete m_ShaderFont;
		m_ShaderFont = 0;
	}

	// Release the cpu object.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the position object.
	if (m_CameraController)
	{
		delete m_CameraController;
		m_CameraController = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the color shader object.
	if (m_ShaderColor)
	{
		m_ShaderColor->Shutdown();
		delete m_ShaderColor;
		m_ShaderColor = 0;
	}

	// Release the terrain object.
	//if (m_Terrain)
	//{
	//	m_Terrain->Shutdown();
	//	delete m_Terrain;
	//	m_Terrain = 0;
	//}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}

bool Application::Frame()
{
	bool result;

	// Read the user input.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Update the FPS value in the text object.
	result = m_Text->SetFPS(*m_Fps->GetFPS(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the CPU usage value in the text object.
	result = m_Text->SetCPU(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}


	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

	result = Update();
	if (!result) return false;


	m_Direct3D->SetRenderState();
	// Render the graphics.
	result = RenderGraphics();
	if (!result)
	{
		return false;
	}

	return result;
}

bool Application::HandleInput(float frameTime)
{
	bool keyDown, result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Set the frame time for calculating the updated position.
	m_CameraController->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_Input->IsAPressed();
	m_CameraController->MoveLeft(keyDown);

	keyDown = m_Input->IsDPressed();
	m_CameraController->MoveRight(keyDown);

	keyDown = m_Input->IsWPressed();
	m_CameraController->MoveForward(keyDown);

	keyDown = m_Input->IsSPressed();
	m_CameraController->MoveBackward(keyDown);

	keyDown = m_Input->IsEPressed();
	m_CameraController->MoveUpward(keyDown);

	keyDown = m_Input->IsQPressed();
	m_CameraController->MoveDownward(keyDown);

	keyDown = m_Input->IsUpPressed();
	m_CameraController->LookUpward(keyDown);

	keyDown = m_Input->IsDownPressed();
	m_CameraController->LookDownward(keyDown);

	keyDown = m_Input->IsLeftPressed();
	m_CameraController->LookLeft(keyDown);

	keyDown = m_Input->IsRightPressed();
	m_CameraController->LookRight(keyDown);

	// Get the view point position/rotation.
	m_CameraController->GetPosition(posX, posY, posZ);
	m_CameraController->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Update the position values in the text object.
	result = m_Text->SetCameraPosition(posX, posY, posZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the rotation values in the text object.
	result = m_Text->SetCameraRotation(rotX, rotY, rotZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	return true;
}

bool Application::Update()
{
	for (vector<Object*>::iterator it = m_Objects.begin(); it != m_Objects.end(); it++)
	{
		(*it)->Update();
	}
	return true;
}

bool Application::RenderGraphics()
{
	XMFLOAT4X4 worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// Clear the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	m_Terrain.clear();
	m_QuadTreeRootNode->GetVisibleTerrain(&m_Camera->GetPosition(), &m_Terrain);
	//m_QuadTreeRootNode->RenderTree(m_Direct3D->GetDeviceContext(), &m_Camera->GetPosition());
	
	// Set the terrain shader parameters that it will use for rendering.
	for (vector<Terrain*>::iterator it = m_Terrain.begin(); it != m_Terrain.end(); it++)
	{
		(*it)->Render(m_Direct3D->GetDeviceContext());
		result = m_ShaderTerrain->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, *m_Light->GetAmbientColor(), *m_Light->GetDiffuseColor(), *m_Light->GetDirection(), (*it)->GetTexture());
		result = m_ShaderTerrain->Render(m_Direct3D->GetDeviceContext(), (*it)->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, *m_Light->GetAmbientColor(), *m_Light->GetDiffuseColor(), *m_Light->GetDirection(), (*it)->GetTexture());
		//if (!result) return false;
	}

	// Render the terrain using the quad tree and terrain shader.
	int k = 0;
	/*for (vector<Terrain*>::iterator it = m_Terrain.begin(); it != m_Terrain.end(); it++)
	{
		if ((*it)->NeedsToUpdate())
		{
			(*it)->UpdateBuffers(m_Direct3D->GetDeviceContext());
		}
	}*/

	//for (vector<QuadTree*>::iterator it = m_QuadTree.begin(); it != m_QuadTree.end(); it++)
	//{
	//	(*it)->Render(&m_Camera->GetPosition(), m_Frustum, m_Direct3D->GetDeviceContext(), m_ShaderTerrain);
	//}

	// Set the number of rendered terrain triangles since some were culled.
	/*result = m_Text->SetRenderCount(m_QuadTree->GetDrawCount(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}*/

	// Render the terrain buffers.
	/*for (vector<Terrain*>::iterator it = m_Terrain.begin(); it != m_Terrain.end(); it++)
	{
		(*it)->Render(m_Direct3D->GetDeviceContext());

		if (!result)
		{
			return false;
		}
	}*/

	TwDraw();

	//if (m_Direct3D->GetWireframeValue())
	//{
	//	// Turn off the Z buffer to begin all 2D rendering.
	//	m_Direct3D->DisableZBuffer();

	//	// Turn on the alpha blending before rendering the text.
	//	m_Direct3D->TurnOnAlphaBlending();

	//	// Render the text user interface elements.
	///*	result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_ShaderFont, worldMatrix, orthoMatrix);
	//	if (!result)
	//	{
	//		return false;
	//	}*/

	//	// Turn off alpha blending after rendering the text.
	//	m_Direct3D->TurnOffAlphaBlending();

	//	// Turn the Z buffer back on now that all 2D rendering has completed.
	//	m_Direct3D->EnableZBuffer();
	//}


	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}
