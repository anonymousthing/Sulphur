#ifdef WINDOWS

#include "Window.h"
#include "WindowsHelper.h"

LRESULT CALLBACK _wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CLOSE) {
		PostMessageA(hwnd, WM_QUIT, 0, 0);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static HWND createWindow(Log &log, RectangleI bounds) {
	DWORD style = WS_OVERLAPPEDWINDOW;

	WNDCLASSEXA data;
	ZeroMemory(&data, sizeof(data));
	data.cbSize = sizeof(data);
	data.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	data.lpszClassName = "defaultWindow";
	data.lpfnWndProc = _wndProc;
	ATOM wndClass = RegisterClassExA(&data);
	if (wndClass == 0) {
		logWindowsError("Call to RegisterClassExA failed.");
		log.fatal("Cannot continue -- critical API call failed.");
	}

	HWND handle = CreateWindowExA(0, (char*)wndClass, "", style,
		bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
		nullptr, nullptr, nullptr, nullptr);
	if (handle == 0) {
		logWindowsError("Call to CreateWindowExA failed.");
		log.fatal("Cannot continue -- critical API call failed.");
	}

	return handle;
}

Window::Window(RectangleI bounds) : log("Window") {
	handle = (void*)createWindow(log, bounds);
}
Window::~Window() {
	hide();
	//TODO: Destroy window
}

void Window::setSize(SizeI size) {
	bounds.setSize(size);

	WINAPI_ENSURE_NOT_ZERO(
		MoveWindow((HWND)handle, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), true),
		"Call to MoveWindow failed.");
}

void Window::setLocation(PointI location) {
	bounds.setLocation(location);

	WINAPI_ENSURE_NOT_ZERO(
		MoveWindow((HWND)handle, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), true),
		"Call to MoveWindow failed.");
}

void Window::setBounds(RectangleI bounds) {
	this->bounds = bounds;

	WINAPI_ENSURE_NOT_ZERO(
		MoveWindow((HWND)handle, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), true),
		"Call to MoveWindow failed.");
}

void Window::setTitle(const char *text) {
	//Release old text
	if (title) {
		free(title);
	}
	
	//Copy new text
	if (text) {
		size_t len = strlen(text);

		title = (char*)malloc(len + 1);
		memcpy(title, text, len);
		title[len] = 0;
	} else {
		title = (char*)malloc(1);
		title[0] = 0;
	}

	//Set title
	log.info("Setting title text.");
	SetWindowTextA((HWND)handle, title);
}

void Window::show() {
	log.info("Showing window.");
	ShowWindow((HWND)handle, SW_SHOW);
}

void Window::hide() {
	log.info("Hiding window.");
	ShowWindow((HWND)handle, SW_HIDE);
}

void Window::signalClose() {
	log.info("signalClose called... Posting quit message.");
	PostMessageA((HWND)handle, WM_QUIT, 0, 0);
}

void Window::processEvents() {
	MSG message;
	while (PeekMessageA(&message, (HWND)handle, 0, 0, TRUE)) {
		DispatchMessageA(&message);
	}
}

void Window::startMessageLoop() {
	MSG msg;
	BOOL bRet;
	while (bRet = GetMessageA(&msg, (HWND)handle, 0, 0)) {
		if (bRet == -1) {
			log.fatal("Call to GetMessageA failed.");
		}
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

#endif
