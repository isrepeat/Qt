#pragma once
#include <condition_variable>

namespace H {
	class UniqueThreadAccessor {
	public:
		UniqueThreadAccessor() = default;
		~UniqueThreadAccessor();

		void Lock();
		void Unlock();

		bool IsLocked();

	private:
		std::atomic<bool> locked = false;
		std::condition_variable cvLocker;
	};
}