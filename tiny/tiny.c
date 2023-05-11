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
static EGLContext eglContext;
static int dirty = TRUE;

static void errormsg(const char *m, ...)
{
	static char buf[1024];
	va_list ap;
	va_start(ap, m);
	snprintf(buf, sizeof(buf), m, ap);
	va_end(ap);
	MessageBoxA(NULL, buf, "Error", MB_OK);
}

static void check_gl_error(void)
{
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {
		errormsg("GL Error 0x%04X", (unsigned)e);
	}
}

static void check_egl_error(void)
{
	GLenum e = eglGetError();
	if (e != EGL_SUCCESS) {
		errormsg("EGL Error 0x%04X", (unsigned)e);
	}
}

static void check_error(void)
{
	check_egl_error();
	check_gl_error();
}

static void setup(void)
{
	printf("Initializing scene ...\n");
//	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
}

static void paint(double timeFactor)
{
	(void)timeFactor;

	printf("Painting ...\n");
	glViewport(0, 0, width, height);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	// glDrawArrays(GL_TRIANGLE_STRIP, 0, 26);

	// glFlush();
}

LRESULT CALLBACK wndProc(HWND hwnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg) {
#if 0
	PAINTSTRUCT ps;

	case WM_PAINT:
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
#endif

	case WM_CLOSE :
		quit = TRUE;
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		printf("WM_SIZE\n");
		return 0;
	}
	return (DefWindowProc(hwnd, msg, wParam, lParam));
}

static int newwin(void)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wcex = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_OWNDC,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = NULL,
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.hbrBackground = 0,
		.lpszMenuName = NULL,
		.lpszClassName = L"eglsamplewnd",
		.hIconSm = NULL,
		.lpfnWndProc = wndProc,
	};

	RegisterClassEx(&wcex);
	RECT rect = { 0, 0, width, height };
	int style = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
	AdjustWindowRect(&rect, style, FALSE);

	HWND hwnd = CreateWindow(L"eglsamplewnd", L"Tiny EGL example", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, SW_SHOW);

	HDC hdc = GetDC(hwnd);
	eglDisplay = eglGetDisplay(hdc);
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

	eglSurface = eglCreateWindowSurface(eglDisplay, windowConfig, hwnd, surfaceAttributes);
	if (!nrOfConfigs || eglSurface == EGL_NO_SURFACE) {
		errormsg("Could not create EGL surface");
		// TODO: tear down eglDisplay and hwnd
		return ERR;
	}

	eglContext = eglCreateContext(eglDisplay, windowConfig, NULL, contextAttributes);
	if (eglContext == EGL_NO_CONTEXT) {
		errormsg("Could not create EGL context");
		// TODO: tear down eglSurface, eglDisplay and hwnd
		return ERR;
	}

	check_egl_error();

	printf("EGL_VERSION=%s\n", eglQueryString(eglDisplay, EGL_VERSION));
	printf("EGL_VENDOR=%s\n", eglQueryString(eglDisplay, EGL_VENDOR));
	printf("EGL_CLIENT_APIS=%s\n", eglQueryString(eglDisplay, EGL_CLIENT_APIS));
	printf("EGL_EXTENSIONS=%s\n", eglQueryString(eglDisplay, EGL_EXTENSIONS));

#if 0 // HACK

	int eglNumConfigs;
	eglGetConfigs(eglDisplay, NULL, 0, &eglNumConfigs);
	EGLConfig eglConfigs[eglNumConfigs];

	for (int i = 0; i < eglNumConfigs; i++) {
		printf("Config %d\n", i);
		printf("Supported APIs :");
		int eglRenderable;
		eglGetConfigAttrib(eglDisplay, eglConfigs[i], EGL_RENDERABLE_TYPE, &eglRenderable);
		if (eglRenderable & EGL_OPENGL_ES_BIT) printf(" OPENGL ES");
		if (eglRenderable & EGL_OPENGL_ES2_BIT) printf(" OPENGL ES2");
		if (eglRenderable & EGL_OPENVG_BIT) printf(" OPENVG");
		if (eglRenderable & EGL_OPENGL_BIT) printf(" OPENGL");
		printf("\n");
	}

	EGLint attr[] = {
		EGL_BUFFER_SIZE, 16,
		EGL_RENDERABLE_TYPE,
		EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLConfig eglConfig;
	int eglNumConfig;
	if (!eglChooseConfig(eglDisplay, attr, &eglConfig, sizeof(eglConfig), &eglNumConfig)) {
		errormsg("Could not get valid EGL configuration!");
		return ERR;
	}

#endif

	return OK;
}

static int init(void)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&qpcFrequency);

	if (newwin() != OK) {
		return ERR;
	}

	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

	printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
	printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
	printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
	printf("GL_SHADING_LANGUAGE_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	setup();

	check_error();


	return OK;
}

static void loop(void)
{
	double timeFactor = 1.0f;
	MSG uMsg;

	LONGLONG qpcStart, qpcEnd;

	while (!quit)  {
		QueryPerformanceCounter((LARGE_INTEGER*)&qpcStart);

		if (dirty) {
			while (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE) > 0) {
				printf("MSG %d\n", uMsg.message);
				TranslateMessage(&uMsg);
				DispatchMessage(&uMsg);
			}
		} else {
			while (GetMessage(&uMsg, NULL, 0, 0)) {
				printf("MSG %d\n", uMsg.message);
				TranslateMessage(&uMsg);
				DispatchMessage(&uMsg);
			}
		}

		dirty = FALSE;
		paint(timeFactor);
		eglSwapBuffers(eglDisplay, eglSurface);
		check_error();

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
