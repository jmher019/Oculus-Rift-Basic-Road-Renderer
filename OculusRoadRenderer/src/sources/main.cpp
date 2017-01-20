// Include the Oculus SDK
#include "..\..\lib\LibOVR\Include\OVR_CAPI_GL.h"

#if defined(_WIN32)
	#include <dxgi.h> // for GetDefaultAdapterLuid
	#pragma comment(lib, "dxgi.lib")
#endif

#ifndef VALIDATE
	#define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "OculusRoadRenderer", MB_ICONERROR | MB_OK); exit(-1); }
#endif

#ifndef OVR_DEBUG_LOG
	#define OVR_DEBUG_LOG(x)
#endif

#include <iostream>

// Global OpenGL state
//static OGL Platform;

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int) {
	AllocConsole();
	freopen("CONOUT$", "w+", stdout);

	// Initializes LibOVR, and the Rift
	ovrInitParams initParams = { ovrInit_RequestVersion, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	//VALIDATE(Platform.InitWindow(hinst, L"Oculus Room Tiny (GL)"), "Failed to open window.");

	//Platform.Run(MainLoop);

	ovr_Shutdown();

	return(0);
}