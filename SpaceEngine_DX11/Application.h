#ifndef _Application_H_
#define _Application_H_

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 3000.0f; //Change render distance
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
#include "Object.h"
#include "AntTweakBar.h"

#include <vector>

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	bool Update();

private:
	bool HandleInput(float);
	bool RenderGraphics();
	vector<Object*> m_Objects;

private:
	Input* m_Input;
	Direct3D* m_Direct3D;
	Camera* m_Camera;
	vector<Terrain*> m_Terrain;
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
	vector<QuadTree*> m_QuadTree;
	PerlinNoise* m_PerlinNoise;

};
#endif