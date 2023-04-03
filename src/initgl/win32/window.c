// TODO:

    // Create a new window for render output
    HWND hwnd = createWindow(winWidth, winHeight);
    // Get device context from the window
    HDC hdc = GetDC(hwnd);
    // Create EGL display connection
    EGLDisplay eglDisplay = eglGetDisplay(hdc);
    // Initialize EGL for this display, returns EGL version
    EGLint eglVersionMajor, eglVersionMinor;
    eglInitialize(eglDisplay, &eglVersionMajor, &eglVersionMinor);


    EGLint configAttributes[] =
    {
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



    setupScene()
    while (!quit) {
		renderScene();
		eglSwapBuffers(eglDisplay, eglSurface);
    }

