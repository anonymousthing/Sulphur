#pragma once

#include "PlatformDelegates.h"
#include "AsyncProgress.h"
#include "Thread.h"

class ThreadPool {
private:
	struct ThreadPoolEntry {
		Thread thread;

		volatile union {
			AsyncDelegate *asyncTask;
			Delegate *delegateTask;
			Method *task;
		};
		volatile int taskType;
		volatile void *taskArg;
		volatile AsyncProgress *progress;
		volatile bool startedTask;
		volatile bool busy;

		ThreadPoolEntry(Delegate *task) : thread(task, this, true) {
			startedTask = false;
			busy = false;
		}
	};
	
	std::vector<ThreadPoolEntry> threads;

	ThreadPoolEntry *getAvailableThread();

	static int threadPoolThreadEntry(void *_threadInfo);
	static void fence();
public:
	ThreadPool(int numThreads = 0);

	//Determines whether or not there are any available threads for dispatch.
	//If this returns false, then any calls to add tasks to this ThreadPool will fail.
	inline bool hasAvailableThread() { return getAvailableThread() != nullptr; }

	volatile AsyncProgress *queue(AsyncDelegate *target, void *arg);
	volatile AsyncProgress *queue(Method *target);
	volatile AsyncProgress *queue(Delegate *target, void *arg);

	template<typename T>
	struct ThreadIteratorArgs {
		typename T::iterator begin, end;
		void(*task)(typename T::value_type &);
	};

	template<typename T>
	static void runTaskOnIterators(AsyncProgress *progress, void *iterators) {
		typename ThreadIteratorArgs<T> *args = (ThreadIteratorArgs<T> *)iterators;
		void(*task)(typename T::value_type &) = args->task;

		for (auto ai = args->begin, ae = args->end; ai != ae; ++ai) {
			typename T::value_type &item = *ai;
			task(item);
		}
	}

	template<typename T>
	void launchThreadWithIterators(ThreadPoolEntry *threadInfo, typename T::iterator begin, typename T::iterator end, void(*task)(typename T::value_type &)) {
		typename ThreadIteratorArgs<T> *args = THREADSAFE_NEW(ThreadIteratorArgs<T>);

		args->begin = begin;
		args->end = end;
		args->task = task;

		fence();

		threadInfo->taskType = 0;
		threadInfo->asyncTask = &runTaskOnIterators<typename T>;
		threadInfo->taskArg = args;
		threadInfo->progress = new AsyncProgress(threadInfo->thread);
		threadInfo->busy = true;
		threadInfo->startedTask = false;

		fence();

		//TODO: Change Thread.resume to return whether or not the Thread was running when it was resumed.
		//Then, spin only while thread.resume indicates that the Thread was not sleeping.
		while (!threadInfo->startedTask)
			threadInfo->thread.resume();

		//Thread is running and has the task.
		//return threadInfo->progress;
	}

	//Runs the given method over every element in the given collection.
	//task is run as if it had the signature "void task(T &elem)"
	//This method blocks, utilising all Threads AND the calling thread.
	template<typename T>
	bool runOnCollection(T &collection, void (*task)(typename T::value_type &)); //TODO:

	//Runs the given method over every element in the given collection.
	//task is run as if it had the signature "void task(T &elem)"
	template<typename T>
	AsyncProgress *queueCollection(T &collection, void(*task)(typename T::value_type &)) {
		std::vector<ThreadPoolEntry*> availableThreads;

		while (ThreadPoolEntry *tpe = getAvailableThread())
			availableThreads.push_back(tpe);

		int numThreads = (int)availableThreads.size();
		if (numThreads == 0)
			return nullptr;
		
		int elemsPerThread = (int)collection.size() / numThreads;
		int numThreadsWithLeftover = (int)collection.size() % numThreads;
		
		//There will be numThreads groups.
		//numThreadsWithLeftover Threads will have (elemsPerThread+1) elements, the rest will have (elemsPerThread) elements.
		
		typename T::iterator first = collection.begin();
		for (int i = 0; i < numThreadsWithLeftover; i++) {
			typename T::iterator last = first + elemsPerThread + 1;
			launchThreadWithIterators<typename T>(availableThreads[i], first, last, task);
			first = last;
		}
		for (int i = numThreadsWithLeftover; i < numThreads; i++) {
			typename T::iterator last = first + elemsPerThread;
			launchThreadWithIterators<typename T>(availableThreads[i], first, last, task);
			first = last;
		}
		
		//TODO: Return progress.
		return nullptr;
	}
};
