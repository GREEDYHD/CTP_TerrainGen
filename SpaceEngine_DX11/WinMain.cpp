#include "System.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	System* mSystem;
	bool result;


	// Create the system object.
	mSystem = new System;
	if (!mSystem)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = mSystem->Initialize();
	if (result)
	{
		mSystem->Run();
	}

	// Shutdown and release the system object.
	mSystem->Shutdown();
	delete mSystem;
	mSystem = 0;

	return 0;
}