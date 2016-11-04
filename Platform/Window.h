#pragma once

#include "Rectangle.h"
#include "TSQueue.h"
#include "Log.h"

enum class WindowMessageType {
	MouseMove,
	MouseEnter,
	MouseLeave,
	MouseClick,
	KeyDown,
	KeyUp
};

struct WindowMessage {
	WindowMessageType type;
	int param1, param2;
	void *param3;
	void *sender;
};

class Window {
private:
	//Platform specific handle type.
	//This is big enough for Windows, Linux & Mac.
	void *handle;

	//Cached copy of this Window's bounds.
	RectangleI bounds;

	//Cached copy of the Window's title.
	char *title;

	//Thread-safe access to the message queue.
	TSQueue<WindowMessage> msgQueue;

	Log log;

public:
	Window(RectangleI bounds);
	~Window();

	//Bounds accessors.
	inline SizeI getSize() { return bounds.getSize(); }
	inline void setSize(int width, int height) { setSize(SizeI(width, height)); }
	void setSize(SizeI size);

	inline PointI getLocation() { return bounds.getLocation(); }
	inline void setLocation(int x, int y) { setLocation(PointI(x, y)); }
	void setLocation(PointI location);

	inline RectangleI getBounds() { return bounds; }
	void setBounds(RectangleI bounds);

	inline TSQueue<WindowMessage> *getMessageQueue() { return &msgQueue; }

	//Title
	inline const char *getTitle() { return title; }
	void setTitle(const char *text);

	//Window visibility
	void show();
	void hide();

	//Can be run on any thread.
	//Signals that a running message loop should terminate.
	void signalClose();

	//Signals that the Window should process any underlying events and add them to its event queue.
	//This should only be called on the Thread that creates the Window.
	void processEvents();

	//Blocks this thread
	void startMessageLoop();
};
