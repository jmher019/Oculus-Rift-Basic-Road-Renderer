#include "..\include\Window.h"

Window::Window(void) :
	window(nullptr),
	hDC(nullptr),
	wglContext(nullptr),
	gleContext(),
	Running(false),
	winSizeW(0),
	winSizeH(0),
	fboId(0),
	hInstance(nullptr) {
	// Clear input
	for (int i = 0; i < sizeof(Key) / sizeof(Key[0]); i++)
		Key[i] = false;
}

Window::~Window(void) {
	ReleaseDevice();
	CloseWindow();
}

bool Window::InitWindow(HINSTANCE hInst, LPCWSTR title) {
	hInstance = hInst;
	Running = true;

	WNDCLASSW wc;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbWndExtra = sizeof(Window *);
	wc.hInstance = GetModuleHandleW(NULL);
	wc.lpszClassName = L"ORR";
	RegisterClassW(&wc);

	// adjust the window size and show at InitDevice time
	window = CreateWindowW(wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, 0, hInstance, 0);
	if (!window) return false;

	SetWindowLongPtr(window, 0, LONG_PTR(this));
	
	hDC = GetDC(window);

	return true;
}

void Window::CloseWindow(void) {
	if (window) {
		if (hDC) {
			ReleaseDC(window, hDC);
			hDC = nullptr;
		}
		DestroyWindow(window);
		window = nullptr;
		UnregisterClassW(L"Window", hInstance);
	}
}

bool Window::InitDevice(int vpW, int vpH, const LUID *pLuid, bool windowed) {
	UNREFERENCED_PARAMETER(windowed);

	winSizeW = vpW;
	winSizeH = vpH;

	RECT size = { 0, 0, vpW, vpH };
	AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, false);
	const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
	if (!SetWindowPos(window, nullptr, 0, 0, size.right - size.left, size.bottom - size.top, flags))
		return false;

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARBFunc = nullptr;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARBFunc = nullptr;
	{
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 16;
		int pf = ChoosePixelFormat(hDC, &pfd);
		VALIDATE(pf, "Failed to choose pixel format");

		VALIDATE(SetPixelFormat(hDC, pf, &pfd), "Failed to set pixel format");

		HGLRC context = wglCreateContext(hDC);
		VALIDATE(context, "wglCreateContextFailed");
		VALIDATE(wglMakeCurrent(hDC, context), "wglMakeCurrent failed.");

		wglChoosePixelFormatARBFunc = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		wglCreateContextAttribsARBFunc = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		assert(wglChoosePixelFormatARBFunc && wglCreateContextAttribsARBFunc);

		wglDeleteContext(context);
	}

	// Now create the real context that we will be using.
	int iAttributes[] =
	{
		// WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 16,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
		0, 0
	};

	float fAttributes[] = { 0, 0 };
	int   pf = 0;
	UINT  numFormats = 0;

	VALIDATE(wglChoosePixelFormatARBFunc(hDC, iAttributes, fAttributes, 1, &pf, &numFormats),
		"wglChoosePixelFormatARBFunc failed.");

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	VALIDATE(SetPixelFormat(hDC, pf, &pfd), "SetPixelFormat failed.");

	GLint attribs[16];
	int   attribCount = 0;
	if (debug)
	{
		attribs[attribCount++] = WGL_CONTEXT_FLAGS_ARB;
		attribs[attribCount++] = WGL_CONTEXT_DEBUG_BIT_ARB;
	}

	attribs[attribCount] = 0;

	wglContext = wglCreateContextAttribsARBFunc(hDC, 0, attribs);
	VALIDATE(wglMakeCurrent(hDC, wglContext), "wglMakeCurrent failed.");

	OVR::GLEContext::SetCurrentContext(&gleContext);
	gleContext.Init();

	glGenFramebuffers(1, &fboId);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);

	if (debug && GLE_ARB_debug_output)
	{
		glDebugMessageCallbackARB(DebugGLCallback, NULL);
		if (glGetError())
		{
			OVR_DEBUG_LOG(("glDebugMessageCallbackARB failed."));
		}

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

		// Explicitly disable notification severity output.
		glDebugMessageControlARB(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	}

	return true;
}

bool Window::HandleMessages(void) {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return Running;
}

void Window::Run(bool (*MainLoop)(bool retryCreate)) {
	while (HandleMessages()) {
		// true-> we'll attempt to retry for ovrError_DisplayLost
		if (!MainLoop(true))
			break;
		// Sleep a bit before retrying to reduce CPU load while the HMD is disconnected
		Sleep(10);
	}
}

void Window::ReleaseDevice() {
	if (fboId) {
		glDeleteFramebuffers(1, &fboId);
		fboId = 0;
	}
	if (wglContext) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(wglContext);
		wglContext = nullptr;
	}
	gleContext.Shutdown();
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	Window *w = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, 0));
	switch (Msg) {
	case WM_KEYDOWN:
		w->Key[wParam] = true;
		break;
	case WM_KEYUP:
		w->Key[wParam] = false;
		break;
	case WM_DESTROY:
		w->Running = false;
		break;
	default:
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	if ((w->Key['Q'] && w->Key[VK_CONTROL]) || w->Key[VK_ESCAPE]) {
		w->Running = false;
	}
	return 0;
}

void GLAPIENTRY Window::DebugGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	UNREFERENCED_PARAMETER(source);
	UNREFERENCED_PARAMETER(type);
	UNREFERENCED_PARAMETER(id);
	UNREFERENCED_PARAMETER(severity);
	UNREFERENCED_PARAMETER(length);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(userParam);
	OVR_DEBUG_LOG(("Message from OpenGL: %s\n", message));
}