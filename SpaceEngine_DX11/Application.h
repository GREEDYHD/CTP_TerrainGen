#ifndef _Application_H_
#define _Application_H_

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#include "Input.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Terrain.h"
#include "ShaderColor.h"
#include "Timer.h"
#include "CameraController.h"
#include "InfoFPS.h"
#include "InfoCPU.h"
#include "ShaderFont.h"
#include "Text.h"
#include "ShaderTerrain.h"
#include "Light.h"
#include "Frustum.h"
#include "QuadTree.h"

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float);
	bool RenderGraphics();

private:
	Input* m_Input;
	Direct3D* m_Direct3D;
	Camera* m_Camera;
	Terrain* m_Terrain;
	ShaderColor* m_ShaderColor;
	Timer* m_Timer;
	CameraController* m_CameraController;
	InfoFPS* m_Fps;
	InfoCPU* m_Cpu;
	ShaderFont* m_ShaderFont;
	Text* m_Text;
	ShaderTerrain* m_ShaderTerrain;
	Light* m_Light;
	Frustum* m_Frustum;
	QuadTree* m_QuadTree;
};

#endif