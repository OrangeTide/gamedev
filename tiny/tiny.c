#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif
#include <windows.h>
#include <tchar.h>
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdarg.h>

#define ERR (-1)
#define OK (0)

static BOOL quit = FALSE;
static int width = 800, height = 600;
static LONGLONG qpcFrequency;
static EGLDisplay eglDisplay;
static EGLSurface eglSurface;

static void errormsg(const char *m, ...)
{
	static char buf[1024];
	va_list ap;
	va_start(ap, m);
	snprintf(buf, sizeof(buf), m, ap);
	va_end(ap);
	MessageBoxA(NULL, buf, "Error", MB_OK);
}

static void setup(void)
{
	printf("Initializing scene ...\n");
//	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.9f, 0.9f, 1.0f);
}

static void paint(double timeFactor)
{
	(void)timeFactor;

	glViewport(0, 0, width, height);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	// glDrawArrays(GL_TRIANGLE_STRIP, 0, 26);

	glFlush();
}

LRESULT CALLBACK wndProc(HWND hwnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CLOSE :
		quit = TRUE;
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		return 0;
	}

	return (DefWindowProc(hwnd, msg, wParam, lParam));
}

static int newwin(void)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = &DefWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"eglsamplewnd";
	wcex.hIconSm = NULL;
	wcex.lpfnWndProc = wndProc;

	RegisterClassEx(&wcex);
	RECT rect = { 0, 0, width, height };
	int style = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
	AdjustWindowRect(&rect, style, FALSE);

	HWND hwnd = CreateWindow(L"eglsamplewnd", L"EGL OpenGL ES 2.0 example", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, SW_SHOW);

	HDC hdc = GetDC(hwnd);
	EGLDisplay eglDisplay = eglGetDisplay(hdc);
	if (eglDisplay == EGL_NO_DISPLAY) {
		errormsg("No EGL display");
		return ERR;
	}

	EGLint eglVersionMajor, eglVersionMinor;
	eglInitialize(eglDisplay, &eglVersionMajor, &eglVersionMinor);
	eglBindAPI(EGL_OPENGL_ES_API);

	EGLint configAttributes[] = {
		EGL_BUFFER_SIZE, 0,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_ALPHA_SIZE, 0,
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_CONFIG_CAVEAT, EGL_DONT_CARE,
		EGL_CONFIG_ID, EGL_DONT_CARE,
		EGL_DEPTH_SIZE, 24,
		EGL_LEVEL, 0,
		EGL_MAX_SWAP_INTERVAL, EGL_DONT_CARE,
		EGL_MIN_SWAP_INTERVAL, EGL_DONT_CARE,
		EGL_NATIVE_RENDERABLE, EGL_DONT_CARE,
		EGL_NATIVE_VISUAL_TYPE, EGL_DONT_CARE,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_SAMPLES, 0,
		EGL_STENCIL_SIZE, 0,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_TRANSPARENT_TYPE, EGL_NONE,
		EGL_TRANSPARENT_RED_VALUE, EGL_DONT_CARE,
		EGL_TRANSPARENT_GREEN_VALUE, EGL_DONT_CARE,
		EGL_TRANSPARENT_BLUE_VALUE, EGL_DONT_CARE,
		EGL_NONE
	};

	EGLint surfaceAttributes[] = { EGL_NONE };
	EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

	EGLint nrOfConfigs;
	EGLConfig windowConfig;
	if (!eglChooseConfig(eglDisplay, configAttributes, &windowConfig, 1, &nrOfConfigs)) {
		errormsg("No valid EGL configs");
		// TODO: tear down eglDisplay and hwnd
		return ERR;
	}

	EGLSurface eglSurface = eglCreateWindowSurface(eglDisplay, windowConfig, hwnd, surfaceAttributes);
	if (!nrOfConfigs || eglSurface == EGL_NO_SURFACE) {
		errormsg("Could not create EGL surface");
		// TODO: tear down eglDisplay and hwnd
		return ERR;
	}

	EGLContext eglContext = eglCreateContext(eglDisplay, windowConfig, NULL, contextAttributes);

	if (eglContext == EGL_NO_CONTEXT) {
		errormsg("Could not create EGL context");
		// TODO: tear down eglSurface, eglDisplay and hwnd
		return ERR;
	}

	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

	setup();

	return OK;
}

static int init(void)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&qpcFrequency);

	if (newwin() != OK) {
		return ERR;
	}

	return OK;
}

static void loop(void)
{
	double timeFactor = 1.0f;
	MSG uMsg;

#if 0
	if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE) > 0) {
		TranslateMessage(&uMsg);
		DispatchMessage(&uMsg);
	}
#endif

	LONGLONG qpcStart, qpcEnd;

	while (!quit)  {
		QueryPerformanceCounter((LARGE_INTEGER*)&qpcStart);

		printf("Painting...\n");
		paint(timeFactor);

#if 0
		while (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE) > 0) {
			printf("MSG %d\n", uMsg.message);
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
#else
		while (GetMessage(&uMsg, NULL, 0, 0)) {
			printf("MSG %d\n", uMsg.message);
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
#endif

		eglSwapBuffers(eglDisplay, eglSurface);

		QueryPerformanceCounter((LARGE_INTEGER*)&qpcEnd);
		double dTime = (double)(qpcEnd - qpcStart) / (double)qpcFrequency;
		timeFactor += dTime * 0.01f;
	}
}

static void done(void)
{
	// TODO:
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;

	if (init() != OK) {
		return 1;
	}

	loop();
	done();

	return 0;
}
