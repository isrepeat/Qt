#pragma once
#include "Thread.h"
//#include <processthreadsapi.h>


namespace H {
	ThreadsFinishHelper::~ThreadsFinishHelper() {
		Stop();
	}

	void ThreadsFinishHelper::Register(std::weak_ptr<IThread> threadClass, std::wstring className) {
		std::lock_guard lk{ mx };
		PurgeExpiredPointers();

		threadsClassesWeak.push_back({ threadClass, className });
	}

	void ThreadsFinishHelper::Stop() {
		if (stopped)
			return;

		Visit([](std::shared_ptr<IThread> threadClass, const std::wstring& className) {
			threadClass->NotifyAboutStop();
			});


		Visit([](std::shared_ptr<IThread> threadClass, const std::wstring& className) {
			threadClass->WaitingFinishThreads();
			});

		stopped = true;
	}

	void ThreadsFinishHelper::Visit(std::function<void(std::shared_ptr<IThread>, const std::wstring&)> handler) {
		for (auto& [threadClassWeak, className] : threadsClassesWeak) {
			if (auto threadClass = threadClassWeak.lock()) {
				handler(threadClass, className);
			}
		}
	}
	void ThreadsFinishHelper::PurgeExpiredPointers() {
		auto newEnd = std::remove_if(threadsClassesWeak.begin(), threadsClassesWeak.end(), [](decltype(threadsClassesWeak.back())& pair) {
			return pair.first.expired();
			});
		threadsClassesWeak.erase(newEnd, threadsClassesWeak.end());
	}



	//thread_local std::wstring ThreadNameHelper::threadName = L"unnamed thread";

	//void ThreadNameHelper::SetThreadName(const std::wstring& name) {
	//	threadName = name;
	//	SetThreadDescription(GetCurrentThread(), threadName.c_str());
	//}

	//const std::wstring& ThreadNameHelper::GetThreadName() {
	//	return threadName;
	//}

	//size_t GetThreadId() {
	//	return static_cast<size_t>(::GetCurrentThreadId());
	//}
}