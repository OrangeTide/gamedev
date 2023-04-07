#ifndef UNICODE
# define UNICODE
# define _UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define TINYLIB_OK (0)
#define TINYLIB_ERR (-1)

enum {
	TINYLIB_WM_APP_INITDONE = WM_APP, /* unblock rendering thread */
	TINYLIB_WM_APP_PAUSE, /* pauses rendering */
	TINYLIB_WM_APP_CONTINUE, /* continues rendering */
	TINYLIB_WM_APP_STOP, /* exit rendering thread */
	TINYLIB_WM_APP_RESIZE, /* resize viewport */
};

#ifndef TINYLIB_MAX_WINDOWS
#define TINYLIB_MAX_WINDOWS 5
#endif

typedef void (*render_func_t)(double elapsed, unsigned width, unsigned height);

struct win_info {
	BOOL alive;
	HWND hWnd;
	render_func_t render_cb;
	BOOL bFullscreen;
};

static int current_index = TINYLIB_ERR;
static struct win_info window[TINYLIB_MAX_WINDOWS];
static INT config_min_width, config_min_height;
static UINT _nRenderThreadID;

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

static int
find_next_window(void)
{
	int i;
	for (i = 0; i < TINYLIB_MAX_WINDOWS; i++) {
		if (window[i].alive == FALSE) {
			return i;
		}
	}

	return TINYLIB_ERR;
}

static int
find_win(HWND hWnd) {
	int i;
	for (i = 0; i < TINYLIB_MAX_WINDOWS; i++) {
		if (window[i].alive == TRUE && window[i].hWnd == hWnd) {
			return i;
		}
	}

	return TINYLIB_ERR;
}

static LRESULT CALLBACK
MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	int cur = find_win(hWnd);
	switch (Msg) {

	case TINYLIB_WM_APP_INITDONE:
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

	case WM_GETMINMAXINFO:
		/* restrict window resize if a mininum size is set */
		if (config_min_height > 0 && config_min_width > 0) {
			MINMAXINFO *pInfo = (MINMAXINFO *)lParam;
			pInfo->ptMinTrackSize.x = config_min_width;
			pInfo->ptMinTrackSize.y = config_min_height;
		}
		break;

	case WM_SIZE:

		/* pause rendering on minimize */
		switch (wParam) {
		case SIZE_MINIMIZED:
			/* pause rendering while minimized */
			render_pause();
			break;

		case SIZE_RESTORED:
			/* tell the render thread to resize viewport */
			PostThreadMessage(_nRenderThreadID, TINYLIB_WM_APP_RESIZE, 0, 0);

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
		// TODO: PostThreadMessage(_nRenderThreadID, TINYLIB_WM_APP_STOP, 0, 0);
		// TODO: WaitForSingleObject(_hRenderThread, 5000);

		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT: // TODO: Force a redraw through the render thread
		if (cur != TINYLIB_ERR && window[cur].render_cb) {
			// PAINTSTRUCT ps;
			// HDC hdc = BeginPaint(hWnd, &ps);
			// window[cur].render_cb(hWnd, hdc, &ps.rcPaint);
			// EndPaint(hWnd, &ps);
			window[cur].render_cb(0.0, 0, 0); // TODO: get elapsed time and width/height
		}
		return 0;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

static int
newwin(const char *title, int width, int height, render_func_t render_func)
{
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
			return TINYLIB_ERR;
		}
	}

	int win_index = find_next_window();

	if (win_index == TINYLIB_ERR) {
		MessageBox(NULL, TEXT("Too many open windows."), TEXT("Error"), MB_OK);
		return TINYLIB_ERR;
	}

#ifdef UNICODE
        static WCHAR title_buf[256];
        int len;
        len = MultiByteToWideChar(CP_UTF8, 0, title, -1, title_buf, sizeof(title_buf) / sizeof(*title_buf));
        if (!len) {
		title_buf[0] = 0; // ERROR
		MessageBox(NULL, TEXT("Invalidate window title string"), TEXT("Error"), MB_OK);
		return TINYLIB_ERR;
	}
#else
        char *title_buf = title; // pass it through
	// TODO: utf8 to local encoding
#endif

	HWND hwnd = CreateWindow(MAKEINTATOM(wndcls), title_buf,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, TEXT("CreateWindow() failed: Cannot create OpenGL window."), TEXT("Error"), MB_OK);
		return TINYLIB_ERR;
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
	window[win_index] = (struct win_info){ .hWnd = hwnd, .render_cb = render_func };

	/* select the new window */
	current_index = win_index;

	// TODO: create new context

	// TODO: move GL context creation to this thread
	// _hRenderThread = (HANDLE)_beginthreadex(NULL, 0, RenderMain, &args, 0, &_nRenderThreadID);

	// TODO: clean up old window and old context

	return win_index;
}

static int
delwin(int win_index) {
	if (win_index < 0 || win_index >= TINYLIB_MAX_WINDOWS || !window[win_index].alive) {
		return TINYLIB_ERR;
	}

	DestroyWindow(window[win_index].hWnd);
	window[win_index].hWnd = NULL; // TODO: move into WM_DESTROY handler
	window[win_index].alive = FALSE;
	if (current_index == win_index) {
		current_index = TINYLIB_ERR;
	}

	return TINYLIB_OK;
}

/* selects a window for future operations */
static int
selectwin(int win_index)
{
	if (win_index < 0 || win_index >= TINYLIB_MAX_WINDOWS || !window[win_index].alive) {
		return TINYLIB_ERR;
	}

	current_index = win_index;

	return TINYLIB_OK;
}

static int
loop(void)
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// TODO: if (_hRenderThread != NULL) CloseHandle(_hRenderThread);

	return (int)msg.wParam;
}


static void
my_paint(double elapsed, unsigned width, unsigned height)
{
	(void)elapsed;
	(void)width;
	(void)height;

	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PWSTR pCmdLine, int nCmdShow)
{
	(void)hInstance;
	(void)prevInstance;
	(void)pCmdLine;
	(void)nCmdShow;

	newwin(__FILE__, 1024, 768, my_paint);

	// TODO: test selectwin();
	// TODO: test delwin();

	return loop();
}
