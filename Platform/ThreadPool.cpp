#include "ThreadPool.h"

#include <assert.h>

#ifdef WINDOWS
#ifdef WINDOWS8
#include <processthreadsapi.h>
#else
#include <Windows.h>
#endif
#else
#error "Must provide 'MemoryFence'"
#endif

//This is where 'worker' threads sit.
//TODO: Have a flag that causes worker threads to terminate.
int ThreadPool::threadPoolThreadEntry(void *_threadInfo) {
	ThreadPoolEntry *threadInfo = (ThreadPoolEntry*)_threadInfo;

	//Thread's wait loop.
	while (true) {
		//We shouldn't be woken until we have a task
		assert(threadInfo->task && threadInfo->busy && "No task or busy not set.");
		assert(threadInfo->startedTask == false && "Task already started.");

		//Take task
		fence();
		threadInfo->startedTask = true;
		fence();

		switch (threadInfo->taskType) {
		case 0:
			threadInfo->asyncTask((AsyncProgress*)threadInfo->progress, (void*)threadInfo->taskArg);
			break;
		case 1:
			threadInfo->delegateTask((void*)threadInfo->taskArg);

			//Handle AsyncProgress for functions that don't support them.
			threadInfo->progress->progress = 1000;
			threadInfo->progress->completed = true;
			break;
		case 2:
			threadInfo->task();

			//Handle AsyncProgress for functions that don't support them.
			threadInfo->progress->progress = 1000;
			threadInfo->progress->completed = true;
			break;
		default:
			assert(0 && "Invalid task type.");
			break;
		}

		fence();
		threadInfo->busy = false;
		fence();
		threadInfo->thread.suspend();
	}

	return 0;
}

ThreadPool::ThreadPoolEntry *ThreadPool::getAvailableThread() {
	for (auto tpei = threads.begin(), tpee = threads.end(); tpei != tpee; ++tpei) {
		ThreadPoolEntry &tpe = *tpei;

		if (!tpe.busy) {
			return &tpe;
		}
	}

	return nullptr;
}

void ThreadPool::fence() {
	MemoryFence();
}

ThreadPool::ThreadPool(int numThreads) {
	for (int i = 0; i < numThreads; i++) {
		threads.push_back(ThreadPoolEntry(&threadPoolThreadEntry));
	}
}

volatile AsyncProgress *ThreadPool::queue(AsyncDelegate *target, void *arg) {
	ThreadPoolEntry *threadInfo = getAvailableThread();

	//No available threads.
	if (threadInfo == nullptr)
		return nullptr;

	fence();

	threadInfo->taskType = 0;
	threadInfo->asyncTask = target;
	threadInfo->taskArg = arg;
	threadInfo->progress = new AsyncProgress(threadInfo->thread);
	threadInfo->busy = true;
	threadInfo->startedTask = false;

	fence();

	//TODO: Change Thread.resume to return whether or not the Thread was running when it was resumed.
	//Then, spin only while thread.resume indicates that the Thread was not sleeping.
	while (!threadInfo->startedTask)
		threadInfo->thread.resume();

	//Thread is running and has the task.
	return threadInfo->progress;
}

volatile AsyncProgress *ThreadPool::queue(Method *target) {
	ThreadPoolEntry *threadInfo = getAvailableThread();

	//No available threads.
	if (threadInfo == nullptr)
		return nullptr;

	fence();

	threadInfo->taskType = 2;
	threadInfo->task = target;
	threadInfo->progress = new AsyncProgress(threadInfo->thread);
	threadInfo->busy = true;
	threadInfo->startedTask = false;

	fence();

	//TODO: Change Thread.resume to return whether or not the Thread was running when it was resumed.
	//Then, spin only while thread.resume indicates that the Thread was not sleeping.
	while (!threadInfo->startedTask)
		threadInfo->thread.resume();

	//Thread is running and has the task.
	return threadInfo->progress;
}

volatile AsyncProgress *ThreadPool::queue(Delegate *target, void *arg) {
	ThreadPoolEntry *threadInfo = getAvailableThread();
	
	//No available threads.
	if (threadInfo == nullptr)
		return nullptr;

	fence();

	threadInfo->taskType = 1;
	threadInfo->delegateTask = target;
	threadInfo->taskArg = arg;
	threadInfo->progress = new AsyncProgress(threadInfo->thread);
	threadInfo->busy = true;
	threadInfo->startedTask = false;

	fence();

	//TODO: Change Thread.resume to return whether or not the Thread was running when it was resumed.
	//Then, spin only while thread.resume indicates that the Thread was not sleeping.
	while (!threadInfo->startedTask)
		threadInfo->thread.resume();

	//Thread is running and has the task.
	return threadInfo->progress;
}
