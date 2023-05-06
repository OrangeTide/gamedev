#ifndef UNICODE
# define UNICODE
# define _UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "initgl.h"

/**********************************************************************/

#ifndef INITGL_MAX_WINDOWS
#define INITGL_MAX_WINDOWS 4
#endif

#if 0
enum {
	INITGL_WM_APP_INITDONE = WM_APP, /* unblock rendering thread */
	INITGL_WM_APP_PAUSE, /* pauses rendering */
	INITGL_WM_APP_CONTINUE, /* continues rendering */
	INITGL_WM_APP_STOP, /* exit rendering thread */
	INITGL_WM_APP_RESIZE, /* resize viewport */
};
#endif

struct win_info {
	BOOL alive;
	BOOL bFullscreen;
	HWND hWnd;
	EGLContext eglContext;
	EGLSurface surface;
	struct window_callback_functions callback;
	// TODO: INT config_min_width, config_min_height; /* restrict window resize if a mininum size is set */
};

/**********************************************************************/

static int current_index = INITGL_ERR;
static struct win_info window[INITGL_MAX_WINDOWS];
static int default_width = 640, default_height = 480;
static EGLDisplay eglDisplay = EGL_NO_DISPLAY;

int terminate_flag;

/**********************************************************************/

/* find next unused window */
static int
find_next_window(void)
{
	int i;
	for (i = 0; i < INITGL_MAX_WINDOWS; i++) {
		if (window[i].alive == FALSE) {
			return i;
		}
	}

	return INITGL_ERR;
}

/* find index of matching window handle */
static int
find_win(HWND hWnd) {
	int i;
	for (i = 0; i < INITGL_MAX_WINDOWS; i++) {
		if (window[i].alive == TRUE && window[i].hWnd == hWnd) {
			return i;
		}
	}

	return INITGL_ERR;
}

static void
render_pause(void)
{
	// TODO: PostThreadMessage(_nRenderThreadID, TINYLIB_WM_APP_PAUSE, TRUE, 0);
}

static void
render_continue(void)
{
	// TODO: PostThreadMessage(_nRenderThreadID, TINYLIB_WM_APP_CONTINUE, FALSE, 0);
}

static LRESULT CALLBACK
MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	int cur = find_win(hWnd);
	struct win_info *info = cur < 0 ? NULL : &window[cur];
	switch (Msg) {

#if 0 // TODO: support a render thread
	case INITGL_WM_APP_INITDONE:
		/* render thread is ready */
		if (wParam) { /* full screen window? */
			ShowWindow(hWnd, SW_MAXIMIZE);
		} else {
			ShowWindow(hWnd, SW_SHOW);
		}

		/* take the focus */
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);

		return 1; /* handled */
#endif

#if 0 // TODO: implement this
	case WM_GETMINMAXINFO:
		/* restrict window resize if a mininum size is set */
		if (window[cur].config_min_height > 0 && window[cur].config_min_width > 0) {
			MINMAXINFO *pInfo = (MINMAXINFO *)lParam;
			pInfo->ptMinTrackSize.x = window[cur].config_min_width;
			pInfo->ptMinTrackSize.y = window[cur].config_min_height;
		}
		break;
#endif

	case WM_SIZE:

		/* pause rendering on minimize */
		switch (wParam) {
		case SIZE_MINIMIZED:
			/* pause rendering while minimized */
			render_pause();
			break;

		case SIZE_RESTORED:
			/* tell the render thread to resize viewport */
			// TODO: PostThreadMessage(_nRenderThreadID, INITGL_WM_APP_RESIZE, 0, 0);

			/* unblock render thread */
			render_continue();
			break;
		}

		break;

	case WM_ENTERMENULOOP:
		/* suspend while a menu is accessed */
		if (!IsIconic(hWnd)) {
			render_pause();
		}
		break;

	case WM_EXITMENULOOP:
		/* restore from the menu triggered suspend */
		if (!IsIconic(hWnd)) {
			render_continue();
		}
		break;

	case WM_KEYDOWN:
		break;

	case WM_CLOSE:
		// TODO: save window size and position

		/* terminate render thread */
		// TODO: PostThreadMessage(_nRenderThreadID, INITGL_WM_APP_STOP, 0, 0);
		// TODO: WaitForSingleObject(_hRenderThread, 5000);

		if (eglDisplay != EGL_NO_DISPLAY) {
			if (info->eglContext != EGL_NO_CONTEXT) {
				eglDestroyContext(eglDisplay, info->eglContext);
				info->eglContext = EGL_NO_CONTEXT;
			}
			if (info->surface != EGL_NO_SURFACE) {
				eglDestroySurface(eglDisplay, info->surface);
				info->surface = EGL_NO_SURFACE;
			}
		}
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT: // TODO: Force a redraw through the render thread
		if (cur != INITGL_ERR && window[cur].callback.paint) {
			// PAINTSTRUCT ps;
			// HDC hdc = BeginPaint(hWnd, &ps);
			// window[cur].render_cb(hWnd, hdc, &ps.rcPaint);
			// EndPaint(hWnd, &ps);
			window[cur].callback.paint();
		}
		return 0;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int
window_new(const struct window_callback_functions *callback)
{
	const char *title = "TODO"; // TODO: configure this
	static ATOM wndcls;
	static HINSTANCE hInstance;

	if (!hInstance) {
		hInstance = GetModuleHandle(NULL);
	}

	if (!wndcls) {
		WNDCLASSEX wcx = { 0 };

		wcx.cbSize = sizeof(wcx);
		wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcx.lpfnWndProc = MainWndProc;
		wcx.cbClsExtra = 0;
		wcx.cbWndExtra = 0;
		wcx.hInstance = hInstance;
		wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcx.hbrBackground = GetStockObject(WHITE_BRUSH);
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = TEXT(__FILE__);
		wcx.hIconSm = LoadImage(hInstance, MAKEINTRESOURCE(5),
					IMAGE_ICON,
					GetSystemMetrics(SM_CXSMICON),
					GetSystemMetrics(SM_CYSMICON),
					LR_DEFAULTCOLOR);

		wndcls = RegisterClassEx(&wcx);
		if (!wndcls) {
			MessageBox(NULL, TEXT("RegisterClassEx() failed: Cannot register window class."), TEXT("Error"), MB_OK);
			return INITGL_ERR;
		}
	}

	int win_index = find_next_window();

	if (win_index == INITGL_ERR) {
		MessageBox(NULL, TEXT("Too many open windows."), TEXT("Error"), MB_OK);
		return INITGL_ERR;
	}

#ifdef UNICODE
        static WCHAR title_buf[256];
        int len;
        len = MultiByteToWideChar(CP_UTF8, 0, title, -1, title_buf, sizeof(title_buf) / sizeof(*title_buf));
        if (!len) {
		title_buf[0] = 0; // ERROR
		MessageBox(NULL, TEXT("Invalidate window title string"), TEXT("Error"), MB_OK);
		return INITGL_ERR;
	}
#else
        char *title_buf = title; // pass it through
	// TODO: utf8 to local encoding
#endif

	HWND hwnd = CreateWindow(MAKEINTATOM(wndcls), title_buf,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		default_width, default_height, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, TEXT("CreateWindow() failed: Cannot create OpenGL window."), TEXT("Error"), MB_OK);
		return INITGL_ERR;
	}

	HDC hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize        = sizeof(pfd);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = 32;

	int pf = ChoosePixelFormat(hdc, &pfd);

	SetPixelFormat(hdc, pf, &pfd);

	DescribePixelFormat(hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	ReleaseDC(hwnd, hdc);

	// TODO: initialize bootstrap context in this thread

	// TODO: create real window for the new context that will be created by render thread

	/* add the new window to the list */
	window[win_index] = (struct win_info){ .hWnd = hwnd, .callback = *callback };

	/* select the new window */
	current_index = win_index;

	// TODO: move into display_init(). create a dummy window to initialize a display

	if (eglDisplay == EGL_NO_DISPLAY) {
		// Create EGL display connection
		eglDisplay = eglGetDisplay(hdc);

		// Initialize EGL for this display, returns EGL version
		EGLint eglVersionMajor, eglVersionMinor;
		eglInitialize(eglDisplay, &eglVersionMajor, &eglVersionMinor);
		fprintf(stderr, "EGL version %d.%d", eglVersionMajor, eglVersionMinor);
	}

	EGLint configAttributes[] = {
		EGL_BUFFER_SIZE, 0,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_ALPHA_SIZE, 0,
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_DEPTH_SIZE, 24,
		EGL_LEVEL, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_SAMPLES, 0,
		EGL_STENCIL_SIZE, 0,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_TRANSPARENT_TYPE, EGL_NONE,
		EGL_TRANSPARENT_RED_VALUE, EGL_DONT_CARE,
		EGL_TRANSPARENT_GREEN_VALUE, EGL_DONT_CARE,
		EGL_TRANSPARENT_BLUE_VALUE, EGL_DONT_CARE,
		EGL_CONFIG_CAVEAT, EGL_DONT_CARE,
		EGL_CONFIG_ID, EGL_DONT_CARE,
		EGL_MAX_SWAP_INTERVAL, EGL_DONT_CARE,
		EGL_MIN_SWAP_INTERVAL, EGL_DONT_CARE,
		EGL_NATIVE_RENDERABLE, EGL_DONT_CARE,
		EGL_NATIVE_VISUAL_TYPE, EGL_DONT_CARE,
		EGL_NONE
	};

	EGLint numConfigs;
	EGLConfig windowConfig;
	eglChooseConfig(eglDisplay, configAttributes, &windowConfig, 1, &numConfigs);

	EGLint surfaceAttributes[] = { EGL_NONE };
	EGLSurface eglSurface = eglCreateWindowSurface(eglDisplay, windowConfig, hwnd, surfaceAttributes);

	EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	EGLContext eglContext = eglCreateContext(eglDisplay, windowConfig, NULL, contextAttributes);

	window[win_index].surface = eglSurface;
	window[win_index].eglContext = eglContext;

	// TODO: move GL context creation to this thread
	// _hRenderThread = (HANDLE)_beginthreadex(NULL, 0, RenderMain, &args, 0, &_nRenderThreadID);

	// TODO: clean up old window and old context

#if 1 // TODO: remove this once we have implemented a render thread
	ShowWindow(hwnd, SW_SHOW);

	/* take the focus */
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
#endif

	return win_index;
}

int
display_init(void)
{

	return INITGL_OK;
}

void
display_done(void)
{
	if (eglDisplay != EGL_NO_DISPLAY) {
		eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	}

	unsigned i;

	for (i = 0; i < INITGL_MAX_WINDOWS; i++) {
		struct win_info *info = &window[i];
		if (info->alive == TRUE) {
			fprintf(stderr, "TODO: clean up window %d", i);

			if (eglDisplay != EGL_NO_DISPLAY) {
				eglDestroyContext(eglDisplay, info->eglContext);
				info->eglContext = EGL_NO_CONTEXT;

				eglDestroySurface(eglDisplay, info->surface);
				info->surface = EGL_NO_SURFACE;
			}
		}
	}

	if (eglDisplay != EGL_NO_DISPLAY) {
		eglTerminate(eglDisplay);
		eglDisplay = EGL_NO_DISPLAY;
	}
}

void
process_events(void)
{
	MSG msg;

	while (!terminate_flag && GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// TODO: eglSwapBuffers(eglDisplay, eglSurface);
	// TODO: if (_hRenderThread != NULL) CloseHandle(_hRenderThread);
}

void
paint_all(void)
{
	if (eglDisplay == EGL_NO_DISPLAY) {
		return;
	}

	unsigned i;

	for (i = 0; i < INITGL_MAX_WINDOWS; i++) {
		struct win_info *info = &window[i];
		if (info->alive == TRUE) {
			if (info->callback.paint) {
				info->callback.paint();
			}
			eglSwapBuffers(eglDisplay, info->surface);
		}
	}
}

int
lookup_key(const char *s)
{
	const struct { BYTE code; char name[15]; } codes[] = {
		{ VK_LEFT, "Left" },
		{ VK_UP, "Up" },
		{ VK_RIGHT, "Right" },
		{ VK_DOWN, "Down" },
		{ VK_OEM_4, "[{" }, // TODO: use the X11 name
		{ VK_OEM_5, "\\|" }, // TODO: use the X11 name
		{ VK_OEM_6, "]}" }, // TODO: use the X11 name
	};

	unsigned i;
	for (i = 0; i < sizeof(codes) / sizeof(*codes); i++) {
		// TODO: strcasecmp()
		if (!strcmp(codes[i].name, s)) {
			return codes[i].code;
		}
	}

	/* take a guess at single letter codes - not always correct! */
	if (s[0] || !s[1]) {
		return toupper(s[0]);
	}

	return -1;
}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PWSTR pCmdLine, int nCmdShow)
{
	(void)hInstance;
	(void)prevInstance;
	(void)pCmdLine;
	(void)nCmdShow;

	// TODO: load and initialize libraries

	extern int main(int argc, char *argv[]);
	char *argv[] = { "TODO" }; // TODO: implement this
	return main(1, argv);
}
