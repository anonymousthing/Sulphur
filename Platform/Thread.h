#pragma once

#include "PlatformDelegates.h"

//TODO: Move these to an "Allocation" header.
#define THREADSAFE_NEW(type)			(new type())
#define THREADSAFE_DELETE(type, var)	delete (var)

//A platform specific Thread implementation.
class Thread {
private:
	//This is a platform specific handle to this thread.
	//For platforms that need more than a pointer_t, this can merely point to memory allocated
	//by the constructor. Note that both Windows and Linux will support placing a Thread handle
	//in a void* (On Windows, HANDLE is a typedef for "void *")
	void *handle;

public:
	//Creates and executes a new thread with the given target.
	Thread(Method *target, bool startSuspended = false);

	//Creates and executes a new thread with the given target.
	Thread(Delegate *target, void *arg, bool startSuspended = false);

	//TODO: A variadically templated Thread constructor.

	//This provides a way of suggesting to the operating system that this thread be scheduled on
	//the specified processor. Note that this does not force a Thread to be run on the specified
	//processor.
	void setPreferredProcessor(int processorId);

	//Signals that the thread should be immediately aborted.
	//Do not use this unless the Thread is doing something wrong, as this causes memory leaks.
	void abort();

	//Suspends this thread.
	//Note that calling 'suspend' on an already suspended Thread is a platform specific operation
	//and should not be done.
	void suspend();

	//Resumes this thread, if it was suspended.
	//Note that calling 'resume' on a running Thread is a platform specific operation.
	void resume();
};
