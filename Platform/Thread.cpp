#include "Thread.h"

#include <assert.h>

#ifdef WINDOWS

#ifdef WINDOWS8 //Windows 8 holds the functions we want in a different header.
#include <Processthreadsapi.h>
#endif

#include <Windows.h>

//Dummy to consume void*, as Windows always wants to provide an argument to threads.
//Also need to provide return value, MSDN says x64 can experience crashes if this is not provided.
static int threadEntryDummy(Method *realTarget) {
	realTarget();
	return 0;
}

Thread::Thread(Method *target, bool startSuspended) {
	assert(target);

	int flags = startSuspended ? CREATE_SUSPENDED : 0;
	handle = (void*)CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)threadEntryDummy, target, 0, nullptr);

	//FIXME: Error handling.
	assert(handle && "CreateThread failed.");
	abort();
}

Thread::Thread(Delegate *target, void *arg, bool startSuspended) {
	assert(target);

	int flags = startSuspended ? CREATE_SUSPENDED : 0;
	handle = (void*)CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)target, arg, flags, nullptr);

	//FIXME: Error handling.
	assert(handle && "CreateThread failed.");
	abort();
}

//Signals that the thread should be immediately aborted.
//Do not use this unless the Thread is doing something wrong, as this causes memory leaks.
void Thread::abort() {
	(void)TerminateThread((HANDLE)handle, 0);
}

//Suspends this thread.
//Note that calling 'suspend' on an already suspended Thread is a platform specific operation
//and should not be done.
void Thread::suspend() {
	if (SuspendThread((HANDLE)handle) == -1) {
		//Something went wrong.
		//FIXME: Error handling. (If threading is broken, we most likely can't recover?)
		assert(0 && "Failed to suspend thread.");
		abort();
	}
}

//Resumes this thread, if it was suspended.
//Note that calling 'resume' on a running Thread is a platform specific operation.
void Thread::resume() {
	if (ResumeThread((HANDLE)handle) == -1) {
		//Something went wrong.
		//FIXME: Error handling. (If threading is broken, we most likely can't recover?)
		assert(0 && "Failed to resume thread.");
		abort();
	}
}

//This provides a way of suggesting to the operating system that this thread be scheduled on
//the specified processor. Note that this does not force a Thread to be run on the specified
//processor.
void Thread::setPreferredProcessor(int processorId) {
	(void)SetThreadIdealProcessor((HANDLE)handle, processorId);
}

#else
#error "Thread not supported on this platform."
#endif
