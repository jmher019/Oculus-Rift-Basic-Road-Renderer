#ifndef WINDOW_H
#define WINDOW_H
// Include the Oculus SDK
#include "..\..\lib\LibOVR\Include\OVR_CAPI_GL.h"

#include "..\..\lib\LibOVRKernel\Src\GL\CAPI_GLE.h"
#include "..\..\lib\LibOVRKernel\Src\GL\CAPI_GLE_GL.h"
#include <assert.h>

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

class Window {
public:
	static const bool debug = false;
	HWND						window;
	HDC							hDC;
	HGLRC						wglContext;
	OVR::GLEContext					gleContext;
	bool						Running;
	bool						Key[256];
	int							winSizeW;
	int							winSizeH;
	GLuint						fboId;
	HINSTANCE					hInstance;

	Window(void);
	~Window(void);
	
	bool InitWindow(HINSTANCE hInst, LPCWSTR title);
	void CloseWindow(void);
	bool InitDevice(int vpW, int vpH, const LUID* pLuid, bool windowed = true);
	bool HandleMessages(void);
	void Run(bool (*MainLoop)(bool retryCreate));
	void ReleaseDevice(void);

	static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	static void GLAPIENTRY DebugGLCallback(GLenum soruce, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

};
#endif // !WINDOW_H